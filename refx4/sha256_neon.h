#ifndef SHA256_NEON_H
#define SHA256_NEON_H 

//#define SPX_SHA256_OUTPUT_BYTES 32

void sha256x4(
    uint8_t *digest0,
    uint8_t *digest1,
    uint8_t *digest2,
    uint8_t *digest3,
    uint8_t *message0,
    uint8_t *message1,
    uint8_t *message2,
    uint8_t *message3,
    uint64_t mlen);

void sha256x4_seeded(
    uint8_t *digest0,
    uint8_t *digest1,
    uint8_t *digest2,
    uint8_t *digest3,
    uint8_t *seed,
    uint8_t *message0,
    uint8_t *message1,
    uint8_t *message2,
    uint8_t *message3,
    uint64_t mlen);

void mgf1x4(
            unsigned char *out0,
            unsigned char *out1,
            unsigned char *out2,
            unsigned char *out3,
            unsigned long outlen,
            const unsigned char *in0,
            const unsigned char *in1,
            const unsigned char *in2,
            const unsigned char *in3,
            unsigned long inlen);
#endif /* SHA256_NEON_H */