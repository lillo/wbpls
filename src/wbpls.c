#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "wbpls.h"

#include "wbpls_internal.h"

const Configuration Default;

#define HADAMARD_ROW 4

signed char H8[8][8] = {
  {1,   1,   1,   1,   1,   1,   1,   1},
  {1,  -1,   1,  -1,   1,  -1,   1,  -1},
  {1,   1,  -1,  -1,   1,   1,  -1,  -1},
  {1,  -1,  -1,   1,   1,  -1,  -1,   1},
  {1,   1,   1,   1,  -1,  -1,  -1,  -1},
  {1,  -1,   1,  -1,  -1,   1,  -1,   1},
  {1,   1,  -1,  -1,  -1,  -1,   1,   1},
  {1,  -1,  -1,   1,  -1,   1,   1,  -1},
};

short* chars2bits(const char* msg, size_t length){
  short* msg_in_bits = malloc(length * 8 * sizeof(short));   // TODO: check se malloc fail

  for(size_t i = 0; i < length; ++i){
    for(int j = 7; j > -1; --j){
      short mask = (1 << j);
      msg_in_bits[i*8 + 7 - j] = (msg[i] & mask) > 0 ? 1 : -1;
    }
  }  

  return msg_in_bits;
}


void bits2chars(char* msg, size_t msg_length, short* recv_bits, size_t bits_length){
  for(size_t i=0; i < msg_length; ++i){
    for(size_t j=0; j < 8; ++j){
      msg[i] = msg[i] << 1;
      if(recv_bits[i*8 + j] == 1)
        msg[i] |= 1;
    }
  }
}

short* spread_watermark(const short* info_bits, size_t info_bits_length, size_t hadamard_row, size_t row_length) {
  short* watermark = malloc(info_bits_length * row_length * sizeof(short)); // TODO: controllare fail malloc

  for(size_t i = 0; i < info_bits_length; ++i){
    for(size_t j = 0; j < row_length; ++j){
      watermark[i * info_bits_length + j] = H8[hadamard_row][j] * info_bits[i];
    }
  }

  return watermark;
}

void send(const char*msg, size_t length)
{
  assert(length == 8);
  // 1. Convertire il messaggio in binario dove ogni bit è un intero
  short* msg_in_bits = chars2bits(msg, length);
  size_t length_in_bits = length * 8;

  /*
  for(size_t i=0; i < length_in_bits; ++i){
    if(i % 8 == 0) printf("|");
    printf("%2d ", msg_in_bits[i]);
  }
  puts("\n");
  */

  // 2. Estrazione dei primi 8 bits di informazione da usare come chiave
  short info_bits[8] = {0};
  memcpy(info_bits, msg_in_bits, sizeof(short)*8);

  // 3. Calcolo del watermark
  short* watermark = spread_watermark(info_bits, 8, HADAMARD_ROW, 8);

  /*
  for(size_t i=0; i < length_in_bits; ++i){
    if(i % 8 == 0) printf("|");
    printf("%2d ", watermark[i]);
  }
  puts("\n");
  */

  // 4. Embed del watermark all'interno del messaggio
  for(size_t i=0; i < length_in_bits; ++i)
    msg_in_bits[i] += watermark[i];
  
  FILE* f = fopen("transit_data.dat", "w");
  for(size_t i=0; i < length_in_bits; ++i){
    fprintf(f,"%d ", msg_in_bits[i]);
  }
  fclose(f);

  if(msg_in_bits)
    free(msg_in_bits);

  if(watermark)
    free(watermark);
}

short* extract_watermark(short *watermarked_bitstream, int row) {
    static short extracted_watermark_bits[8]; // static array to hold the extracted bits
    int i, j;
    float correlation[8] = {0};

    float norm;
    float max_corr = 0;
    float max_norm = 0;
    for (i = 0; i < 8; i++) {
        norm = 0;
        for (j = 0; j < 8; j++) {
            norm += pow(H8[row][j], 2);
        }
        norm = sqrt(norm);
        if (norm > max_norm) {
            max_norm = norm;
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            correlation[i] += watermarked_bitstream[i * 8 + j] * H8[row][j]/ max_norm;
            if (fabs(correlation[i]) > max_corr) {
                max_corr = fabs(correlation[i]);
            }
        }
    }

    //float th_corr = 0.7 * max_corr;
    float th_corr = 0;
    for (i = 0; i < 8; i++) {
        if (correlation[i] > 0 && correlation[i] >= th_corr) {
            extracted_watermark_bits[i] = 1;
        } else if (correlation[i] < 0 && fabs(correlation[i]) >= th_corr) {
            extracted_watermark_bits[i] = -1;
        }
    }
    return extracted_watermark_bits;
}

void recv(char* buffer, size_t length)
{
  FILE* f = fopen("transit_data.dat", "r");
  size_t recv_buffer_length = length * 8;
  short *recv_buffer = malloc(recv_buffer_length * sizeof(short));
  
  int current_pos = 0;
  int current_c;
  int current_sign = 1;
  
  while((current_c = fgetc(f)) != EOF){
    switch(current_c){
      case '0':
      case '1':
      case '2':
        recv_buffer[current_pos++] = (current_c - '0') * current_sign;
        break;
      case '-':
      current_sign = -1;
      break;
      case ' ':
      current_sign = 1;
      break;
    }
  }

  for(size_t i=0; i < 3; ++i)
    recv_buffer[i] = rand() % 2;

  short* info_bits = extract_watermark(recv_buffer, HADAMARD_ROW);    
  for(size_t i=0; i < 8; ++i){
     if(i % 8 == 0) printf("|");
     printf("%d ", info_bits[i]);
  }
  puts("\n");

  short* watermark = spread_watermark(info_bits, 8, HADAMARD_ROW, 8);

  // 4. Remove il watermark dal messaggio
  for(size_t i=0; i < 8; ++i)
    recv_buffer[i] = info_bits[i];

  for(size_t i=8; i < recv_buffer_length; ++i)
    recv_buffer[i] -= watermark[i];
  
  // for(size_t i=0; i < recv_buffer_length; ++i){
  //   if(i % 8 == 0) printf("|");
  //   printf("%d ", recv_buffer[i]);
  // }

  //fclose(f);
  // 
  memset(buffer,0,length);
  bits2chars(buffer, length, recv_buffer, recv_buffer_length);

}


