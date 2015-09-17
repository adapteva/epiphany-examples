#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <e-lib.h>

#include "common.h"

union ptr_t {
    uint8_t  *u8;
    uint16_t *u16;
    uint32_t *u32;
    uint64_t *u64;
};

int main(void)
{
  int N;
  int i;
  volatile uint32_t *flag     = (uint32_t *) DEV_ADDR_FLAG;
  volatile uint32_t *val      = (uint32_t *) DEV_ADDR_VAL;
  volatile uint32_t *addr     = (uint32_t *) DEV_ADDR_ADDR;
  volatile uint32_t *phase = (uint32_t *) DEV_ADDR_PHASE;
  volatile uint32_t *expect   = (uint32_t *) DEV_ADDR_EXPECT;
  volatile const uint32_t *host_ack  = (uint32_t *) DEV_ADDR_HOST_ACK;
  volatile const uint32_t *host_stop = (uint32_t *) DEV_ADDR_HOST_STOP;

//  uint32_t coreid = e_get_coreid();
//  coreid -= 0x808;
//  uint32_t coreoff = (coreid >> 6) * 4 + coreid % 64;

  volatile union ptr_t extmem = { .u32 = (uint32_t *) 0x8e000000 };
//  extmem.u32 = (uint32_t *) (((uintptr_t) extmem.u32) + ((uintptr_t) coreoff));

  *val = 0;
  *addr = 0;
  *phase = 0;

  while (*flag)
     ;

  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;

  /* 32 bit R/W */
  N = 1*1024*1024/4;
  for (i = 0; i < N; i++)
      extmem.u32[i] = 0xFFFFFFFF;

  N = 1*1024*1024/4;
  for (i = 0; i < N; i++)
      extmem.u32[i] = 0xACCE5500 | (i % 256);

  for (i = 0; i < N; i++) {
      *expect = (0xACCE5500 | (i % 256));
      *val = extmem.u32[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u32[i];
          return 1;
      }
  }
  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;

  for (i = 0; i < N; i++)
      extmem.u32[i] = ~0xACCE55FF | (i % 256);

  for (i = 0; i < N; i++) {
      *expect = (~0xACCE55FF | (i % 256));
      *val = extmem.u32[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u32[i];
          return 1;
      }
  }
  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;

  /* 16 bit R/W */
  N = 1*1024*1024/4;
  for (i = 0; i < N; i++)
      extmem.u32[i] = 0xFFFFFFFF;

  N = 1*1024*1024/2;
  for (i = 0; i < N; i++)
      extmem.u16[i] = 0xDA00 | (i % 256);

  for (i = 0; i < N; i++) {
      *expect = (0xDA00 | (i % 256));
      *val = extmem.u16[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u16[i];
          return 1;
      }
  }
  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;


  for (i = 0; i < N; i++)
      extmem.u16[i] = ~0xDAFF | (i % 256);

  for (i = 0; i < N; i++) {
      *expect = (~0xDAFF | (i % 256));
      *val = extmem.u16[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u16[i];
          return 1;
      }
  }
  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;


  /* 8 bit R/W */
  N = 1*1024*1024/4;
  for (i = 0; i < N; i++)
      extmem.u32[i] = 0xFFFFFFFF;

  N = 1*1024*1024/1;
  for (i = 0; i < N; i++)
      extmem.u8[i] = 0xA0 | (i % 16);

  for (i = 0; i < N; i++) {
      *expect = (0xA0 | (i % 16));
      *val = extmem.u8[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u8[i];
          return 1;
      }
  }
  *phase += 1;
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;


  for (i = 0; i < N; i++)
      extmem.u16[i] = (~0xAF | (i % 16));

  for (i = 0; i < N; i++) {
      *expect = (~0xAF | (i % 16));
      *val = extmem.u8[i];
      if (*val != *expect) {
          *flag = FAIL;
          *addr = (uint32_t) &extmem.u8[i];
          return 1;
      }
  }
  *phase += 1;

#if 0
  while (*host_ack != *phase)
      ;
  if (*host_stop)
      return 0;
#endif


  *flag = OK;

  //Put core in idle state
  __asm__ __volatile__("idle");
}
