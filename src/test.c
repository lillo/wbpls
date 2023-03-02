#include <stdio.h>
#include <math.h>
#include <wbpls.h>
#include "wbpls_internal.h"

/*
void tabulate_cosine_test(float freq, unsigned int table[SAMPLES_COUNT]){
  //double delta = (2 * M_PI * freq) / SAMPLES_COUNT;
  //double delta = SAMPLES_COUNT / freq;
  double delta = 1.0/ SAMPLES_COUNT;

  printf("delta: %f\n", delta);  

  for(unsigned int i=0; i < SAMPLES_COUNT; ++i){
    double a =  2 * M_PI * freq * delta * i;
    printf("%f, ", cos(a));
  }
}

void print_cosine(unsigned int ff[2][80]){
    
    for(size_t j=0; j < 2; ++j) {
        printf("Frequency: %zu\n", j);

        for(size_t i=0; i < 80; ++i){
            printf("%f, ", INT_TO_FLOAT(ff[j][i]));
        }
        printf("\n\n");
    }
}
*/

int main(){
    unsigned int buffer[BUFFER_SIZE];
    init(NULL);
    printf("%d\n", BUFFER_SIZE);
    mod_data("A5A5A5A5", f1f2, buffer);
    for(size_t i=0; i < BUFFER_SIZE; ++i)
      printf("%.4f, ", INT_TO_FLOAT(buffer[i]));

    //print_cosine(f1f2);
    //
    //float a = 0.4f;
    //unsigned int b = FLOAT_TO_INT(a);
    //float c = b / ((float) (COS_BASE));
    //printf("%f -- %u -- %f\n", a, b, c);
    //print_cosine(f3f4);
    //tabulate_cosine_test(6.0, NULL);
    return 0;
    
}