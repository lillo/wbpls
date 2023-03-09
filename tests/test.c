#include <stdio.h>
#include <stdlib.h>
#include <wbpls.h>
#include <wbpls_internal.h>


void formatted_print(char* buffer) {
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%hhx|", buffer[i]);
  puts("");
}

int main(){
  char msg[PACKET_SIZE] = "A5A5A5A5";
  char spreading_code[BUFFER_SIZE];

  puts("The original message:");
  formatted_print(msg);

  printf("Ready to compute the sprearding code...\n");
  spreading(msg[0], 4, spreading_code);
  printf("Sprearding code: done!\n");

  formatted_print(spreading_code);
  
  printf("Ready to compute the watermarked msg...\n");
  for (size_t i = 0; i < BUFFER_SIZE; ++i)
    msg[i] ^= spreading_code[i];

  formatted_print(msg);
  
  printf("Extracted watermark: %x\n", extract_watermark(msg, 4));

  return EXIT_SUCCESS;  
}