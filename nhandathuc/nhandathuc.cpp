#include <stdio.h>
#include <stdint.h>

// Hàm phụ trợ: Lấy độ dài bit (để xác định số hàng cần lập bảng tính)
int get_bit_length(uint8_t val) {
    int length = 0;
    while (val > 0) {
        length++;
        val >>= 1;
    }
    return length;
}

// Hàm phụ trợ: In số nhị phân 8 bit để dễ quan sát
void print_bin(uint8_t val) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
    }
}

/* ==========================================================

HÀM NHÂN TRONG TRƯỜNG GF(2^8)

Đa thức tối giản: x^8 + x^4 + x^3 + x + 1 (0b00011011)

========================================================== */
uint8_t multiply_gf2_8_table_method(uint8_t p1, uint8_t p2) {
    int max_power = get_bit_length(p1);
    uint8_t powers[8] = { 0 }; // Mảng lưu các hàng kết quả x^i * P2
    uint8_t current = p2;

    // BƯỚC 1: Lập bảng tính các giá trị x^i * P2
    for (int i = 0; i < max_power; i++) {
        powers[i] = current;

        // Chuẩn bị cho vòng lặp sau (tính x^(i+1) * P2)
        uint8_t hi_bit = current & 0b10000000; // Kiểm tra bit bậc 7 
        current = (current << 1) & 0b11111111; // Dịch trái 1 bit (nhân với x), giữ 8 bit

        // Nếu tràn bậc 8, rút gọn bằng XOR với đa thức tối giản
        if (hi_bit) {
            current ^= 0b00011011;
        }


    }

    // BƯỚC 2: Cộng (XOR) các hàng tương ứng với bit '1' của P1
    uint8_t result = 0;
    for (int i = 0; i < max_power; i++) {
        // Kiểm tra bit thứ i của P1 có bằng 1 không
        if ((p1 >> i) & 1) {
            result ^= powers[i];
        }
    }

    return result;
}

/* ==========================================================

HÀM NHÂN TRONG TRƯỜNG GF(2^4)

Đa thức tối giản: x^4 + x + 1 (0b0011)

========================================================== */
uint8_t multiply_gf2_4_table_method(uint8_t p1, uint8_t p2) {
    int max_power = get_bit_length(p1);
    uint8_t powers[8] = { 0 };
    uint8_t current = p2;

    for (int i = 0; i < max_power; i++) {
        powers[i] = current;

        uint8_t hi_bit = current & 0b1000; // Kiểm tra bit bậc 3
        current = (current << 1) & 0b1111; // Dịch trái 1 bit, giữ trong 4 bit

        if (hi_bit) {
            current ^= 0b0011;
        }


    }

    uint8_t result = 0;
    for (int i = 0; i < max_power; i++) {
        if ((p1 >> i) & 1) {
            result ^= powers[i];
        }
    }

    return result;
}

int main() {

    // P1 = x^5 + x^2 + x -> 00100110
    // P2 = x^7 + x^4 + x^3 + x^2 + x -> 10011110
    uint8_t p1_val = 0b00100110;
    uint8_t p2_val = 0b10011110;

    printf("--- Kiem tra phep nhan GF(2^8) ---\n");
    printf("P1 = "); print_bin(p1_val); printf("\n");
    printf("P2 = "); print_bin(p2_val); printf("\n");

    uint8_t res_8 = multiply_gf2_8_table_method(p1_val, p2_val);

    // ket qua 00101111 -> x^5 + x^3 + x^2 + x + 1
    printf("Ket qua P1 * P2 = ");
    print_bin(res_8);
    printf("\n\n");


    // a = x^2 + 1 (0b0101), b = x^2 + x + 1 (0b0111)
    // ---------------------------------------------------------
    uint8_t a_val = 0b0101;
    uint8_t b_val = 0b0111;

    printf("--- Kiem tra phep nhan GF(2^4) ---\n");
    printf("A = 00000101\n");
    printf("B = 00000111\n");

    uint8_t res_4 = multiply_gf2_4_table_method(a_val, b_val);

    printf("Ket qua A * B = ");
    print_bin(res_4);
    printf("\n");

    return 0;


}