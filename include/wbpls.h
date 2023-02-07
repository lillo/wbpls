#ifndef __WBPLS_H__
#define __WBPLS_H__

#define PACKET_SIZE 8

const double deffreq0msg = 2400.0;
const double deffreq1msg = 3600.0;
const double deffreq0sc = 4900.0;
const double deffreq1sc = 5100.0;

// Types
typedef struct {
  unsigned int fmsg0, fmsg1, fsc0, fsc1;
} Configuration;

const Configuration Default = {deffreq0msg, deffreq1msg, deffreq0sc, deffreq1sc};

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