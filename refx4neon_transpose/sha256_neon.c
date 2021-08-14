#include<stdio.h>
#include<string.h>
#include<arm_neon.h>
#include "sha256_neon.h"
#include "sha256.h"
#include "utils.h"

typedef struct state_t{
    uint32x4_t registers[8];
} state;
uint32x4_t digest[8];

uint8_t* ms0;
uint8_t* ms1;
uint8_t* ms2;
uint8_t* ms3;

uint32_t iv[] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};

uint32x4_t k[] = {
    {0x428a2f98,0x428a2f98,0x428a2f98,0x428a2f98}, {0x71374491,0x71374491,0x71374491,0x71374491}, {0xb5c0fbcf,0xb5c0fbcf,0xb5c0fbcf,0xb5c0fbcf}, {0xe9b5dba5,0xe9b5dba5,0xe9b5dba5,0xe9b5dba5}, {0x3956c25b,0x3956c25b,0x3956c25b,0x3956c25b}, {0x59f111f1,0x59f111f1,0x59f111f1,0x59f111f1}, {0x923f82a4,0x923f82a4,0x923f82a4,0x923f82a4}, {0xab1c5ed5,0xab1c5ed5,0xab1c5ed5,0xab1c5ed5},
    {0xd807aa98,0xd807aa98,0xd807aa98,0xd807aa98}, {0x12835b01,0x12835b01,0x12835b01,0x12835b01}, {0x243185be,0x243185be,0x243185be,0x243185be}, {0x550c7dc3,0x550c7dc3,0x550c7dc3,0x550c7dc3}, {0x72be5d74,0x72be5d74,0x72be5d74,0x72be5d74}, {0x80deb1fe,0x80deb1fe,0x80deb1fe,0x80deb1fe}, {0x9bdc06a7,0x9bdc06a7,0x9bdc06a7,0x9bdc06a7}, {0xc19bf174,0xc19bf174,0xc19bf174,0xc19bf174},
    {0xe49b69c1,0xe49b69c1,0xe49b69c1,0xe49b69c1}, {0xefbe4786,0xefbe4786,0xefbe4786,0xefbe4786}, {0x0fc19dc6,0x0fc19dc6,0x0fc19dc6,0x0fc19dc6}, {0x240ca1cc,0x240ca1cc,0x240ca1cc,0x240ca1cc}, {0x2de92c6f,0x2de92c6f,0x2de92c6f,0x2de92c6f}, {0x4a7484aa,0x4a7484aa,0x4a7484aa,0x4a7484aa}, {0x5cb0a9dc,0x5cb0a9dc,0x5cb0a9dc,0x5cb0a9dc}, {0x76f988da,0x76f988da,0x76f988da,0x76f988da},
    {0x983e5152,0x983e5152,0x983e5152,0x983e5152}, {0xa831c66d,0xa831c66d,0xa831c66d,0xa831c66d}, {0xb00327c8,0xb00327c8,0xb00327c8,0xb00327c8}, {0xbf597fc7,0xbf597fc7,0xbf597fc7,0xbf597fc7}, {0xc6e00bf3,0xc6e00bf3,0xc6e00bf3,0xc6e00bf3}, {0xd5a79147,0xd5a79147,0xd5a79147,0xd5a79147}, {0x06ca6351,0x06ca6351,0x06ca6351,0x06ca6351}, {0x14292967,0x14292967,0x14292967,0x14292967},
    {0x27b70a85,0x27b70a85,0x27b70a85,0x27b70a85}, {0x2e1b2138,0x2e1b2138,0x2e1b2138,0x2e1b2138}, {0x4d2c6dfc,0x4d2c6dfc,0x4d2c6dfc,0x4d2c6dfc}, {0x53380d13,0x53380d13,0x53380d13,0x53380d13}, {0x650a7354,0x650a7354,0x650a7354,0x650a7354}, {0x766a0abb,0x766a0abb,0x766a0abb,0x766a0abb}, {0x81c2c92e,0x81c2c92e,0x81c2c92e,0x81c2c92e}, {0x92722c85,0x92722c85,0x92722c85,0x92722c85},
    {0xa2bfe8a1,0xa2bfe8a1,0xa2bfe8a1,0xa2bfe8a1}, {0xa81a664b,0xa81a664b,0xa81a664b,0xa81a664b}, {0xc24b8b70,0xc24b8b70,0xc24b8b70,0xc24b8b70}, {0xc76c51a3,0xc76c51a3,0xc76c51a3,0xc76c51a3}, {0xd192e819,0xd192e819,0xd192e819,0xd192e819}, {0xd6990624,0xd6990624,0xd6990624,0xd6990624}, {0xf40e3585,0xf40e3585,0xf40e3585,0xf40e3585}, {0x106aa070,0x106aa070,0x106aa070,0x106aa070},
    {0x19a4c116,0x19a4c116,0x19a4c116,0x19a4c116}, {0x1e376c08,0x1e376c08,0x1e376c08,0x1e376c08}, {0x2748774c,0x2748774c,0x2748774c,0x2748774c}, {0x34b0bcb5,0x34b0bcb5,0x34b0bcb5,0x34b0bcb5}, {0x391c0cb3,0x391c0cb3,0x391c0cb3,0x391c0cb3}, {0x4ed8aa4a,0x4ed8aa4a,0x4ed8aa4a,0x4ed8aa4a}, {0x5b9cca4f,0x5b9cca4f,0x5b9cca4f,0x5b9cca4f}, {0x682e6ff3,0x682e6ff3,0x682e6ff3,0x682e6ff3},
    {0x748f82ee,0x748f82ee,0x748f82ee,0x748f82ee}, {0x78a5636f,0x78a5636f,0x78a5636f,0x78a5636f}, {0x84c87814,0x84c87814,0x84c87814,0x84c87814}, {0x8cc70208,0x8cc70208,0x8cc70208,0x8cc70208}, {0x90befffa,0x90befffa,0x90befffa,0x90befffa}, {0xa4506ceb,0xa4506ceb,0xa4506ceb,0xa4506ceb}, {0xbef9a3f7,0xbef9a3f7,0xbef9a3f7,0xbef9a3f7}, {0xc67178f2,0xc67178f2,0xc67178f2,0xc67178f2}};

//void u32_to_bytes(unsigned char *out, uint32_t in)
//{
//    out[0] = (unsigned char)(in >> 24);
//    out[1] = (unsigned char)(in >> 16);
//    out[2] = (unsigned char)(in >> 8);
//    out[3] = (unsigned char)in;
//}

static void to_little_endian(uint64_t len, uint8_t* t) {
    // Convert little endian char* from uint64_t.

    t[7] = (uint8_t)len;
    t[6] = len >> 8;
    t[5] = len >> 16;
    t[4] = len >> 24;
    t[3] = len >> 32;
    t[2] = len >> 40;
    t[1] = len >> 48;
    t[0] = len >> 56;
}

static uint32_t load4be(uint8_t* msg) {
    // Load 4 bytes of message big-endian-ly
    return msg[3] ^ (msg[2] << 8) ^ (msg[1] << 16) ^ (msg[0] << 24);
}

static void make_message_schedule(
    uint32x4_t *w,
    uint8_t *msg0, uint8_t *msg1, uint8_t *msg2, uint8_t *msg3){
    // Copy 64 bytes of message into message schedule
    for (int counter = 0; counter < 16; counter++) {

        w[counter][0] = load4be(msg0 + 4*counter);
        w[counter][1] = load4be(msg1 + 4*counter);
        w[counter][2] = load4be(msg2 + 4*counter);
        w[counter][3] = load4be(msg3 + 4*counter);
    }
    // Generate remaining message schedule
    for (int counter = 16; counter < 64; counter++) {
        // (w[i-15] >> 7) xor (w[i-15] >> 18) xor (w[i-15] >>> 3)
        uint32x4_t sig0 =   veorq_u32(
                            veorq_u32(
                                veorq_u32(vshlq_n_u32(w[counter-15], 32 - 7), vshrq_n_u32(w[counter-15], 7)),
                                vshrq_n_u32(w[counter-15], 3)),
                            veorq_u32(vshlq_n_u32(w[counter-15], 32 - 18), vshrq_n_u32(w[counter-15], 18))
                        );
        // (w[i-2] >> 17) xor (w[i-2] >> 19) xor (w[i-2] >>> 10)
        uint32x4_t sig1 =   veorq_u32(
                            veorq_u32(
                                veorq_u32(vshlq_n_u32(w[counter-2], 32 - 17), vshrq_n_u32(w[counter-2], 17)),
                                vshrq_n_u32(w[counter-2], 10)),
                            veorq_u32(vshlq_n_u32(w[counter-2], 32 - 19), vshrq_n_u32(w[counter-2], 19))
                        );
        // w[i] = (w[i-16] + sig0) + (w[i-7] + s1)
        w[counter] = 
            vaddq_u32(
                vaddq_u32(
                    w[counter-16],
                    sig0
                ),
                vaddq_u32(
                    w[counter-7],
                    sig1
                )
            );
    }
}

static void sha_updatex4(state* state){
    uint32x4_t w[64] = {0};
    uint32x4_t ch, maj, sig0, sig1, t1, t2;
    uint32x4_t a, b, c, d, e, f, g, h;
    make_message_schedule(w, ms0, ms1, ms2, ms3);

    a = state->registers[0];
    b = state->registers[1];
    c = state->registers[2];
    d = state->registers[3];
    e = state->registers[4];
    f = state->registers[5];
    g = state->registers[6];
    h = state->registers[7];

    for (int i = 0; i < 64; i++) {
        // (e and f) xor ( (xor(ffffffff, e)) and g )
        ch      =   veorq_u32(
                    vandq_u32(e, f),
                    vandq_u32(
                        veorq_u32(vdupq_n_u32(0xffffffff), e),
                        g
                    )
                );
        // (a and b) xor (a and c) xor (b and c)
        maj     =   veorq_u32(
                    veorq_u32(
                        vandq_u32(a,b),
                        vandq_u32(a,c)),
                    vandq_u32(b,c)
                );
        // (a >>> 2) xor (a >>> 13) xor (a >>> 22)
        sig0    =   veorq_u32(
                    veorq_u32(
                        veorq_u32(vshlq_n_u32(a, 32 - 2), vshrq_n_u32(a, 2)),
                        veorq_u32(vshlq_n_u32(a, 32 - 22), vshrq_n_u32(a, 22))),
                    veorq_u32(vshlq_n_u32(a, 32 - 13), vshrq_n_u32(a, 13))
                );
        // (e >>> 6) xor (e >>> 11) xor (e >>> 25)
        sig1    =   veorq_u32(
                    veorq_u32(
                        veorq_u32(vshlq_n_u32(e, 32 - 6), vshrq_n_u32(e, 6)),
                        veorq_u32(vshlq_n_u32(e, 32 - 25), vshrq_n_u32(e, 25))),
                    veorq_u32(vshlq_n_u32(e, 32 - 11), vshrq_n_u32(e, 11))
                );
        // ((h, ch) + (w, k)) + sig1
        t1 = vaddq_u32( vaddq_u32( vaddq_u32(h, ch), vaddq_u32(w[i], k[i])), sig1);
        t2 = vaddq_u32(sig0, maj);

        h = g;
        g = f;
        f = e;
        e = vaddq_u32(d, t1);
        d = c;
        c = b;
        b = a;
        a = vaddq_u32(t1, t2);
    }
    state->registers[0] = vaddq_u32(a,state->registers[0]);
    state->registers[1] = vaddq_u32(b,state->registers[1]);
    state->registers[2] = vaddq_u32(c,state->registers[2]);
    state->registers[3] = vaddq_u32(d,state->registers[3]);
    state->registers[4] = vaddq_u32(e,state->registers[4]);
    state->registers[5] = vaddq_u32(f,state->registers[5]);
    state->registers[6] = vaddq_u32(g,state->registers[6]);
    state->registers[7] = vaddq_u32(h,state->registers[7]);
}

static void sha_digest(state* state, uint64_t mlen) {
    uint64_t mlen2 = mlen;
    uint8_t buff0[64];
    uint8_t buff1[64];
    uint8_t buff2[64];
    uint8_t buff3[64];

    while (mlen >= 64){
        sha_updatex4(state);
        ms0 = ms0 + 64;
        ms1 = ms1 + 64;
        ms2 = ms2 + 64;
        ms3 = ms3 + 64;
        mlen = mlen - 64;
    }

    // copy remaining message into buffers
    for (uint64_t counter = 0; counter < mlen; counter++){
        buff0[counter] = ms0[counter];
        buff1[counter] = ms1[counter];
        buff2[counter] = ms2[counter];
        buff3[counter] = ms3[counter];
    }
    //append '1' bit after message
    buff0[mlen] = 128;
    buff1[mlen] = 128;
    buff2[mlen] = 128;
    buff3[mlen] = 128;

    ms0 = &buff0[0];
    ms1 = &buff1[0];
    ms2 = &buff2[0];
    ms3 = &buff3[0];

    //No space left for size bytes
    if (mlen > 55) {
        for (int counter = mlen+1; counter < 64; counter++) {
            buff0[counter] = 0;
            buff1[counter] = 0;
            buff2[counter] = 0;
            buff3[counter] = 0;
        }
        sha_updatex4(state);

        mlen = 0;
        buff0[0] = 0;
        buff1[0] = 0;
        buff2[0] = 0;
        buff3[0] = 0;
    }
    
    for(int counter = mlen + 1; counter < 56; counter++){
        buff0[counter] = 0;
        buff1[counter] = 0;
        buff2[counter] = 0;
        buff3[counter] = 0;
    }

    uint8_t rev_mlen[8];
    to_little_endian(mlen2 * 8, &rev_mlen[0]);

    for (int counter = 56; counter < 64; counter++) {
        buff0[counter] = rev_mlen[counter - 56];
        buff1[counter] = rev_mlen[counter - 56];
        buff2[counter] = rev_mlen[counter - 56];
        buff3[counter] = rev_mlen[counter - 56];
    }
    sha_updatex4(state);
}

static void SerializeInt32(uint8_t buf[4], int32_t val) {
    uint32_t uval = val;
    buf[3] = uval;
    buf[2] = uval >> 8;
    buf[1] = uval >> 16;
    buf[0] = uval >> 24;
}

void sha256x4(
    uint8_t *digest0,
    uint8_t *digest1,
    uint8_t *digest2,
    uint8_t *digest3,
    uint8_t *message0,
    uint8_t *message1,
    uint8_t *message2,
    uint8_t *message3,
    uint64_t mlen){
    
    state stat;
    state* state = &stat;

    ms0 = &message0[0];
    ms1 = &message1[0];
    ms2 = &message2[0];
    ms3 = &message3[0];

    for (int i = 0; i < 8; i++) {
        stat.registers[i] = vdupq_n_u32(iv[i]);
        digest[i] = vdupq_n_u32(iv[i]);
    }

    sha_digest(state, mlen);

    for (int counter = 0; counter < 8; counter++) {
        SerializeInt32(&digest0[counter*4], state->registers[counter][0]);
        SerializeInt32(&digest1[counter*4], state->registers[counter][1]);
        SerializeInt32(&digest2[counter*4], state->registers[counter][2]);
        SerializeInt32(&digest3[counter*4], state->registers[counter][3]);
    }
}


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
    uint64_t mlen){

    state stat;
    state* state = &stat;
    uint32_t t;

    for (size_t i = 0; i < 8; i++) {
        t = load4be(seed + 4*i);
        stat.registers[i] = vdupq_n_u32(t);
    }

    ms0 = &message0[0];
    ms1 = &message1[0];
    ms2 = &message2[0];
    ms3 = &message3[0];

    sha_digest(state, mlen);

    for (int counter = 0; counter < 8; counter++) {
        SerializeInt32(&digest0[counter*4], state->registers[counter][0]);
        SerializeInt32(&digest1[counter*4], state->registers[counter][1]);
        SerializeInt32(&digest2[counter*4], state->registers[counter][2]);
        SerializeInt32(&digest3[counter*4], state->registers[counter][3]);
    }
}

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
            unsigned long inlen)
{
    unsigned char inbuf0[inlen + 4];
    unsigned char inbuf1[inlen + 4];
    unsigned char inbuf2[inlen + 4];
    unsigned char inbuf3[inlen + 4];
    unsigned char outbuf0[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf1[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf2[SPX_SHA256_OUTPUT_BYTES];
    unsigned char outbuf3[SPX_SHA256_OUTPUT_BYTES];
    unsigned long i;


    memcpy(inbuf0, in0, inlen);
    memcpy(inbuf1, in1, inlen);
    memcpy(inbuf2, in2, inlen);
    memcpy(inbuf3, in3, inlen);


    /* While we can fit in at least another full block of SHA256 output.. */
    for (i = 0; (i+1)*SPX_SHA256_OUTPUT_BYTES <= outlen; i++) {
        u32_to_bytes(inbuf0 + inlen, i);
        u32_to_bytes(inbuf1 + inlen, i);
        u32_to_bytes(inbuf2 + inlen, i);
        u32_to_bytes(inbuf3 + inlen, i);

        sha256x4(out0, out1, out2, out3, inbuf0, inbuf1, inbuf2, inbuf3, inlen + 4);
        out0 += SPX_SHA256_OUTPUT_BYTES;
        out1 += SPX_SHA256_OUTPUT_BYTES;
        out2 += SPX_SHA256_OUTPUT_BYTES;
        out3 += SPX_SHA256_OUTPUT_BYTES;
    }

    /* Until we cannot anymore, and we fill the remainder. */
        u32_to_bytes(inbuf0 + inlen, i);
        u32_to_bytes(inbuf1 + inlen, i);
        u32_to_bytes(inbuf2 + inlen, i);
        u32_to_bytes(inbuf3 + inlen, i);

    sha256x4(
            outbuf0,
            outbuf1,
            outbuf2,
            outbuf3,
            inbuf0,
            inbuf1,
            inbuf2,
            inbuf3,
            inlen + 4);

    memcpy(out0, outbuf0, outlen - i*SPX_SHA256_OUTPUT_BYTES);
    memcpy(out1, outbuf1, outlen - i*SPX_SHA256_OUTPUT_BYTES);
    memcpy(out2, outbuf2, outlen - i*SPX_SHA256_OUTPUT_BYTES);
    memcpy(out3, outbuf3, outlen - i*SPX_SHA256_OUTPUT_BYTES);
}
//}