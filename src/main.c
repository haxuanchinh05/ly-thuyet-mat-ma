#include "des.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

int main(void) {
    MKDIR("results");

    /* 1. Kiem dinh DES (mau Slide b0..b63 / Stallings) */
    uint64_t des_pt       = 0x02468ACEECA86420ULL;
    uint64_t des_key      = 0x0F1571C947D9E859ULL;
    uint64_t des_expected = 0xDA02CE3A89ECAC3BULL;

    FILE *f_des = fopen("results/des_trace.txt", "w");
    uint64_t des_ct = des_encrypt_trace(des_pt, des_key, f_des ? f_des : stdout);
    if (f_des) {
        fprintf(f_des, "\n[KIEM DINH DES] Expected: %016llX | Actual: %016llX -> %s\n",
                (unsigned long long)des_expected,
                (unsigned long long)des_ct,
                (des_ct == des_expected) ? "PASS" : "FAIL");
        fclose(f_des);
    }

    printf("=== KET QUA KIEM DINH DES ===\n");
    printf("1. DES (P=02468ACEECA86420, K=0F1571C947D9E859)\n");
    printf("   Ciphertext: %016llX [%s]\n",
           (unsigned long long)des_ct, (des_ct == des_expected) ? "PASS" : "FAIL");
    printf("   Chi tiet  : results/des_trace.txt\n");

    return (des_ct == des_expected) ? 0 : 1;
}