#ifndef SHA256_NEON3_H
#define SHA256_NEON3_H 

#include <stddef.h>
#include <stdint.h>
#include <arm_neon.h>

/* The incremental API allows hashing of individual input blocks; these blocks
    must be exactly 64 bytes each.
    Use the 'finalize' functions for any remaining bytes (possibly over 64). */

#define PQC_sha256ctx2_BYTES 32
#define SPX_SHA256_OUTPUT_BYTES 32
/* Structure for the incremental API */
typedef struct {
    uint8_t ctx[4*PQC_sha256ctx2_BYTES + 8];
} sha256ctx2;

/* ====== SHA256 API ==== */

/**
 * Initialize the incremental hashing API
 */
void sha256x4_inc_init(sha256ctx2 *state);
/**
 * Absorb blocks
 */
void sha256x4_inc_blocks(sha256ctx2 *state, const uint8_t *in, size_t inblocks);

/**
 * Finalize and obtain the digest
 *
 * If applicable, this function will free the memory associated with the sha256ctx2.
 */
void sha256x4_inc_finalize(
                            uint8_t *out0,
                            uint8_t *out1,
                            uint8_t *out2,
                            uint8_t *out3,
                            uint8_t *state,
                            const uint8_t *in0,
                            const uint8_t *in1,
                            const uint8_t *in2,
                            const uint8_t *in3,
                            size_t inlen);
/**
 * All-in-one sha256 function
 */
void sha256x42(
                    uint8_t *out0,
                    uint8_t *out1,
                    uint8_t *out2,
                    uint8_t *out3,
                    const uint8_t *in0,
                    const uint8_t *in1,
                    const uint8_t *in2,
                    const uint8_t *in3,
                    size_t inlen);

void mgf1x42(
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

#endif