#include <stdlib.h>
#include <string.h>

#include <e-lib.h>

#define USE_DMA
#ifdef USE_DMA
#	define e_memcopy(dst, src, size) e_dma_copy(dst, src, size)
#else
#	define e_memcopy(dst, src, size) memcpy(dst, src, size)
#endif

#define _BUF_SZ (8*1024)

// test results data structure
typedef struct {
	unsigned int sram_clocks_write;
	unsigned int sram_clocks_read;
	unsigned int eram_clocks_write;
	unsigned int eram_clocks_read;
	unsigned int eot;
} test_t;

volatile char   Abuf[_BUF_SZ] SECTION(".data_bank1"); // local A (src) buffer
volatile char   Bbuf[_BUF_SZ] SECTION(".data_bank2"); // local B (dest) buffer


int main(void){
  test_t *me = (void *) 0x7000;
  void   *pmem, *src, *dst;
  
  pmem = (void *) (e_emem_config.base + 0x01000000);
  me->eot = 0;
  
  me->sram_clocks_read = me->sram_clocks_write = 11223344;
  me->eram_clocks_read = me->eram_clocks_write = 55667788;
  
  me->eot = 1;
  
  // Initialize timer
  e_irq_mask(E_TIMER0_INT, E_TRUE);
  e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
  
  // Measure time for local -> SRAM
  src = (void *) (Abuf);
  dst = (void *) e_get_global_address(0, 1, (void *) 0x2000);
  me->sram_clocks_write = e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
  e_memcopy(dst, src, _BUF_SZ);
  me->sram_clocks_write -= e_ctimer_get(E_CTIMER_0);
  
  me->eot = 2;
  
  // Measure time for SRAM -> local
  src = (void *) e_get_global_address(0, 1, (void *) 0x4000);
  dst = (void *) (Bbuf);
  me->sram_clocks_read = e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
  e_memcopy(dst, src, _BUF_SZ);
  me->sram_clocks_read -= e_ctimer_get(E_CTIMER_0);
  
  me->eot = 3;
  
  // Measure time for local -> ERAM
  src = (void *) (Abuf);
  dst = (void *) (pmem+0x0000);
  me->eram_clocks_write = e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
  e_memcopy(dst, src, _BUF_SZ);
  me->eram_clocks_write -= e_ctimer_get(E_CTIMER_0);
  
  me->eot = 4;
  
  // Measure time for ERAM -> local
  src = (void *) (pmem+0x0000);
  dst = (void *) (Bbuf);
  me->eram_clocks_read = e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
  e_memcopy(dst, src, _BUF_SZ);
  me->eram_clocks_read -= e_ctimer_get(E_CTIMER_0);
  
  me->eot = 5;
  
  return EXIT_SUCCESS;
}


