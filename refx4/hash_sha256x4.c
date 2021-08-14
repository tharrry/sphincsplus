#include <stdint.h>
#include <string.h>

#include "address.h"
#include "utils.h"
#include "params.h"
#include "hashx4.h"
#include "sha256.h"
#include "sha256_neon.h"



/*
 * 4-way parallel version of prf_addr; takes 4x as much input and output
 */
void prf_addrx4(unsigned char *out0,
                unsigned char *out1,
                unsigned char *out2,
                unsigned char *out3,
                const unsigned char *key,
                const uint32_t addrx4[4*8])
{
    unsigned char bufx4[4 * (SPX_N + SPX_SHA256_ADDR_BYTES)];
    unsigned char outbufx4[4 * SPX_SHA256_OUTPUT_BYTES];
    unsigned int j;

    for (j = 0; j < 4; j++) {
        memcpy(bufx4 + j*(SPX_N + SPX_SHA256_ADDR_BYTES), key, SPX_N);
        memcpy(bufx4 + SPX_N + j*(SPX_N + SPX_SHA256_ADDR_BYTES),
                         addrx4 + j*8, SPX_SHA256_ADDR_BYTES);
    }

    sha256x4(outbufx4 + 0*SPX_SHA256_OUTPUT_BYTES,
                outbufx4 + 1*SPX_SHA256_OUTPUT_BYTES,
                outbufx4 + 2*SPX_SHA256_OUTPUT_BYTES,
                outbufx4 + 3*SPX_SHA256_OUTPUT_BYTES,
                bufx4 + 0*(SPX_N + SPX_SHA256_ADDR_BYTES),
                bufx4 + 1*(SPX_N + SPX_SHA256_ADDR_BYTES),
                bufx4 + 2*(SPX_N + SPX_SHA256_ADDR_BYTES),
                bufx4 + 3*(SPX_N + SPX_SHA256_ADDR_BYTES),
                SPX_N + SPX_SHA256_ADDR_BYTES);

    memcpy(out0, outbufx4 + 0*SPX_SHA256_OUTPUT_BYTES, SPX_N);
    memcpy(out1, outbufx4 + 1*SPX_SHA256_OUTPUT_BYTES, SPX_N);
    memcpy(out2, outbufx4 + 2*SPX_SHA256_OUTPUT_BYTES, SPX_N);
    memcpy(out3, outbufx4 + 3*SPX_SHA256_OUTPUT_BYTES, SPX_N);
}