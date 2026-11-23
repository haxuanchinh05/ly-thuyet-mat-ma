#include "des.h"
#include "aes128.h"
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

static void print_hex_inline(FILE *out, const uint8_t *buf, int len) {
    for (int i = 0; i < len; i++) fprintf(out, "%02X", buf[i]);
}

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

    /* 2. Kiem dinh AES-128 (P = 0x00...1111, K = 0x00...0000) */
    uint8_t aes_pt[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11
    };
    uint8_t aes_key[16] = {0};
    uint8_t aes_ct[16]  = {0};
    uint8_t aes_expected[16] = {
        0x44, 0x27, 0x58, 0xDF, 0x6D, 0xDA, 0x9E, 0xDB,
        0x6B, 0xF1, 0x46, 0x8A, 0x17, 0xB9, 0x3F, 0xE5
    };

    FILE *f_aes = fopen("results/aes_trace.txt", "w");
    aes128_encrypt_trace(aes_pt, aes_key, aes_ct, f_aes ? f_aes : stdout);
    int aes_ok = (memcmp(aes_ct, aes_expected, 16) == 0);
    if (f_aes) {
        fprintf(f_aes, "\n[KIEM DINH AES-128] Expected: ");
        print_hex_inline(f_aes, aes_expected, 16);
        fprintf(f_aes, " | Actual: ");
        print_hex_inline(f_aes, aes_ct, 16);
        fprintf(f_aes, " -> %s\n", aes_ok ? "PASS" : "FAIL");
        fclose(f_aes);
    }

    /* 3. In tom tat ket qua kiem dinh ra man hinh console */
    printf("=== TONG HOP KET QUA KIEM DINH DES & AES-128 ===\n");
    printf("1. DES (P=02468ACEECA86420, K=0F1571C947D9E859)\n");
    printf("   Ciphertext: %016llX [%s]\n",
           (unsigned long long)des_ct, (des_ct == des_expected) ? "PASS" : "FAIL");
    printf("   Chi tiet  : results/des_trace.txt\n\n");
    printf("2. AES-128 (P=00000000000000000000000000001111, K=00..00)\n");
    printf("   Ciphertext: ");
    print_hex_inline(stdout, aes_ct, 16);
    printf(" [%s]\n", aes_ok ? "PASS" : "FAIL");
    printf("   Chi tiet  : results/aes_trace.txt\n");

    return (des_ct == des_expected && aes_ok) ? 0 : 1;
}