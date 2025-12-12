#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "cca_protocol.h"

#define PORT 8080


void rsi_get_token(const uint8_t* challenge, uint8_t* out_buffer) {
    const char* prefix = "REALM_CCA_TOKEN_SIG_";
    size_t prefix_len = strlen(prefix);
    memset(out_buffer, 0, TOKEN_SIZE);

    memcpy(out_buffer, prefix, prefix_len);

    for (size_t i = 0; i < CHALLENGE_SIZE; ++i) {
        if (prefix_len + i < TOKEN_SIZE) {
            out_buffer[prefix_len + i] = challenge[i] ^ 0xAA;
        }
    }

    printf("[Internal RSI] Token generated based on challenge.\n");
}

void rsi_get_measurement(uint8_t* out_buffer) {
    const char* boot_hash = "DEADBEEF-BOOT-MEASUREMENT-SHA512-SIMULATION";
    memset(out_buffer, 0xEE, MEASUREMENT_SIZE);
    size_t copy_len = strlen(boot_hash);
    if (copy_len > MEASUREMENT_SIZE) copy_len = MEASUREMENT_SIZE;
    memcpy(out_buffer, boot_hash, copy_len);
    printf("[Internal RSI] Measurement read from RIM/REM registers.\n");
}

void handle_client(int client_sock) {
    AttestationRequest req;
    AttestationResponse res;
    ssize_t valread;

    memset(&req, 0, sizeof(req));
    memset(&res, 0, sizeof(res));

    valread = read(client_sock, &req, sizeof(req));
    if (valread <= 0) {
        perror("read failed or connection closed");
        close(client_sock);
        return;
    }

    printf("[Server] Received Challenge from Client.\n");

    rsi_get_token(req.challenge, res.token);
    rsi_get_measurement(res.measurement);

    res.status_code = 1;

    send(client_sock, &res, sizeof(res), 0);
    printf("[Server] Sent Attestation Report to Client.\n");

    close(client_sock);
}

int main(void) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf("=== Realm Attestation Server Starting... ===\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("=== Listening on Port %d ===\n", PORT);

    while (1) {
        printf("\n[Server] Waiting for connection...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        handle_client(new_socket);
    }

    return 0;
}
