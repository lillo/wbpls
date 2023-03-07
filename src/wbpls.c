#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "wbpls.h"

#include "wbpls_internal.h"

/*
const char H8[][16] = {
    { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},    
    { 1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  1,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0, 0},
    { 1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0, 0},
    { 1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0, 0},
    { 1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0, 0},
    { 1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0, 0},  
    { 1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, 0},
    { 1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0, 0},
    { 1,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1, 0},
    { 1,  1,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0, 1}
};
*/

// const double deffreq0msg = 2.0;
// const double deffreq1msg = 4.0;
// const double deffreq0sc = 7.0;
// const double deffreq1sc = 9.0;

const Configuration Default; // = {deffreq0msg, deffreq1msg, deffreq0sc, deffreq1sc};

/// @brief Hadamard matrix of size 16x16
const unsigned short H16[16] = {
  32768, 49152, 40960, 53248,
  34816, 50176, 41472, 53504,
  32896, 49216, 40992, 53264,
  34824, 50180, 41474, 53505
};

// unsigned f1f2[2][SAMPLES_COUNT] = {0};
// unsigned f3f4[2][SAMPLES_COUNT] = {0};


/// @brief Assume len(buffer) == BUFFER_SIZE
/// @param data the packet to be modulated
/// @param f an array of length 2 where f[0] is the encoding of bit 0 and f[1] is the encoding of bit 1
/// @param buffer the resulting buffer 
// void mod_data(char* data, unsigned int f[2][SAMPLES_COUNT], unsigned int * buffer) {
//   for(size_t i = 0, k=BUFFER_SIZE; i < (PACKET_SIZE/sizeof(unsigned long)); i += sizeof(unsigned long)){
//     //printf("It: %d\n - k = %d", i, k);
//     unsigned long bitset = *((unsigned long*)(data + i));
//     //printf(" bitset: %x", bitset);
//     for(size_t z=0; z < sizeof(bitset) * 8; ++z){
//       //printf("z= %d", z);
//       for(size_t j=SAMPLES_COUNT; 0 < j; --j){
//         //printf("j = %d", j); 
//         buffer[--k] = f[bitset & 0x1][j];
//       }
//       bitset >>= 1;
//     }
//   }
// }/// 


//
// void demod_data(unsigned int* buffer, float f[2], char* data){

// }

/// @brief Sample the cosine of a given frequency
/// @param freq the frequency 
/// @param table it stores the result of sampling the cosine
// void tabulate_cosine(float freq, unsigned int table[SAMPLES_COUNT]){
//   float delta = 1.0 / SAMPLES_COUNT;

//   //printf("\n\nTabulate cos(%f)\n\n", freq);
//   for(unsigned int i=0; i < SAMPLES_COUNT; ++i){
//     //printf("%f,", cosf(2 * M_PI * freq * delta * i));
//     table[i] = FLOAT_TO_INT(cosf(2 * M_PI * freq * delta * i));
//   }
//   printf("\n\n");
// }

void init(const Configuration* conf) {
  if(conf == NULL) {
    conf = &Default;
  }

  // tabulate_cosine(conf->fmsg0, f1f2[0]);
  // tabulate_cosine(conf->fmsg1, f1f2[1]);
  // tabulate_cosine(conf->fsc0, f3f4[0]);
  // tabulate_cosine(conf->fsc1, f3f4[1]);
}

// WATERMARK LENGTH
/// @brief Compute the spreading code for the watermarking
/// @param data  
/// @param row 
/// @param buffer  
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
  unsigned int watermarked_buffer[BUFFER_SIZE];
  unsigned char spreding_code[WATERMARK_LENGTH];


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
