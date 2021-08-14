#include<stdio.h>
#include<arm_neon.h>
//#include "sha256_neon.h"


/* Based on the public domain implementation in
 * crypto_hash/sha512/ref/ from http://bench.cr.yp.to/supercop.html
 * by D. J. Bernstein */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arm_neon.h>
#include "sha256_neon11.h"
#include "utils.h"

static uint32x4_t load_bigendian_state_32(const uint8_t *x) {
    uint32x4_t result;
    for(int i = 0; i < 4; i++){
        result[i] = 
          (uint32_t)(x[3+i*32]) | 
        (((uint32_t)(x[2+i*32])) << 8) |
        (((uint32_t)(x[1+i*32])) << 16) |
        (((uint32_t)(x[0+i*32])) << 24);
    }
    return result;
}

static uint64_t load_bigendian_64(const uint8_t *x) {
    return (uint64_t)(x[7]) | (((uint64_t)(x[6])) << 8) |
           (((uint64_t)(x[5])) << 16) | (((uint64_t)(x[4])) << 24) |
           (((uint64_t)(x[3])) << 32) | (((uint64_t)(x[2])) << 40) |
           (((uint64_t)(x[1])) << 48) | (((uint64_t)(x[0])) << 56);
}

static void store_bigendian_32(uint8_t *x, const uint32x4_t c) {
    uint32x4_t u;
    u = vshrq_n_u32(c, 24);
    x[0]    = (uint8_t) u[0];
    x[0+32] = (uint8_t) u[1];
    x[0+64] = (uint8_t) u[2];
    x[0+96] = (uint8_t) u[3];
    u = vshrq_n_u32(c, 16);
    x[1]    = (uint8_t) u[0];
    x[1+32] = (uint8_t) u[1];
    x[1+64] = (uint8_t) u[2];
    x[1+96] = (uint8_t) u[3];
    u = vshrq_n_u32(c, 8);
    x[2]    = (uint8_t) u[0];
    x[2+32] = (uint8_t) u[1];
    x[2+64] = (uint8_t) u[2];
    x[2+96] = (uint8_t) u[3];
    u = c;
    x[3]    = (uint8_t) u[0];
    x[3+32] = (uint8_t) u[1];
    x[3+64] = (uint8_t) u[2];
    x[3+96] = (uint8_t) u[3];
}

static void store_bigendian_64(uint8_t *x, uint64_t u) {
    x[7] = (uint8_t) u;
    u >>= 8;
    x[6] = (uint8_t) u;
    u >>= 8;
    x[5] = (uint8_t) u;
    u >>= 8;
    x[4] = (uint8_t) u;
    u >>= 8;
    x[3] = (uint8_t) u;
    u >>= 8;
    x[2] = (uint8_t) u;
    u >>= 8;
    x[1] = (uint8_t) u;
    u >>= 8;
    x[0] = (uint8_t) u;
}


#define SHR(x, c) (vshrq_n_u32((x), (c)))
//#define ROTR_32(x, c) (veorq_u32(((vshrq_n_u32(x), (c))), (vshlq_n_u32(x), (32-(c)))))
#define ROTR_32(x, c) (veorq_u32(vshlq_n_u32((x), 32 - (c)), vshrq_n_u32((x), (c))))

#define Ch(x, y, z) (veorq_u32(vandq_u32((x), (y)),vandq_u32(veorq_u32(vdupq_n_u32(0xffffffff), (x)),(z))))
#define Maj(x, y, z) (veorq_u32(veorq_u32(vandq_u32((x),(y)),vandq_u32((x),(z))),vandq_u32((y),(z))))

#define Sigma0_32(x) (veorq_u32(ROTR_32(x, 2), veorq_u32(ROTR_32(x,13), ROTR_32(x,22))))
#define Sigma1_32(x) (veorq_u32(ROTR_32(x, 6), veorq_u32(ROTR_32(x,11), ROTR_32(x,25))))
#define sigma0_32(x) (veorq_u32(ROTR_32(x, 7), veorq_u32(ROTR_32(x,18), SHR(x, 3))))
#define sigma1_32(x) (veorq_u32(ROTR_32(x, 17), veorq_u32(ROTR_32(x,19), SHR(x, 10))))

#define M_32(w0, w14, w9, w1) w0 = vaddq_u32(vaddq_u32((sigma1_32(w14)), (w9)), vaddq_u32((sigma0_32(w1)), (w0)));

#define EXPAND_32           \
    M_32(w0, w14, w9, w1)   \
    M_32(w1, w15, w10, w2)  \
    M_32(w2, w0, w11, w3)   \
    M_32(w3, w1, w12, w4)   \
    M_32(w4, w2, w13, w5)   \
    M_32(w5, w3, w14, w6)   \
    M_32(w6, w4, w15, w7)   \
    M_32(w7, w5, w0, w8)    \
    M_32(w8, w6, w1, w9)    \
    M_32(w9, w7, w2, w10)   \
    M_32(w10, w8, w3, w11)  \
    M_32(w11, w9, w4, w12)  \
    M_32(w12, w10, w5, w13) \
    M_32(w13, w11, w6, w14) \
    M_32(w14, w12, w7, w15) \
    M_32(w15, w13, w8, w0)

#define F_32(w, k)                                   \
    T1 = vaddq_u32(h, vaddq_u32(Sigma1_32(e), vaddq_u32(Ch(e, f, g), vaddq_u32((k), (w))))); \
    T2 = vaddq_u32(Sigma0_32(a), Maj(a, b, c));                \
    h = g;                                           \
    g = f;                                           \
    f = e;                                           \
    e = vaddq_u32(d, T1);                                      \
    d = c;                                           \
    c = b;                                           \
    b = a;                                           \
    a = vaddq_u32(T1, T2);


static size_t crypto_hashblocks_sha256(uint8_t *statebytes,
                                       const uint8_t *in0,
                                       const uint8_t *in1,
                                       const uint8_t *in2,
                                       const uint8_t *in3,
                                       size_t inlen,
                                       int padded) {

    size_t offset = 0;
    uint32x4_t state[8];
    uint32x4_t a;
    uint32x4_t b;
    uint32x4_t c;
    uint32x4_t d;
    uint32x4_t e;
    uint32x4_t f;
    uint32x4_t g;
    uint32x4_t h;
    uint32x4_t T1;
    uint32x4_t T2;

    (void)padded;

    uint32x4_t w0;
    uint32x4_t w1;
    uint32x4_t w2;
    uint32x4_t w3;
    uint32x4_t w4;
    uint32x4_t w5;
    uint32x4_t w6;
    uint32x4_t w7;
    uint32x4_t w8;
    uint32x4_t w9;
    uint32x4_t w10;
    uint32x4_t w11;
    uint32x4_t w12;
    uint32x4_t w13;
    uint32x4_t w14;
    uint32x4_t w15;

    uint32x4_t i,  j,  k,  l, t;
    uint64x2_t i2, j2, k2, l2;

    uint32_t *in32_0 = (uint32_t *)in0;
    uint32_t *in32_1 = (uint32_t *)in1;
    uint32_t *in32_2 = (uint32_t *)in2;
    uint32_t *in32_3 = (uint32_t *)in3;

    a = load_bigendian_state_32(statebytes + 0);
    state[0] = a;
    b = load_bigendian_state_32(statebytes + 4);
    state[1] = b;
    c = load_bigendian_state_32(statebytes + 8);
    state[2] = c;
    d = load_bigendian_state_32(statebytes + 12);
    state[3] = d;
    e = load_bigendian_state_32(statebytes + 16);
    state[4] = e;
    f = load_bigendian_state_32(statebytes + 20);
    state[5] = f;
    g = load_bigendian_state_32(statebytes + 24);
    state[6] = g;
    h = load_bigendian_state_32(statebytes + 28);
    state[7] = h;

    while (inlen - offset*4 >= 64) {

        i = vld1q_u32(in32_0 +  0 + offset);
        j = vld1q_u32(in32_1 +  0 + offset);
        k = vld1q_u32(in32_2 +  0 + offset);
        l = vld1q_u32(in32_3 +  0 + offset);

        t = vtrn1q_u32(i, j);
        j = vtrn2q_u32(i, j);
        i = t;

        t = vtrn1q_u32(k, l);
        l = vtrn2q_u32(k, l);
        k = t;

        i2 = vreinterpretq_u64_u32(i);
        j2 = vreinterpretq_u64_u32(j);
        k2 = vreinterpretq_u64_u32(k);
        l2 = vreinterpretq_u64_u32(l);

        w0 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(i2, k2)));
        w1 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(j2, l2)));
        w2 = vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(i2, k2)));
        w3 = vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(j2, l2)));


        i = vld1q_u32(in32_0 +  4 + offset);
        j = vld1q_u32(in32_1 +  4 + offset);
        k = vld1q_u32(in32_2 +  4 + offset);
        l = vld1q_u32(in32_3 +  4 + offset);

        t = vtrn1q_u32(i, j);
        j = vtrn2q_u32(i, j);
        i = t;

        t = vtrn1q_u32(k, l);
        l = vtrn2q_u32(k, l);
        k = t;

        i2 = vreinterpretq_u64_u32(i);
        j2 = vreinterpretq_u64_u32(j);
        k2 = vreinterpretq_u64_u32(k);
        l2 = vreinterpretq_u64_u32(l);

        w4 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(i2, k2)));
        w5 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(j2, l2)));
        w6 = vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(i2, k2)));
        w7 = vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(j2, l2)));


        i = vld1q_u32(in32_0 +  8 + offset);
        j = vld1q_u32(in32_1 +  8 + offset);
        k = vld1q_u32(in32_2 +  8 + offset);
        l = vld1q_u32(in32_3 +  8 + offset);

        t = vtrn1q_u32(i, j);
        j = vtrn2q_u32(i, j);
        i = t;

        t = vtrn1q_u32(k, l);
        l = vtrn2q_u32(k, l);
        k = t;

        i2 = vreinterpretq_u64_u32(i);
        j2 = vreinterpretq_u64_u32(j);
        k2 = vreinterpretq_u64_u32(k);
        l2 = vreinterpretq_u64_u32(l);

        w8 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(i2, k2)));
        w9 = vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(j2, l2)));
        w10= vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(i2, k2)));
        w11= vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(j2, l2)));


        i = vld1q_u32(in32_0 + 12 + offset);
        j = vld1q_u32(in32_1 + 12 + offset);
        k = vld1q_u32(in32_2 + 12 + offset);
        l = vld1q_u32(in32_3 + 12 + offset);

        t = vtrn1q_u32(i, j);
        j = vtrn2q_u32(i, j);
        i = t;

        t = vtrn1q_u32(k, l);
        l = vtrn2q_u32(k, l);
        k = t;

        i2 = vreinterpretq_u64_u32(i);
        j2 = vreinterpretq_u64_u32(j);
        k2 = vreinterpretq_u64_u32(k);
        l2 = vreinterpretq_u64_u32(l);

        w12= vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(i2, k2)));
        w13= vrev32q_u8(vreinterpretq_u32_u64(vtrn1q_u64(j2, l2)));
        w14= vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(i2, k2)));
        w15= vrev32q_u8(vreinterpretq_u32_u64(vtrn2q_u64(j2, l2)));

        F_32(w0, vdupq_n_u32(0x428a2f98))
        F_32(w1, vdupq_n_u32(0x71374491))
        F_32(w2, vdupq_n_u32(0xb5c0fbcf))
        F_32(w3, vdupq_n_u32(0xe9b5dba5))
        F_32(w4, vdupq_n_u32(0x3956c25b))
        F_32(w5, vdupq_n_u32(0x59f111f1))
        F_32(w6, vdupq_n_u32(0x923f82a4))
        F_32(w7, vdupq_n_u32(0xab1c5ed5))
        F_32(w8, vdupq_n_u32(0xd807aa98))
        F_32(w9, vdupq_n_u32(0x12835b01))
        F_32(w10, vdupq_n_u32(0x243185be))
        F_32(w11, vdupq_n_u32(0x550c7dc3))
        F_32(w12, vdupq_n_u32(0x72be5d74))
        F_32(w13, vdupq_n_u32(0x80deb1fe))
        F_32(w14, vdupq_n_u32(0x9bdc06a7))
        F_32(w15, vdupq_n_u32(0xc19bf174))

        EXPAND_32

        F_32(w0,  vdupq_n_u32(0xe49b69c1))
        F_32(w1,  vdupq_n_u32(0xefbe4786))
        F_32(w2,  vdupq_n_u32(0x0fc19dc6))
        F_32(w3,  vdupq_n_u32(0x240ca1cc))
        F_32(w4,  vdupq_n_u32(0x2de92c6f))
        F_32(w5,  vdupq_n_u32(0x4a7484aa))
        F_32(w6,  vdupq_n_u32(0x5cb0a9dc))
        F_32(w7,  vdupq_n_u32(0x76f988da))
        F_32(w8,  vdupq_n_u32(0x983e5152))
        F_32(w9,  vdupq_n_u32(0xa831c66d))
        F_32(w10, vdupq_n_u32(0xb00327c8))
        F_32(w11, vdupq_n_u32(0xbf597fc7))
        F_32(w12, vdupq_n_u32(0xc6e00bf3))
        F_32(w13, vdupq_n_u32(0xd5a79147))
        F_32(w14, vdupq_n_u32(0x06ca6351))
        F_32(w15, vdupq_n_u32(0x14292967))

        EXPAND_32

        F_32(w0,  vdupq_n_u32(0x27b70a85))
        F_32(w1,  vdupq_n_u32(0x2e1b2138))
        F_32(w2,  vdupq_n_u32(0x4d2c6dfc))
        F_32(w3,  vdupq_n_u32(0x53380d13))
        F_32(w4,  vdupq_n_u32(0x650a7354))
        F_32(w5,  vdupq_n_u32(0x766a0abb))
        F_32(w6,  vdupq_n_u32(0x81c2c92e))
        F_32(w7,  vdupq_n_u32(0x92722c85))
        F_32(w8,  vdupq_n_u32(0xa2bfe8a1))
        F_32(w9,  vdupq_n_u32(0xa81a664b))
        F_32(w10, vdupq_n_u32(0xc24b8b70))
        F_32(w11, vdupq_n_u32(0xc76c51a3))
        F_32(w12, vdupq_n_u32(0xd192e819))
        F_32(w13, vdupq_n_u32(0xd6990624))
        F_32(w14, vdupq_n_u32(0xf40e3585))
        F_32(w15, vdupq_n_u32(0x106aa070))

        EXPAND_32

        F_32(w0,  vdupq_n_u32(0x19a4c116))
        F_32(w1,  vdupq_n_u32(0x1e376c08))
        F_32(w2,  vdupq_n_u32(0x2748774c))
        F_32(w3,  vdupq_n_u32(0x34b0bcb5))
        F_32(w4,  vdupq_n_u32(0x391c0cb3))
        F_32(w5,  vdupq_n_u32(0x4ed8aa4a))
        F_32(w6,  vdupq_n_u32(0x5b9cca4f))
        F_32(w7,  vdupq_n_u32(0x682e6ff3))
        F_32(w8,  vdupq_n_u32(0x748f82ee))
        F_32(w9,  vdupq_n_u32(0x78a5636f))
        F_32(w10, vdupq_n_u32(0x84c87814))
        F_32(w11, vdupq_n_u32(0x8cc70208))
        F_32(w12, vdupq_n_u32(0x90befffa))
        F_32(w13, vdupq_n_u32(0xa4506ceb))
        F_32(w14, vdupq_n_u32(0xbef9a3f7))
        F_32(w15, vdupq_n_u32(0xc67178f2))

        a += state[0];
        b += state[1];
        c += state[2];
        d += state[3];
        e += state[4];
        f += state[5];
        g += state[6];
        h += state[7];

        state[0] = a;
        state[1] = b;
        state[2] = c;
        state[3] = d;
        state[4] = e;
        state[5] = f;
        state[6] = g;
        state[7] = h;

        //in += 64;
        offset += 16;
    }

    store_bigendian_32(statebytes + 0, state[0]);
    store_bigendian_32(statebytes + 4, state[1]);
    store_bigendian_32(statebytes + 8, state[2]);
    store_bigendian_32(statebytes + 12, state[3]);
    store_bigendian_32(statebytes + 16, state[4]);
    store_bigendian_32(statebytes + 20, state[5]);
    store_bigendian_32(statebytes + 24, state[6]);
    store_bigendian_32(statebytes + 28, state[7]);

    return inlen - offset;
}


static const uint8_t iv_256[32] = {
    0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85,
    0x3c, 0x6e, 0xf3, 0x72, 0xa5, 0x4f, 0xf5, 0x3a,
    0x51, 0x0e, 0x52, 0x7f, 0x9b, 0x05, 0x68, 0x8c,
    0x1f, 0x83, 0xd9, 0xab, 0x5b, 0xe0, 0xcd, 0x19
};

void sha256x4_inc_init(sha256ctx2 *state) {
    for (size_t counter = 0; counter < 4; counter = counter + 1) {
        for (size_t i = 0; i < 32; ++i) {
            state->ctx[i + 32*counter] = iv_256[i];
        }
    }
    for (size_t i = 0; i < 8; ++i) {
        state->ctx[32*4 + i] = 0;
    }
    //state = iv|iv|iv|iv|length
}

void sha256x4_inc_blocks(sha256ctx2 *state, const uint8_t *in, size_t inblocks) {
    uint64_t bytes = load_bigendian_64(state->ctx + 32);

    crypto_hashblocks_sha256(state->ctx, in, in, in, in, 64 * inblocks, 0);
    bytes += 64 * inblocks;

    store_bigendian_64(state->ctx + 32, bytes);
}

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
                            size_t inlen) {
    uint8_t padded0[128];
    uint8_t padded1[128];
    uint8_t padded2[128];
    uint8_t padded3[128];
    uint64_t bytes = load_bigendian_64(state+ 4*32) + inlen; //bytes = length = 26
    //size_t inlen_copy = inlen;

    crypto_hashblocks_sha256(state, in0, in1, in2, in3, inlen, 0);


    in0 += inlen;
    in1 += inlen;
    in2 += inlen;
    in3 += inlen;

    inlen &= 63;

    in0 -= inlen;
    in1 -= inlen;
    in2 -= inlen;
    in3 -= inlen;

    for (size_t i = 0; i < inlen; ++i) {
        padded0[i]       = in0[i];
        padded1[i]       = in1[i];
        padded2[i]       = in2[i];
        padded3[i]       = in3[i];
    }

    padded0[inlen]       = 0x80;
    padded1[inlen]       = 0x80;
    padded2[inlen]       = 0x80;
    padded3[inlen]       = 0x80;

    uint8_t placeholder;

    if (inlen < 56) {
        for (size_t i = inlen + 1; i < 56; ++i) {
            padded0[i]       = 00;
            padded1[i]       = 00;
            padded2[i]       = 00;
            padded3[i]       = 00;
        }
        placeholder = (uint8_t) (bytes >> 53);
        padded0[56]       = placeholder;
        padded1[56]       = placeholder;
        padded2[56]       = placeholder;
        padded3[56]       = placeholder;
        placeholder = (uint8_t) (bytes >> 45);
        padded0[57]       = placeholder;
        padded1[57]       = placeholder;
        padded2[57]       = placeholder;
        padded3[57]       = placeholder;
        placeholder = (uint8_t) (bytes >> 37);
        padded0[58]       = placeholder;
        padded1[58]       = placeholder;
        padded2[58]       = placeholder;
        padded3[58]       = placeholder;
        placeholder = (uint8_t) (bytes >> 29);
        padded0[59]       = placeholder;
        padded1[59]       = placeholder;
        padded2[59]       = placeholder;
        padded3[59]       = placeholder;
        placeholder = (uint8_t) (bytes >> 21);
        padded0[60]       = placeholder;
        padded1[60]       = placeholder;
        padded2[60]       = placeholder;
        padded3[60]       = placeholder;
        placeholder = (uint8_t) (bytes >> 13);
        padded0[61]       = placeholder;
        padded1[61]       = placeholder;
        padded2[61]       = placeholder;
        padded3[61]       = placeholder;
        placeholder = (uint8_t) (bytes >> 5);
        padded0[62]       = placeholder;
        padded1[62]       = placeholder;
        padded2[62]       = placeholder;
        padded3[62]       = placeholder;
        placeholder = (uint8_t) (bytes << 3);
        padded0[63]       = placeholder;
        padded1[63]       = placeholder;
        padded2[63]       = placeholder;
        padded3[63]       = placeholder;
        crypto_hashblocks_sha256(state, padded0, padded1, padded2, padded3, 64, 1);
    } else {
        for (size_t i = inlen + 1; i < 120; ++i) {
            padded0[i]       = 0x0;
            padded1[i]       = 0x0;
            padded2[i]       = 0x0;
            padded3[i]       = 0x0;
        }
        placeholder = (uint8_t) (bytes >> 53);
        padded0[120]       = placeholder;
        padded1[120]       = placeholder;
        padded2[120]       = placeholder;
        padded3[120]       = placeholder;
        placeholder = (uint8_t) (bytes >> 45);
        padded0[121]       = placeholder;
        padded1[121]       = placeholder;
        padded2[121]       = placeholder;
        padded3[121]       = placeholder;
        placeholder = (uint8_t) (bytes >> 37);
        padded0[122]       = placeholder;
        padded1[122]       = placeholder;
        padded2[122]       = placeholder;
        padded3[122]       = placeholder;
        placeholder = (uint8_t) (bytes >> 29);
        padded0[123]       = placeholder;
        padded1[123]       = placeholder;
        padded2[123]       = placeholder;
        padded3[123]       = placeholder;
        placeholder = (uint8_t) (bytes >> 21);
        padded0[124]       = placeholder;
        padded1[124]       = placeholder;
        padded2[124]       = placeholder;
        padded3[124]       = placeholder;
        placeholder = (uint8_t) (bytes >> 13);
        padded0[125]       = placeholder;
        padded1[125]       = placeholder;
        padded2[125]       = placeholder;
        padded3[125]       = placeholder;
        placeholder = (uint8_t) (bytes >> 5);
        padded0[126]       = placeholder;
        padded1[126]       = placeholder;
        padded2[126]       = placeholder;
        padded3[126]       = placeholder;
        placeholder = (uint8_t) (bytes << 3);
        padded0[127]       = placeholder;
        padded1[127]       = placeholder;
        padded2[127]       = placeholder;
        padded3[127]       = placeholder;
        crypto_hashblocks_sha256(state, padded0, padded1, padded2, padded3, 128, 0);
    }

    for (size_t i = 0; i < 32; ++i) {
        out0[i] = state[i];
        out1[i] = state[i + 32];
        out2[i] = state[i + 64];
        out3[i] = state[i + 96];
    }
}

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

        sha256x42(out0, out1, out2, out3, inbuf0, inbuf1, inbuf2, inbuf3, inlen + 4);
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

    sha256x42(
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

void sha256x42(
                uint8_t *out0,
                uint8_t *out1,
                uint8_t *out2,
                uint8_t *out3,
                const uint8_t *in0,
                const uint8_t *in1,
                const uint8_t *in2,
                const uint8_t *in3,
                size_t inlen) {
    sha256ctx2 state;

    sha256x4_inc_init(&state);
    sha256x4_inc_finalize(out0, out1, out2, out3, state.ctx, in0, in1, in2, in3, inlen);
}