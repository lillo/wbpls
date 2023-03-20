#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "wbpls.h"

#include "wbpls_internal.h"

const Configuration Default;

#define HADAMARD_ROW 6 

/// @brief Hadamard matrix of size 16x16
/*
const unsigned short H16[16] = {
    32768, 49152, 40960, 53248,
    34816, 50176, 41472, 53504,
    32896, 49216, 40992, 53264,
    34824, 50180, 41474, 53505};
*/

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

static void formatted_print(char* buffer) {
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%hhX|", buffer[i]);
  puts("");
}

/// @brief Compute the spreading code for the watermarking
/// @param data a byte representing the watermark
/// @param row the row of Hadamard to use for the spreading
/// @param buffer an output buffer storing the spreading_code
void spreading(unsigned char data, size_t row, char buffer[BUFFER_SIZE])
{
  memset(buffer, 0, BUFFER_SIZE);

  for(int i = WATERMARK_INFO_LENGTH - 1; i > -1; --i, data >>= 1){
    if(data & 0x1)
      buffer[i] = H8[row];
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

unsigned short extract_watermark(char bitstream[BUFFER_SIZE], size_t hadamard_row){
  char correlation[BUFFER_SIZE] = {0};
  unsigned short extracted_wat = 0;
  unsigned char max = 0;
  
  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    correlation[i] = __builtin_popcount(bitstream[i] & H8[hadamard_row]);
    max = (max < correlation[i] ? correlation[i] : max);
  }

  for(size_t i=0; i < WATERMARK_INFO_LENGTH; ++i){
    extracted_wat |= (correlation[i] > 0.8 * max);
    extracted_wat <<= 1;   
    //if(i < WATERMARK_INFO_LENGTH - 1)  
    printf("%d", (correlation[i] > 0.8 * max));
  }

  printf("\nCorrelation:\n");
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%d|", correlation[i]);
  puts("");

  return extracted_wat;
}

void send(const char*msg, size_t length)
{
  // Alloca buffer
  char watermarked_buffer[BUFFER_SIZE];
  char spreading_code[BUFFER_SIZE];
  char tmp_buffer[BUFFER_SIZE];

  size_t num_blocks = (length / BUFFER_SIZE);
  size_t rem_bytes = length % BUFFER_SIZE;

  if(rem_bytes > 0)
    ++num_blocks;

  assert(num_blocks < USHRT_MAX);

  // TODO: change when network modem lib available
  FILE* f = fopen("transit_data.dat", "wb+");

  for(size_t i = 0; i < num_blocks; ++i){
    const char* msg_buffer;
    
    if(i == num_blocks - 1 && rem_bytes > 0){
      memset(tmp_buffer, 0, BUFFER_SIZE);
      memcpy(tmp_buffer, msg + BUFFER_SIZE *i, rem_bytes);
      msg_buffer = tmp_buffer;
    }else 
      msg_buffer = msg + BUFFER_SIZE *i;

    // 1. Watermark primi 8 bit del messaggio pkt->data[0]
    // Calcolo dello spreading code
    spreading(msg_buffer[0], HADAMARD_ROW, spreading_code);

    printf("Watermark in the send: %hhX\n", msg_buffer[0]);

  printf("Spreading code in the send\n");
  formatted_print(spreading_code);

    // 2. Moltiplicazione tra il messaggio e lo spreading code
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
      watermarked_buffer[i] = msg_buffer[i] ^ spreading_code[i];

    // 3. Send the watermarked msg over the network
    // TODO: to change
    unsigned short seq_num = (unsigned short) i;
    fwrite(&seq_num, sizeof(unsigned short), 1, f);
    //char broken_byte = watermarked_buffer[0] | 0xff;
    //fwrite(&broken_byte, sizeof(char), 1, f);
    fwrite(watermarked_buffer, sizeof(char), BUFFER_SIZE, f);

  }
  // TODO:
  fclose(f);
}

void recv(char* buffer, size_t length)
{
  // Alloca buffer
  //char _buffer[BUFFER_SIZE];
  char spreading_code[BUFFER_SIZE];
  char watermarked_msg[BUFFER_SIZE];

  size_t num_blocks = (length / BUFFER_SIZE);
  size_t rem_bytes = length % BUFFER_SIZE;

  if(rem_bytes > 0)
    ++num_blocks;

  // TODO: change when network modem lib available
  FILE* f = fopen("transit_data.dat", "rb+");

  for(size_t i=0; i < num_blocks; ++i){
    char *recv_buffer = buffer + BUFFER_SIZE * i; 
    unsigned short seq_num;
    size_t nbytes;
    fread(&seq_num, sizeof(unsigned short), 1, f);
    nbytes = fread(watermarked_msg, sizeof(char), (i == num_blocks - 1 && rem_bytes > 0 ? rem_bytes : BUFFER_SIZE), f);

    char watermark = extract_watermark(watermarked_msg, HADAMARD_ROW);
    spreading(watermark, HADAMARD_ROW, spreading_code);


    printf("Watermark in the recv: %hhX\n", watermark);

    printf("Spreading code in the recv\n");
    formatted_print(spreading_code);

    // 2. Moltiplicazione tra il messaggio e lo spreading code
    for (size_t i = 0; i < nbytes; ++i)
      recv_buffer[i] = watermarked_msg[i] ^ spreading_code[i];   
  }

}

