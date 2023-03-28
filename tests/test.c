#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wbpls.h>
#include <wbpls_internal.h>


void formatted_print(char* buffer) {
  for(size_t i=0; i < BUFFER_SIZE; ++i)
    printf("%hhx|", buffer[i]);
  puts("");
}

int main(){  
  /*char msg[PACKET_SIZE] = "a-secret";
  char spreading_code[BUFFER_SIZE];

  puts("The original message:");
  formatted_print(msg);

  printf("Ready to compute the sprearding code...\n");
  spreading(msg[0], 5, spreading_code);
  printf("Sprearding code: done!\n");

  formatted_print(spreading_code);
  
  printf("Ready to compute the watermarked msg...\n");
  for (size_t i = 0; i < BUFFER_SIZE; ++i)
    msg[i] ^= spreading_code[i];

  formatted_print(msg);
  printf("---%s---\n", msg);
  
  printf("Extracted watermark: %x\n", extract_watermark(msg, 5));
*/
  
  char msg[17] = "This_is_a_secret"; //"This_is_a_secret"; //"FKFKFKFKFKFKFKFK"; //"ABABABABABABABAB"; 
  //;//;// ;//; 
  //"WHAT_THE_FUCK!!!"
  //"FUCKFUCKFUCKFUCK"
  //"FUKFUKFUKFUKFUKF"
  //"FKFKFKFKFKFKFKFK"
  char buffer[17] = {0};

  printf("Messaggio originale:\n-%s-\n", msg);
  send(msg, strlen(msg));

  recv(buffer,16);

  printf("Messaggio letto:\n-%s-\n", buffer);

  return EXIT_SUCCESS;  
}