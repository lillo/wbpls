#ifndef __WBPLS_H__
#define __WBPLS_H__

#define PACKET_SIZE 8

// Types
typedef struct {
  unsigned int hadamard_row;
} Configuration;

extern const Configuration Defaultpyth;

// Function prototypes

/*
 *  init:
 *  send: sends a packet
 *  recv:
 */
void init(const Configuration*);
void send(const char* msg, size_t length);
void recv(char* buffer, size_t length);

#endif