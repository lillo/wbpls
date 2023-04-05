#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


#define KEY_LEN 16
#define WATERMARK_LEN 8
#define BITSTREAM_LEN 128
#define MESSAGE_LEN 16

#define AWGN_ON 1
#define JAM_ON  0
#define RANDOM_BITS 0

// Private functions
void hadamard(int n, int matrix[][n], int key[]) {
    int i, j, k, s;
    matrix[0][0] = 1;
    for (s = 2, k = 1; s <= n; s <<= 1, k++) {
        for (i = 0; i < s / 2; i++) {
            for (j = 0; j < s / 2; j++) {
                matrix[i + s / 2][j] = matrix[i][j];
                matrix[i][j + s / 2] = matrix[i][j];
                matrix[i + s / 2][j + s / 2] = -matrix[i][j];
            }
        }
    }
}
void spread_watermark(int *watermark_spread, int *watermark_bits, int *key) {
    int i, j;
    for (i = 0; i < WATERMARK_LEN; i++) {
        int watermark_bit = watermark_bits[i];
        for (j = i * KEY_LEN; j < (i + 1) * KEY_LEN; j++) {
            watermark_spread[j] = key[j % KEY_LEN] * watermark_bit;
        }
    }
}

void embed_watermark(int *watermarked_bitstream, int *watermark_spread, int *bitstream) {
    int i;
    for (i = 0; i < BITSTREAM_LEN; i++) {
        watermarked_bitstream[i] = bitstream[i] ^ watermark_spread[i];
    }
}


 int* extract_watermark(int *watermarked_bitstream, int *key) {
    static int extracted_watermark_bits[WATERMARK_LEN]; // static array to hold the extracted bits
    int i, j;
    float correlation[WATERMARK_LEN] = {0};
    int weights[KEY_LEN] = {32768, 16834,8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1}; // weights for each bit position of the key

    float norm;
    float max_corr = 0;
    float max_norm = 0;
    for (i = 0; i < WATERMARK_LEN; i++) {
        norm = 0;
        for (j = 0; j < KEY_LEN; j++) {
            norm += pow(key[j], 2);
        }
        norm = sqrt(norm);
        if (norm > max_norm) {
            max_norm = norm;
        }
    }

    for (i = 0; i < WATERMARK_LEN; i++) {
        for (j = 0; j < KEY_LEN; j++) {
            correlation[i] += watermarked_bitstream[i * KEY_LEN + j] * key[j] * weights[j]/max_norm;
            if (correlation[i] > max_corr) {
                max_corr = correlation[i];
            }
        }
    }

    float th_corr = 0.7 * max_corr;
    for (i = 0; i < WATERMARK_LEN; i++) {
        extracted_watermark_bits[i] = (correlation[i] > th_corr);
    }
    return extracted_watermark_bits;
}

void add_noise(double snr, int *watermarked_bitstream) {
    int i;
    double noise_var = pow(10, -snr/10);
    double noise_std = sqrt(noise_var);
    for (i = 0; i < BITSTREAM_LEN; i++) {
        double noise = noise_std * ((double)rand() / RAND_MAX);
        double wbs_double = (double) watermarked_bitstream[i];
        wbs_double += noise;
        watermarked_bitstream[i] = (int) wbs_double;
    }
}

void jam_signal(int jam_len, int *watermarked_bitstream) {
    int i;
    for (i = 0; i < jam_len; i++) {
        watermarked_bitstream[i] = rand() % 2;
    }
}

void recover_bitstream(int *extracted_watermark_bits, int *key, int *watermarked_bitstream, int *rec_bitstream) {
    int i, j;
    int rec_watermark_spread[BITSTREAM_LEN];

    for (i = 0; i < WATERMARK_LEN; i++) {
        int extracted_bit = extracted_watermark_bits[i];
        for (j = i * KEY_LEN; j < (i + 1) * KEY_LEN; j++) {
            rec_watermark_spread[j] = key[j % KEY_LEN] * extracted_bit;
        }
    }

    printf("Recovered SS Watermark:\n");
    for(int i = 0; i < BITSTREAM_LEN; i++){
        printf("%d ", rec_watermark_spread[i]);
    }
    printf("\n\n");

    for (i = 0; i < BITSTREAM_LEN; i++) {
        rec_bitstream[i] = watermarked_bitstream[i] ^ rec_watermark_spread[i];
    }
}


// Main program
int main() {

    int key[KEY_LEN];
    int bitstream[BITSTREAM_LEN];
    int rec_bitstream[BITSTREAM_LEN];
    int watermark_bits[WATERMARK_LEN];
    int watermark_spread[BITSTREAM_LEN];
    int watermarked_bitstream[BITSTREAM_LEN];
    //char msg[] = "This_is_a_secret";
   // char msg[] = "A5rt9?xZLM1q@wsi";
char msg[] = "This_FF_A_55FF?m";

    int i;
    int hadamard_matrix[KEY_LEN][KEY_LEN];
    float ber_bitstream, ber_watermark;
    int errors_bitstream = 0, errors_watermark = 0;
    int snr = 10;
    int jam_len = 8;

    // Spreading code
    hadamard(KEY_LEN, hadamard_matrix, key);
    for (i = 0; i < KEY_LEN; i++) {
        key[i] = (hadamard_matrix[4][i] + 1) / 2;
    }

    if (RANDOM_BITS) {
        // Generate random bitstream
        srand(time(NULL)); // Initialize random number generator
        for (i = 0; i < BITSTREAM_LEN; i++) {
            bitstream[i] = rand() % 2; // Generate random 0 or 1
        }
    }else {
        // Convert each character in the message to its corresponding binary representation
        int len = strlen(msg);
        int j,k;
        for (i = 0; i < len; i++) {
            // Convert the character to its ASCII value
            int ascii_val = (int)msg[i];

            // Convert the ASCII value to binary and store it in the bitstream
            for (j = 7, k = i * 8; j >= 0; j--, k++) {
                bitstream[k] = (ascii_val >> j) & 1;
            }
        }
    }

    // Bits for the watermark
    for (i = 0; i < WATERMARK_LEN; i++) {
        watermark_bits[i] = bitstream[i];
    }

    // Spread-spectrum watermark
    spread_watermark(watermark_spread, watermark_bits, key);

    //Embedding the watermark
    embed_watermark(watermarked_bitstream, watermark_spread, bitstream);

    if (AWGN_ON == 1) {
        // AWGN Channel
        add_noise(snr, watermarked_bitstream);
    }
    // Jamming Channel
    if (JAM_ON == 1) {
        jam_signal(jam_len, watermarked_bitstream);
    }
    // Extracting the watermark
    int *ext_bits = extract_watermark(watermarked_bitstream, key);

    // Recovering the bitstream
    recover_bitstream(ext_bits, key, watermarked_bitstream, rec_bitstream);

    // --------------------------------------------------------------------------------------
    // Print the original bitstream and the extracted watermark
    printf("Original Bitstream:\n");
    for(int i = 0; i < BITSTREAM_LEN; i++){
        printf("%d ", bitstream[i]);
    }
    printf("\n");

    // Print the bitstream as a character string
    int  v, n, k;
    for (v = 0; v < 16; v++) {
        int char_val = 0;
        for (n = 0, k = v * 8; n < 8; n++, k++) {
            char_val = (char_val << 1) | bitstream[k];
        }
        printf("%c", char_val);
    }
    printf("\n\n");

    printf("SS Watermark:\n");
    for(int i = 0; i < BITSTREAM_LEN; i++){
        printf("%d ", watermark_spread[i]);
    }
    printf("\n\n");

    printf("Watermarked Bitstream:\n");
    for(int i = 0; i < BITSTREAM_LEN; i++){
        printf("%d ", watermarked_bitstream[i]);
    }
    printf("\n\n");

    printf("Recover Bitstream:\n");
    for(int i = 0; i < BITSTREAM_LEN; i++){
        printf("%d ", rec_bitstream[i]);
    }
    printf("\n\n");

    printf("Bits selected for the  Watermark:\n");
    for(int i = 0; i < WATERMARK_LEN; i++){
        printf("%d ", watermark_bits[i]);
    }
    printf("\n");

    printf("Extracted Watermark:\n");
    for(int i = 0; i < WATERMARK_LEN; i++){
        printf("%d ", ext_bits[i]);
    }
    printf("\n\n");

    // --------------------------------------------------------------------------------------
    // Calculate the bit error rates (BERs) for bitstream and watermark
    for (int i = 0; i < BITSTREAM_LEN; i++){
        if (bitstream[i] != rec_bitstream[i]) {
            errors_bitstream++;
        }
    }
    ber_bitstream = (float)errors_bitstream / BITSTREAM_LEN;
   // int* ext_bits = (int *) extracted_watermark_bits;
    for (int i = 0; i < WATERMARK_LEN; i++){
        if (watermark_bits[i] != ext_bits[i]) {
            errors_watermark++;
        }
    }
    ber_watermark = (float)errors_watermark / WATERMARK_LEN;

    printf("Bit error rate for bitstream: %f\n", ber_bitstream);
    printf("Bit error rate for watermark: %f\n", ber_watermark);

    return 0;
}
