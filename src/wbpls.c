#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "wbpls.h"

#include "wbpls_internal.h"

const Configuration Default;

/// @brief Hadamard matrix of size 16x16
const unsigned short H16[16] = {
    32768, 49152, 40960, 53248,
    34816, 50176, 41472, 53504,
    32896, 49216, 40992, 53264,
    34824, 50180, 41474, 53505};

const char H8[8] = {
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

/// @brief Compute the spreading code for the watermarking
/// @param data a byte representing the watermark
/// @param row the row of Hadamard to use for the spreading
/// @param buffer an output buffer storing the spreading_code
void spreading(unsigned char data, size_t row, char buffer[BUFFER_SIZE])
{
  memset(buffer, 0, BUFFER_SIZE);
  for(int i = BUFFER_SIZE - 1; i > -1; --i, data >>= 1){
    if(data & 0x1)
      buffer[i] = H8[row];
  }
}

unsigned char reverse_bits(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

unsigned short extract_watermark(char bitstream[BUFFER_SIZE], size_t hadamard_row){
  char correlation[BUFFER_SIZE] = {0};
  unsigned short extracted_wat = 0;
  unsigned char max = 0;

  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    correlation[i] = __builtin_popcount(bitstream[i] & H8[hadamard_row]);
    max = (max < correlation[i] ? correlation[i] : max);
  }

  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    extracted_wat |= (correlation[i] > 0.7 * max);
    extracted_wat <<= 1;
  }

  return reverse_bits(extracted_wat);
}

void send(Packet pkt)
{
  // Alloca buffer
  // unsigned int buffer1[BUFFER_SIZE];
  // unsigned int buffer2[BUFFER_SIZE];
  char watermarked_buffer[BUFFER_SIZE];
  char spreading_code[BUFFER_SIZE];

  // 1. Watermark primi 8 bit del messaggio pkt->data[0]
  // Calcolo dello spreading code
  spreading(pkt->data[0], 0, spreading_code);

  // 2. Moltiplicazione tra il messaggio e lo spreading code
  for (size_t i = 0; i < BUFFER_SIZE; ++i)
    watermarked_buffer[i] = pkt->data[i] ^ spreading_code[i];

  // 3. split to datagrams and transmit watermarked_buffer
}

Packet recv()
{
  return NULL;
}
