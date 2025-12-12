#ifndef CCA_PROTOCOL_H
#define CCA_PROTOCOL_H

#include <stdint.h>


#define CHALLENGE_SIZE   64
#define TOKEN_SIZE       256
#define MEASUREMENT_SIZE 64

typedef struct {
    uint8_t challenge[CHALLENGE_SIZE];
} AttestationRequest;

typedef struct {
    uint8_t token[TOKEN_SIZE];
    uint8_t measurement[MEASUREMENT_SIZE];
    uint32_t status_code;
} AttestationResponse;

void rsi_get_token(const uint8_t* challenge, uint8_t* out_buffer);
void rsi_get_measurement(uint8_t* out_buffer);

#endif
