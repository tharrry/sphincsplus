#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<string.h>
#include<arm_neon.h>
#include "sha256_neon.h"
#include "sha256.h"
static inline uint64_t
read_pmccntr(void)
{
    uint64_t val;
    asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
    return val;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Please provide 4 messages to digest.\n");
        return -1;
    }
    if (!(strlen(argv[1]) == strlen(argv[2]) && strlen(argv[2]) == strlen(argv[3]) && strlen(argv[3]) == strlen(argv[4]))) {
        printf("Please provide 4 messages of equal length.\n");
        return -1;
    }


    uint8_t digest0[32], digest1[32], digest2[32], digest3[32];
    uint64_t mlen = strlen(argv[1]);

    uint64_t a = read_pmccntr();

    sha256x4(argv[1], argv[2], argv[3], argv[4], digest0, digest1, digest2, digest3, mlen);

    printf("%d\n", read_pmccntr()-a);

    for(int counter = 0; counter < 32; counter++){
        printf("%02x", digest0[counter]);
    }
    printf("\n");

    //void sha256(uint8_t *out, const uint8_t *in, size_t inlen);
    a = read_pmccntr();
    sha256(digest0,argv[1], mlen);
    printf("%d\n", read_pmccntr()-a);
    return 0;
}