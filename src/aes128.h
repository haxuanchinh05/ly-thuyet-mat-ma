#ifndef AES128_H
#define AES128_H

#include <stdint.h>
#include <stdio.h>

void aes128_key_expansion(const uint8_t key[16], uint8_t round_keys[11][16], FILE *out);
void aes128_encrypt_trace(const uint8_t plaintext[16], const uint8_t key[16],
                          uint8_t ciphertext[16], FILE *out);

#endif