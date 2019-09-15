/*
  fft2d.h

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yainv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/


#ifndef __FFT2D_H__
#define __FFT2D_H__

#include <stdint.h>
#include "fft2dlib.h"
#ifndef __HOST__
#include <e_coreid.h>
#endif // __HOST__

#define _Nchips 1                  // # of chips in operand matrix side
#define _Nside  4                  // # of cores in chip side
#define _Ncores (_Nside * _Nside)  // Num of cores = 16
#ifndef _lgSfft
#define _lgSfft 7                  // Log2 of size of 1D-FFT
#endif
#define _Sfft   (1<<_lgSfft)       // Size of 1D-FFT
#define _Score  (_Sfft / _Ncores)  // Num of 1D vectors per-core
#define _Schip  (_Score * _Ncores) // Num of 1D vectors per-chip
#define _Smtx   (_Schip * _Sfft)   // Num of elements in 2D array

#define _Nbanks 2                  // Num of SRAM banks on core

#define _BankA  0
#define _BankW  1
#define _BankB  2
#define _BankP  3
#define _PING   0
#define _PONG   1


#ifdef __Debug__
#define dstate(x) { me.mystate = (x); }
#else
#define dstate(x)
#endif

#define TIMERS 10

#if 0
#if _lgSfft == 5
#	warning "LgFFt = 5"
#elif _lgSfft == 6
#	warning "LgFFt = 6"
#elif _lgSfft == 7
#	warning "LgFFt = 7"
#endif
#endif

#ifdef __Debug__
typedef struct  /*__attribute__((packed))*/ {
#else
typedef struct __attribute__((packed)) __attribute__((aligned(8))) {
#endif
	int        corenum;
	int        row;
	int        col;

	int        mystate;

	volatile int go_sync;           // The "go" signal from prev core
	volatile int sync[_Ncores];     // Sync with peer cores
	int:32;                         // Pad
	union {
		uint64_t:64;
		volatile int *tgt_go_sync;       // ptr to go_sync of next core
	} __attribute__((packed)) __attribute__((aligned(8)));
	union {
		uint64_t _pad0[_Ncores];
		volatile int *tgt_sync[_Ncores]; // ptr to sync of target neighbor
	} __attribute__((packed)) __attribute__((aligned(8)));

	union {
		uint64_t _pad1[_Nbanks][2];
		volatile cfloat *bank[_Nbanks][2];            // Ping Pong Bank local space pointer
	} __attribute__((packed)) __attribute__((aligned(8)));

	union {
		uint64_t _pad2[_Ncores][_Nbanks][2];
		volatile cfloat *tgt_bk[_Ncores][_Nbanks][2]; // Target Bank for matrix rotate in global space
	} __attribute__((packed)) __attribute__((aligned(8)));

	uint32_t time_p[TIMERS]; // Timers
} __attribute__((packed)) __attribute__((aligned(8))) core_t;


typedef struct __attribute__((packed)) __attribute__((aligned(8))) {
	volatile int64_t  go;     // Call for FFT2D function
	volatile int      ready;  // Core is ready after reset
} __attribute__((packed)) __attribute__((aligned(8))) mbox_t;


typedef struct __attribute__((packed)) __attribute__((aligned(8))) {
	volatile cfloat A[_Smtx]; // Global A matrix
	volatile cfloat B[_Smtx]; // Global B matrix
	volatile mbox_t core;
} __attribute__((packed)) __attribute__((aligned(8))) shared_buf_t;


typedef struct __attribute__((packed)) __attribute__((aligned(8))) {
	union {
		uint64_t:64;
		void            *pBase; // ptr to base of shared buffers
	} __attribute__((packed)) __attribute__((aligned(8)));
	union {
		uint64_t:64;
		volatile cfloat *pA;    // ptr to global A matrix
	} __attribute__((packed)) __attribute__((aligned(8)));
	union {
		uint64_t:64;
		volatile cfloat *pB;    // ptr to global B matrix
	} __attribute__((packed)) __attribute__((aligned(8)));
	union {
		uint64_t:64;
		mbox_t          *pCore; // ptr to cores mailbox
	} __attribute__((packed)) __attribute__((aligned(8)));

} __attribute__((packed)) __attribute__((aligned(8))) shared_buf_ptr_t;


typedef enum {
	e_fft_fwd = 0,
	e_fft_bwd = 1,
} fft_dir_t;

#endif // __FFT2D_H__
