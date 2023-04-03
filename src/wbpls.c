#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "wbpls.h"

#include "wbpls_internal.h"

const Configuration Default;

#define HADAMARD_ROW 15

/// @brief Hadamard matrix of size 16x16

const unsigned short H16[16] = {
    32768, 49152, 40960, 53248,
    34816, 50176, 41472, 53504,
    32896, 49216, 40992, 53264,
    34824, 50180, 41474, 53505};


/// @brief Hadamard matrix of size 8x8
const unsigned short H8[8] = {
  255, 170, 204, 153, 
  240, 165, 195, 150
};


void init(const Configuration *conf)
{
  if (conf == NULL)
  {
    conf = &Default;
  }
}

static void formatted_print(unsigned short* buffer) {
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%04hX|", buffer[i]);
  puts("");
}

/// @brief Compute the spreading code for the watermarking
/// @param data a byte representing the watermark
/// @param row the row of Hadamard to use for the spreading
/// @param buffer an output buffer storing the spreading_code
void spreading(unsigned char data, size_t row, unsigned short buffer[BUFFER_SIZE])
{
  memset(buffer, 0, BUFFER_SIZE);

  unsigned char data_test = data;

  for(int i = WATERMARK_INFO_LENGTH - 1; i > -1; --i, data >>= 1){
    if(data & 0x1)
      buffer[i] = H16[row];
  }

  for(int i = WATERMARK_INFO_LENGTH - 1; i > -1; --i, data_test >>= 1){
    assert(((data_test & 0x1) > 0 && buffer[i] == H16[row]) || ((data_test & 0x1) == 0 && buffer[i] == 0));
  }
}

/*
unsigned char reverse_bits(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
*/

unsigned char extract_watermark(unsigned short bitstream[BUFFER_SIZE], size_t hadamard_row){
  unsigned short correlation[BUFFER_SIZE] = {0};
  unsigned char extracted_wat = 0;
  unsigned char max = 0;
  
  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    correlation[i] = __builtin_popcount(bitstream[i] & H16[hadamard_row]);
    max = (max < correlation[i] ? correlation[i] : max);
  }

  printf("Bits of extracted watermark:");
  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    if(correlation[i] >= 0.7 * max)
      extracted_wat |= 0x1 << (WATERMARK_INFO_LENGTH - i - 1);
    //if(i != WATERMARK_INFO_LENGTH - 1)
    //  extracted_wat <<= 1;   
      //if(i < WATERMARK_INFO_LENGTH - 1)  
    printf("%d", (correlation[i] >= 0.7 * max));
  }

  printf("\nCorrelation vector:\n");
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%d|", correlation[i]);
  puts("");

  return extracted_wat;
}

void send(const char*msg, size_t length)
{
  // Alloca buffer
  assert(length == BUFFER_SIZE * 2);

  unsigned int watermarked_buffer[BUFFER_SIZE] = {0};
  unsigned short watermark[BUFFER_SIZE] = {0};
  unsigned short tmp_buffer[BUFFER_SIZE] = {0};
  unsigned short *msg_buffer = (unsigned short*) msg;
  unsigned char first_byte = msg[0];

  // TODO: change when network modem lib available
  FILE* f = fopen("transit_data.dat", "wb");

  printf("Original message in hex:");
  formatted_print(msg_buffer);

  // 1. Watermark primi 8 bit del messaggio 
  // Calcolo del watermark (risultato della moltiplicazione del byte informativo per la matrice di hadamard)
  spreading(first_byte, HADAMARD_ROW, watermark);

  printf("Info bits in the send is %hhX as hex and %c as char\n", first_byte, first_byte);

  printf("Watermark in the send: ");
  formatted_print(watermark);
  

    // 2. Moltiplicazione tra il messaggio e il watermark
    for (size_t i = 0; i < BUFFER_SIZE; ++i){
      // msg_buffer[i] bit del messaggio
      // watermark[i] bit del watermark
      watermarked_buffer[i] = 0;
      for(size_t j = 0; j < sizeof(unsigned short) * 8; ++j){
        watermarked_buffer[i] |= ((msg_buffer[i] & (1 << j)) << j); //watermarked_buffer[i][2*j] = msg_buffer[i][j]
        watermarked_buffer[i] |= ((watermark[i] & (1 << j)) << (j + 1)); // [2*j+1] = watermark[i][j]
      }
    }
      

    printf("Watermarked message in hex: ");
    //formatted_print(watermarked_buffer);
    for(size_t i=0; i < BUFFER_SIZE; ++i)
      printf("%08X|", watermarked_buffer[i]);
    puts("\n");  
    
    printf("Watermarked message as a string: %s\n", (const char*)watermarked_buffer);

    // 3. Send the watermarked msg over the network
    // TODO: to change
    //unsigned short seq_num = (unsigned short) i;
    //fwrite(&seq_num, sizeof(unsigned short), 1, f);
    //char broken_byte = watermarked_buffer[0] | 0xff;
    //fwrite(&broken_byte, sizeof(char), 1, f);
    fwrite(watermarked_buffer, sizeof(unsigned int), BUFFER_SIZE, f);

  // TODO:
  fclose(f);
}

void recv(char* buffer, size_t length)
{
  // Alloca buffer
  assert(length == 16);
  //char _buffer[BUFFER_SIZE];
  unsigned short watermark[BUFFER_SIZE] = {0};
  unsigned int watermarked_msg[BUFFER_SIZE] = {0};
  unsigned short* recv_buffer = (unsigned short*) buffer;

  
  // TODO: change when network modem lib available
  FILE* f = fopen("transit_data.dat", "rb");

  fread(watermarked_msg, sizeof(unsigned int), BUFFER_SIZE, f);

  printf("In recv watermarked message in hex: ");
  formatted_print(watermarked_msg);
  printf("In recv watermarked message as a string: %s\n", (const char*)watermarked_msg);

  unsigned char info_bits = extract_watermark(watermarked_msg, HADAMARD_ROW);
  
  printf("In recv info bits in the recv as hex %hhX and %c as char \n", info_bits, info_bits);
  
  spreading(info_bits, HADAMARD_ROW, watermark);
  printf("Watermark in the recv\n");
  formatted_print(watermark);

  // 2. Moltiplicazione tra il messaggio e lo spreading code
  for (size_t i = 0; i < BUFFER_SIZE; ++i)
    recv_buffer[i] = watermarked_msg[i] - watermark[i];   

  printf("Reconstructed msg in the recv\n");
  formatted_print(recv_buffer);
}


