#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "wbpls.h"

const char H8[][] = {{1,1,1,1,1,1,1,1},
                    {1,0,1,0,1,0,1,0},
                    {1,1,0,0,1,1,0,0},
                    {1,0,0,1,1,0,0,1},
                    {1,1,1,1,0,0,0,0},
                    {1,0,1,0,0,1,0,1},
                    {1,1,0,0,0,0,1,1},
                    {1,0,0,1,0,1,1,0}};


uint intcos(double c) {
  return ((cos(c)+1)*((UINT_MAX-1) >> 1));
}

void init(Configuration* conf) {
  if(conf == NULL) {
    conf = *DefaultConfiguration;
  }
}

void send(Packet) {

  // 1. Packet modulation (multiplies bits by cos of bit 1 and 0)

  // 2. spreading code modulation (multiplies cw by cos 1 and 0 -- for spreading code)

  // 3. allign and sum modulations' results

  // 4. split to datagrams and transmit
}

Packet recv() {
  return NULL;
}

int main() {

}
