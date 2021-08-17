#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "sha256.h"
//#include "sha256_neon.h"
//#include "sha256_neon11.h"

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
    uint8_t sha2_state0[32 + 8];
    uint8_t sha2_state1[32 + 8];
    uint8_t sha2_state2[32 + 8];
    uint8_t sha2_state3[32 + 8];

    (void)pub_seed; /* Suppress an 'unused parameter' warning. */

    // Copy state
    for (int i = 0; i < 40; i++) {
        sha2_state0[i] = state_seeded[i];
        sha2_state1[i] = state_seeded[i];
        sha2_state2[i] = state_seeded[i];
        sha2_state3[i] = state_seeded[i];
    }

    // Copy arrdess into bufs
    memcpy(buf0, addrx4 +  0, SPX_SHA256_ADDR_BYTES);
    memcpy(buf1, addrx4 +  8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf2, addrx4 + 16, SPX_SHA256_ADDR_BYTES);
    memcpy(buf3, addrx4 + 24, SPX_SHA256_ADDR_BYTES);

    // Copy message into bufs
    memcpy(buf0 + SPX_SHA256_ADDR_BYTES, in0, inblocks * SPX_N);
    memcpy(buf1 + SPX_SHA256_ADDR_BYTES, in1, inblocks * SPX_N);
    memcpy(buf2 + SPX_SHA256_ADDR_BYTES, in2, inblocks * SPX_N);
    memcpy(buf3 + SPX_SHA256_ADDR_BYTES, in3, inblocks * SPX_N);

    // Apply SHA-256
    sha256_inc_finalize(outbuf0, sha2_state0, buf0, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf1, sha2_state1, buf1, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf2, sha2_state2, buf2, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf3, sha2_state3, buf3, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);

    memcpy(out0, outbuf0, SPX_N);
    memcpy(out1, outbuf1, SPX_N);
    memcpy(out2, outbuf2, SPX_N);
    memcpy(out3, outbuf3, SPX_N);
}
