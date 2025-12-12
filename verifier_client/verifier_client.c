#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include "attest_protocol.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define OUTPUT_TOKEN_NAME "cca_example_token.cbor"
#define OUTPUT_MEASURE_NAME "cca_example_measurement"

void print_hex(const char* label, const uint8_t* data, size_t size) {
    printf("%s:\n", label);
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

void save_to_file(const char* filename, const uint8_t* data, size_t size) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to open file for writing");
        return;
    }
    size_t written = fwrite(data, 1, size, fp);
    fclose(fp);
    printf("[Client] Token saved to '%s' (%zu bytes)\n", filename, written);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    AttestationRequest req;
    AttestationResponse res;
    
    srand(time(NULL));
    printf("=== Client: Starting Attestation Flow (C Version) ===\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed. Is the Realm Server running?");
        return -1;
    }

    memset(&req, 0, sizeof(req));
    for (int i = 0; i < CHALLENGE_SIZE; ++i) {
        req.challenge[i] = rand() % 256;
    }
    print_hex("[Client] Generated Challenge", req.challenge, 16); // 앞 16바이트만 출력

    if (send(sock, &req, sizeof(req), 0) < 0) {
        perror("Send failed");
        close(sock);
        return -1;
    }

    // Need update for reading entire measurement and token.
    ssize_t valread = read(sock, &res, sizeof(res));
    if (valread < 0) {
        perror("Read failed");
        close(sock);
        return -1;
    }

    if (res.status_code == 1) {
        printf("\n[Client] Attestation Successful!\n");
        print_hex("[Client] Received Token (First 32 bytes)", res.token, 32);
        print_hex("[Client] Received Measurement (First 32 bytes)", res.measurement, 32);

        save_to_file(OUTPUT_TOKEN_NAME, res.token, TOKEN_SIZE);
        save_to_file(OUTPUT_MEASURE_NAME, res.measurement, MEASUREMENT_SIZE);
    
    } else {
        printf("[Client] Attestation Failed with status: %d\n", res.status_code);
    }

    close(sock);
    return 0;
}
