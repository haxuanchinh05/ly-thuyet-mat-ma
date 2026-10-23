#include <stdio.h>
#include <stdint.h>

// Ham phu tro de in ket qua  duoi dang nhi phan
void in_nhi_phan(uint8_t so, int so_bit) {
    printf("0b");
    for (int i = so_bit - 1; i >= 0; i--) {
        printf("%d", (so >> i) & 1);
    }
}

// Ham nhan tren truong GF(2^4)
uint8_t nhan_gf2_4(uint8_t da_thuc_1, uint8_t da_thuc_2) {
    uint8_t bang_tich[4]; 
    uint8_t gia_tri_tam = da_thuc_2 & 0b1111; 
    
    // buoc 1: lap bang tinh tich
    for (int i = 0; i < 4; i++) {
        bang_tich[i] = gia_tri_tam;
        uint8_t co_tran_bit = gia_tri_tam & 0b1000;
        gia_tri_tam = (gia_tri_tam << 1) & 0b1111; 
        if (co_tran_bit) {
            // day la phan du cua da thuc toi gian x^4 + x + 1 khi luoc bo bac 4
            gia_tri_tam ^= 0b0011;
        }
    }
    
    // buoc 2: cong (XOR) cac hang daa tren da thuc 1
    uint8_t ket_qua = 0;
    for (int i = 0; i < 4; i++) {
        if ((da_thuc_1 >> i) & 1) {
            ket_qua ^= bang_tich[i];
        }
    }
    return ket_qua;
}

// ham nhan tren truong GF(2^8)
uint8_t nhan_gf2_8(uint8_t da_thuc_1, uint8_t da_thuc_2) {
    uint8_t bang_tich[8]; 
    uint8_t gia_tri_tam = da_thuc_2;
    
    // buoc 1: lap bang tinh tich
    for (int i = 0; i < 8; i++) {
        bang_tich[i] = gia_tri_tam;
        uint8_t co_tran_bit = gia_tri_tam & 0b10000000;
        gia_tri_tam = (gia_tri_tam << 1) & 0b11111111;
        if (co_tran_bit) {
            //phan du cua da thuc toi gian  x^8 + x^4 + x^3 + x + 1 la x^4 + x^3 + x + 1
            gia_tri_tam ^= 0b00011011; 
        }
    }
    
    uint8_t ket_qua = 0;
    for (int i = 0; i < 8; i++) {
        if ((da_thuc_1 >> i) & 1) {
            ket_qua ^= bang_tich[i];
        }
    }
    return ket_qua;
}

int main() {
    //x^3 + x + 1
    uint8_t a4 = 0b1011; 
    //x^3 + 1
    uint8_t b4 = 0b1001; 
    printf("GF(2^4): ");
    in_nhi_phan(a4, 4);       
    printf("*");
    in_nhi_phan(b4, 4);       
    printf("= ");
    in_nhi_phan(nhan_gf2_4(a4, b4), 4);
    printf("\n");

    //x^5 + x^2 + x
    uint8_t a8 = 0b00100110; 
    //x^7 + x^4 + x^3 + x^2 + x
    uint8_t b8 = 0b10011110; 
    printf("GF(2^4): ");
    in_nhi_phan(a8, 8);      
    printf("*");
    in_nhi_phan(b8, 8);   
    printf("= ");
    in_nhi_phan(nhan_gf2_8(a8, b8), 8);
    printf("\n");
    
    return 0;
}