#ifndef DES_H
#define DES_H

#include <stdint.h>
#include <stdio.h>

void des_key_schedule(uint64_t key, uint64_t subkeys[16], FILE *out);
uint64_t des_encrypt_trace(uint64_t plaintext, uint64_t key, FILE *out);

#endif