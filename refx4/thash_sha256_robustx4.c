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
    unsigned char buf0[SPX_N + SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf1[SPX_N + SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf2[SPX_N + SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char buf3[SPX_N + SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char outbuf0[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf1[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf2[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf3[SPX_SHA256_OUTPUT_BYTES];
    unsigned char bitmask0[inblocks * SPX_N];
    unsigned char bitmask1[inblocks * SPX_N];
    unsigned char bitmask2[inblocks * SPX_N];
    unsigned char bitmask3[inblocks * SPX_N];
    uint8_t sha2_state0[32 + 8];
    uint8_t sha2_state1[32 + 8];
    uint8_t sha2_state2[32 + 8];
    uint8_t sha2_state3[32 + 8];
    unsigned int i;

    // Copy SPX_N bytes of seed into bufs
    memcpy(buf0, pub_seed, SPX_N);
    memcpy(buf1, pub_seed, SPX_N);
    memcpy(buf2, pub_seed, SPX_N);
    memcpy(buf3, pub_seed, SPX_N);

    // Copy SPX_SHA256_ADDR_BYTES of addr into bufs
    memcpy(buf0 + SPX_N, addrx4 + 0 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf1 + SPX_N, addrx4 + 1 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf2 + SPX_N, addrx4 + 2 * 8, SPX_SHA256_ADDR_BYTES);
    memcpy(buf3 + SPX_N, addrx4 + 3 * 8, SPX_SHA256_ADDR_BYTES);

    // Generate bitmask
    mgf1_256(bitmask0, inblocks * SPX_N, buf0, SPX_N + SPX_SHA256_ADDR_BYTES);
    mgf1_256(bitmask1, inblocks * SPX_N, buf1, SPX_N + SPX_SHA256_ADDR_BYTES);
    mgf1_256(bitmask2, inblocks * SPX_N, buf2, SPX_N + SPX_SHA256_ADDR_BYTES);
    mgf1_256(bitmask3, inblocks * SPX_N, buf3, SPX_N + SPX_SHA256_ADDR_BYTES);

    // Apply bitmask
    for (i = 0; i < inblocks * SPX_N; i++) {
        buf0[SPX_N + SPX_SHA256_ADDR_BYTES + i] =
            in0[i] ^ bitmask0[i];
        buf1[SPX_N + SPX_SHA256_ADDR_BYTES + i] =
            in1[i] ^ bitmask1[i];
        buf2[SPX_N + SPX_SHA256_ADDR_BYTES + i] =
            in2[i] ^ bitmask2[i];
        buf3[SPX_N + SPX_SHA256_ADDR_BYTES + i] =
            in3[i] ^ bitmask3[i];
    }

    // Copy state
    for (i = 0; i < 40; i++) {
        sha2_state0[i] = state_seeded[i];
        sha2_state1[i] = state_seeded[i];
        sha2_state2[i] = state_seeded[i];
        sha2_state3[i] = state_seeded[i];
    }

    sha256_inc_finalize(outbuf0, sha2_state0, buf0 + SPX_N, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf1, sha2_state1, buf1 + SPX_N, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf2, sha2_state2, buf2 + SPX_N, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    sha256_inc_finalize(outbuf3, sha2_state3, buf3 + SPX_N, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);

    memcpy(out0, outbuf0, SPX_N);
    memcpy(out1, outbuf1, SPX_N);
    memcpy(out2, outbuf2, SPX_N);
    memcpy(out3, outbuf3, SPX_N);
}
