#ifndef __WBPLS_H__
#define __WBPLS_H__

#define PACKET_SIZE 8

extern const double deffreq0msg;
extern const double deffreq1msg;
extern const double deffreq0sc;
extern const double deffreq1sc;

// Types
typedef struct {
  unsigned int fmsg0, fmsg1, fsc0, fsc1;
} Configuration;

extern const Configuration Defaultpyth;

typedef struct {
  char data[PACKET_SIZE];
} Pack;

typedef Pack* Packet;

// Function prototypes

/*
 *  init:
 *  send: sends a packet
 *  recv:
 */
void init(const Configuration*);
void send(Packet);
Packet recv();

#endif