#ifndef __WBPLS_INTERNAL_H__
#define __WBPLS_INTERNAL_H__

// #define COS_BITS 14
// #define COS_BASE 1u << COS_BITS
// #define FLOAT_TO_INT(f) ((unsigned int) ((f) * (COS_BASE)))
// //#define INT_TO_FLOAT(i) ((float) ((i) >> COS_BITS))
// #define INT_TO_FLOAT(i) ((i) / ((float) (COS_BASE)))
// #define SAMPLES_COUNT 80
#define WATERMARK_INFO_LENGTH 8
#define SPREADING_CODE_LENGTH 16
#define WATERMARK_LENGTH WATERMARK_INFO_LENGTH * SPREADING_CODE_LENGTH
#define BUFFER_SIZE PACKET_SIZE 
//#define TS 12000

extern const unsigned short H16[SPREADING_CODE_LENGTH];

// extern unsigned f1f2[2][SAMPLES_COUNT];
// extern unsigned f3f4[2][SAMPLES_COUNT];

#endif