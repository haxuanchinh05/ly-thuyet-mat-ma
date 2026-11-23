#include "aes128.h"
#include <string.h>

static const uint8_t SBOX[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t RCON[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static uint8_t xtime(uint8_t x) {
    return (uint8_t)((x << 1) ^ ((x & 0x80U) ? 0x1BU : 0x00U));
}

/* In trang thai dang chuoi Hex 16 byte va ma tran State 4x4 theo cot */
static void print_state(FILE *out, const char *label, const uint8_t s[16]) {
    fprintf(out, "  %-18s: ", label);
    for (int i = 0; i < 16; i++) {
        fprintf(out, "%02X", s[i]);
    }
    fprintf(out, "\n");
    for (int row = 0; row < 4; row++) {
        fprintf(out, "      [ %02X %02X %02X %02X ]\n",
                s[row], s[4 + row], s[8 + row], s[12 + row]);
    }
}

void aes128_key_expansion(const uint8_t key[16], uint8_t round_keys[11][16], FILE *out) {
    uint8_t w[44][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            w[i][j] = key[4 * i + j];
        }
    }

    for (int i = 4; i < 44; i++) {
        uint8_t temp[4];
        memcpy(temp, w[i - 1], 4);
        if (i % 4 == 0) {
            uint8_t t = temp[0];
            temp[0] = SBOX[temp[1]] ^ RCON[i / 4 - 1];
            temp[1] = SBOX[temp[2]];
            temp[2] = SBOX[temp[3]];
            temp[3] = SBOX[t];
        }
        for (int j = 0; j < 4; j++) {
            w[i][j] = w[i - 4][j] ^ temp[j];
        }
    }

    for (int r = 0; r < 11; r++) {
        fprintf(out, "  RoundKey[%2d] (w%-2d..w%-2d): ", r, 4 * r, 4 * r + 3);
        for (int c = 0; c < 4; c++) {
            for (int row = 0; row < 4; row++) {
                round_keys[r][4 * c + row] = w[4 * r + c][row];
                fprintf(out, "%02X", w[4 * r + c][row]);
            }
            if (c < 3) fprintf(out, " ");
        }
        fprintf(out, "\n");
    }
}

static void sub_bytes(uint8_t state[16]) {
    for (int i = 0; i < 16; i++) state[i] = SBOX[state[i]];
}

static void shift_rows(uint8_t s[16]) {
    uint8_t t;
    t = s[1]; s[1] = s[5]; s[5] = s[9]; s[9] = s[13]; s[13] = t;
    t = s[2]; s[2] = s[10]; s[10] = t;
    t = s[6]; s[6] = s[14]; s[14] = t;
    t = s[15]; s[15] = s[11]; s[11] = s[7]; s[7] = s[3]; s[3] = t;
}

static void mix_columns(uint8_t s[16]) {
    for (int c = 0; c < 4; c++) {
        uint8_t *a = &s[4 * c];
        uint8_t t = a[0] ^ a[1] ^ a[2] ^ a[3];
        uint8_t u = a[0];
        a[0] ^= t ^ xtime(a[0] ^ a[1]);
        a[1] ^= t ^ xtime(a[1] ^ a[2]);
        a[2] ^= t ^ xtime(a[2] ^ a[3]);
        a[3] ^= t ^ xtime(a[3] ^ u);
    }
}

static void add_round_key(uint8_t state[16], const uint8_t rk[16]) {
    for (int i = 0; i < 16; i++) state[i] ^= rk[i];
}

void aes128_encrypt_trace(const uint8_t plaintext[16], const uint8_t key[16],
                          uint8_t ciphertext[16], FILE *out) {
    uint8_t round_keys[11][16];
    uint8_t state[16];
    memcpy(state, plaintext, 16);

    fprintf(out, "KIEM DINH CHI TIET TRANG THAI AES-128 (MA TRAN STATE 4x4)\n");
    print_state(out, "Plaintext (P)", plaintext);
    print_state(out, "Key (K)", key);

    fprintf(out, "\nGIAI DOAN 1: MO RONG KHOA (KEY EXPANSION w0..w43)\n");
    aes128_key_expansion(key, round_keys, out);

    fprintf(out, "\nGIAI DOAN 2: VONG KHOI TAO (ROUND 0)\n");
    add_round_key(state, round_keys[0]);
    print_state(out, "After AddRoundKey0", state);

    fprintf(out, "\nGIAI DOAN 3: 10 VONG BIEN DOI CHINH\n");
    for (int r = 1; r <= 10; r++) {
        fprintf(out, "\n[Vong %2d]\n", r);
        sub_bytes(state);
        print_state(out, "After SubBytes", state);

        shift_rows(state);
        print_state(out, "After ShiftRows", state);

        if (r < 10) {
            mix_columns(state);
            print_state(out, "After MixColumns", state);
        }

        add_round_key(state, round_keys[r]);
        print_state(out, "After AddRoundKey", state);
    }

    memcpy(ciphertext, state, 16);
    fprintf(out, "\nGIAI DOAN 4: KET QUA BAN MA CUOI CUNG\n");
    print_state(out, "Ciphertext (C)", ciphertext);
}