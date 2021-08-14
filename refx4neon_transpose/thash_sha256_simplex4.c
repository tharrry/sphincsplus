#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "sha256.h"
#include "sha256_neon.h"
#include "sha256_neon11.h"

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thashx4(
            unsigned char *out0,
            unsigned char *out1,
            unsigned char *out2,
            unsigned char *out3,
            const unsigned char *in0,
            const unsigned char *in1,
            const unsigned char *in2,
            const unsigned char *in3,
            unsigned int inblocks,
            const unsigned char *pub_seed,
            uint32_t addrx4[4*8])
{
    unsigned char buf0[SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf1[SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf2[SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf3[SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char outbuf0[SPX_SHA256_OUTPUT_BYTES]; 
    unsigned char outbuf1[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf2[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf3[SPX_SHA256_OUTPUT_BYTES];
    uint8_t sha2_statex4[4*32+8];

    (void)pub_seed; /* Suppress an 'unused parameter' warning. */
    // Copy SPX_SHA256_ADDR_BYTES of addr into bufs
    memcpy(buf0, addrx4 + 0 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf1, addrx4 + 1 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf2, addrx4 + 2 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf3, addrx4 + 3 * 8, SPX_SHA256_ADDR_BYTES);

    

    memcpy(buf0 + SPX_SHA256_ADDR_BYTES, in0, inblocks*SPX_N);
    memcpy(buf1 + SPX_SHA256_ADDR_BYTES, in1, inblocks*SPX_N);
    memcpy(buf2 + SPX_SHA256_ADDR_BYTES, in2, inblocks*SPX_N);
    memcpy(buf3 + SPX_SHA256_ADDR_BYTES, in3, inblocks*SPX_N);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 32; j++) {
            sha2_statex4[i*32 + j] = state_seeded[j];
        }
    }
    for (int i = 0; i < 8; i++) {
        sha2_statex4[4*32 + i] = state_seeded[32+i];
    }

    sha256x4_inc_finalize(
                    outbuf0,
                    outbuf1,
                    outbuf2,
                    outbuf3,
                    sha2_statex4,
                    buf0,
                    buf1,
                    buf2,
                    buf3,
                    SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);

    memcpy(out0, outbuf0, SPX_N);
    memcpy(out1, outbuf1, SPX_N);
    memcpy(out2, outbuf2, SPX_N);
    memcpy(out3, outbuf3, SPX_N);
}
