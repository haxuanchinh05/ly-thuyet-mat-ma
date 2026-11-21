#include "des.h"

/* IP: Ma tran 8x8 (danh chi so b0..b63 theo slide) */
static const int IP_0[64] = {
    /* 4 hang dau -> L0 (cac bit chi so le) */
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7,
    /* 4 hang sau -> R0 (cac bit chi so chan) */
    56, 48, 40, 32, 24, 16,  8, 0,
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6
};

/* FP (IP^-1): Ma tran 8x8 */
static const int FP_0[64] = {
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25,
    32, 0, 40,  8, 48, 16, 56, 24
};

/* E: Ma tran mo rong 8x6 (32 bit -> 48 bit) */
static const int E_0[48] = {
    31,  0,  1,  2,  3,  4,
     3,  4,  5,  6,  7,  8,
     7,  8,  9, 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31,  0
};

/* P: Ma tran hoan vi 4x8 (32 bit -> 32 bit) */
static const int P_0[32] = {
    15,  6, 19, 20, 28, 11, 27, 16,
     0, 14, 22, 25,  4, 17, 30,  9,
     1,  7, 23, 13, 31, 26,  2,  8,
    18, 12, 29,  5, 21, 10,  3, 24
};

/* PC-1: Ma tran 8x7 (64 bit -> 56 bit, loai bo 8 bit parity) */
static const int PC1_0[56] = {
    /* C0 (28 bit) */
    56, 48, 40, 32, 24, 16,  8,
     0, 57, 49, 41, 33, 25, 17,
     9,  1, 58, 50, 42, 34, 26,
    18, 10,  2, 59, 51, 43, 35,
    /* D0 (28 bit) */
    62, 54, 46, 38, 30, 22, 14,
     6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28,
    20, 12,  4, 27, 19, 11,  3
};

/* PC-2: Ma tran 8x6 (56 bit -> 48 bit khoa vong) */
static const int PC2_0[48] = {
    13, 16, 10, 23,  0,  4,
     2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7,
    15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54,
    29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,
    45, 41, 49, 35, 28, 31
};

static const int SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

static const uint8_t SBOX[8][4][16] = {
    {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7}, {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
     {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0}, {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
    {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10}, {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
     {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15}, {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
    {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8}, {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
     {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7}, {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
    {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15}, {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
     {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4}, {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
    {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9}, {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
     {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14}, {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
    {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11}, {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
     {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6}, {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
    {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1}, {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
     {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2}, {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
    {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7}, {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
     {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8}, {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};

static uint64_t permute(uint64_t in, const int *table, int in_bits, int out_bits) {
    uint64_t out = 0;
    for (int i = 0; i < out_bits; i++) {
        uint64_t bit = (in >> (in_bits - 1 - table[i])) & 1ULL;
        out = (out << 1) | bit;
    }
    return out;
}

static uint32_t rotl28(uint32_t val, int shift) {
    return ((val << shift) | (val >> (28 - shift))) & 0x0FFFFFFFU;
}

static void print_bits(FILE *out, uint64_t val, int nbits, int group) {
    for (int i = 0; i < nbits; i++) {
        fprintf(out, "%d", (int)((val >> (nbits - 1 - i)) & 1ULL));
        if (group > 0 && (i + 1) % group == 0 && i != nbits - 1) {
            fprintf(out, " ");
        }
    }
}

void des_key_schedule(uint64_t key, uint64_t subkeys[16], FILE *out) {
    uint64_t pc1 = permute(key, PC1_0, 64, 56);
    uint32_t C = (uint32_t)(pc1 >> 28) & 0x0FFFFFFFU;
    uint32_t D = (uint32_t)pc1 & 0x0FFFFFFFU;

    fprintf(out, "[PC-1] Loai bo 8 bit parity (b7, b15, b23, b31, b39, b47, b55, b63)\n");
    fprintf(out, "  C0 = %07X (", C); print_bits(out, C, 28, 7); fprintf(out, ")\n");
    fprintf(out, "  D0 = %07X (", D); print_bits(out, D, 28, 7); fprintf(out, ")\n\n");

    for (int r = 0; r < 16; r++) {
        C = rotl28(C, SHIFTS[r]);
        D = rotl28(D, SHIFTS[r]);
        uint64_t cd = ((uint64_t)C << 28) | (uint64_t)D;
        subkeys[r] = permute(cd, PC2_0, 56, 48);
        fprintf(out, "  Vong %2d (dich %d): C%-2d=%07X | D%-2d=%07X | K%-2d=%012llX\n",
                r + 1, SHIFTS[r], r + 1, C, r + 1, D, r + 1, (unsigned long long)subkeys[r]);
    }
}

static uint32_t feistel_f(uint32_t R, uint64_t K, FILE *out) {
    uint64_t er = permute(R, E_0, 32, 48);
    uint64_t xored = er ^ K;
    uint32_t s_out = 0;
    uint8_t s_nibbles[8];

    for (int i = 0; i < 8; i++) {
        uint8_t chunk = (uint8_t)((xored >> (42 - 6 * i)) & 0x3FU);
        int row = ((chunk & 0x20U) >> 4) | (chunk & 0x01U);
        int col = (chunk >> 1) & 0x0FU;
        s_nibbles[i] = SBOX[i][row][col];
        s_out = (s_out << 4) | s_nibbles[i];
    }

    uint32_t p_out = (uint32_t)permute(s_out, P_0, 32, 32);

    fprintf(out, "  E(R)        = %012llX\n", (unsigned long long)er);
    fprintf(out, "  E(R) XOR K  = %012llX\n", (unsigned long long)xored);
    fprintf(out, "  S1..S8      = [%X %X %X %X %X %X %X %X] -> %08X\n",
            s_nibbles[0], s_nibbles[1], s_nibbles[2], s_nibbles[3],
            s_nibbles[4], s_nibbles[5], s_nibbles[6], s_nibbles[7], s_out);
    fprintf(out, "  P(S) = f    = %08X\n", p_out);
    return p_out;
}

uint64_t des_encrypt_trace(uint64_t plaintext, uint64_t key, FILE *out) {
    uint64_t subkeys[16];

    fprintf(out, "KIEM DINH CHI TIET TRANG THAI DES (QUY UOC DAY BIT b0 b1 ... b63)\n");
    fprintf(out, "Plaintext (Hex): %016llX\n", (unsigned long long)plaintext);
    fprintf(out, "Day b0..b63    : "); print_bits(out, plaintext, 64, 8); fprintf(out, "\n");
    fprintf(out, "Key (Hex)      : %016llX\n", (unsigned long long)key);
    fprintf(out, "Key b0..b63    : "); print_bits(out, key, 64, 8); fprintf(out, "\n\n");

    fprintf(out, "--- GIAI DOAN 1: SINH KHOA CON (KEY SCHEDULE) ---\n");
    des_key_schedule(key, subkeys, out);

    uint64_t ip = permute(plaintext, IP_0, 64, 64);
    uint32_t L = (uint32_t)(ip >> 32);
    uint32_t R = (uint32_t)(ip & 0xFFFFFFFFULL);

    fprintf(out, "\n--- GIAI DOAN 2: HOAN VI KHOI DAU (IP) ---\n");
    fprintf(out, "IP(P) = %016llX -> L0 = %08X, R0 = %08X\n", (unsigned long long)ip, L, R);

    fprintf(out, "\n--- GIAI DOAN 3: 16 VONG LAP FEISTEL ---\n");
    for (int r = 0; r < 16; r++) {
        uint32_t prev_L = L;
        uint32_t prev_R = R;
        fprintf(out, "\n[Vong %2d] K%d = %012llX\n", r + 1, r + 1, (unsigned long long)subkeys[r]);
        uint32_t f_val = feistel_f(prev_R, subkeys[r], out);
        L = prev_R;
        R = prev_L ^ f_val;
        fprintf(out, "  Trang thai  : L%-2d = %08X | R%-2d = %08X\n", r + 1, L, r + 1, R);
    }

    uint64_t pre_fp = ((uint64_t)R << 32) | (uint64_t)L;
    uint64_t ciphertext = permute(pre_fp, FP_0, 64, 64);

    fprintf(out, "\n--- GIAI DOAN 4: HOAN VI KET THUC (IP^-1) ---\n");
    fprintf(out, "R16 || L16      : %016llX\n", (unsigned long long)pre_fp);
    fprintf(out, "Ciphertext (Hex): %016llX\n", (unsigned long long)ciphertext);
    fprintf(out, "Cipher b0..b63  : "); print_bits(out, ciphertext, 64, 8); fprintf(out, "\n");
    return ciphertext;
}