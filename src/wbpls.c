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
  34824, 50180, 41474, 53505
};

void init(const Configuration* conf) {
  if(conf == NULL) {
    conf = &Default;
  }

}


/// @brief Compute the spreading code for the watermarking
/// @param data a byte representing the watermark  
/// @param row the row of Hadamard to use for the spreading
/// @param buffer an output buffer storing the spreading_code 
void spreading(unsigned char data, size_t row, char buffer[WATERMARK_LENGTH]){
  memset(buffer, 0, WATERMARK_LENGTH);
  for(size_t i=WATERMARK_LENGTH - 1; i >= 0 ; i -= SPREADING_CODE_LENGTH, data >>= 1){
    if(data & 0x1){
      memcpy(buffer + i, H16[row], sizeof(unsigned short));
    }
  }
}


void send(Packet pkt) {
  // Alloca buffer 
  //unsigned int buffer1[BUFFER_SIZE];
  //unsigned int buffer2[BUFFER_SIZE];
  char watermarked_buffer[BUFFER_SIZE];
  char spreading_code[WATERMARK_LENGTH];


  // 1. Watermark primi 8 bit del messaggio pkt->data[0]
  // Calcolo dello spreading code
  spreading(pkt->data[0], 0, spreading_code);
  
  // 2. Moltiplicazione tra il messaggio e lo spreading code
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    watermarked_buffer[i] = pkt->data[i] | spreading_code[i];
  
  // 3. split to datagrams and transmit watermarked_buffer

}

Packet recv() {
  return NULL;
}

