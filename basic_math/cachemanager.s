# Epiphany Cache Manager (with support for 20 functions)
# Copyright (c) 2013, Adapteva, Inc.
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of Adapteva nor the names of its contributors may be
#    used to endorse or promote products derived from this software without
#    specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

/* Current PLT Design:
   [....] Push LR to stack
   [....] Jump with Link to cachemanager
   [....] Startaddr
   [....] Size (or should this be endaddr?) */

	.section .cacheman,"ax",@progbits
	.balign 4
	.global cacheman
	.type cacheman, @function
cacheman:
	;; lr points to address to actual function (and size)
	ldrd r46,[lr]
	;; Load high and low of cachespace (and end of table)
	mov r48, %low(cachespacebot)
	movt r48, %high(cachespacebot)
	mov r49, %low(cachespacetop)
	movt r49, %high(cachespacetop)
	add r63, r48, #160

	;; We store a table of used space, in a similar design to
	;; | startaddr | endaddr |
	;; |-----------+---------|
	;; |      0x80 |    0x81 |
	;; Note we use r56-r59 as temporary registers throughout
;;; # iter = 0
;;; FIXME: Assign this to another register
	mov r62, #0		; Iter counter
.cachebegin:
;;; row = CACHESPACEBOT
	mov r54, r48		; Current row pointer
	;; We want to check that we actually have a free table space
	;; This is done by checking that the end entry has a start address
	;; equal to cachespacetop
;;; if (start[20] != CACHESPACETOP) goto endoftable
	ldr r56, [r48, #38]	; final entry start addr (152>>2)
	sub r56, r56, r49
	bne .endoftable

	;;  We don't use all the data *THIS* iteration, but doing a double
	;;  load here means we don't have to do lots of little loads later
;;; currentrow = *row++
	ldrd r52, [r54], #1
.checkloop:
;;; previousrow = currentrow
	mov r50, r52		; move element across
	mov r51, r53
;;; currentrow = *row++
	ldrd r52, [r54], #1	; load next entry
;;; availsize = currentrow[start]-previousrow[end]
	sub r55, r52, r51
;;; if (availsize >= neededsize) goto copy
	sub r56, r55, r47
	bgte .copy
	; check if we are at end of table
;;; if (row < CACHESPACETOP+160) goto checkloop
	sub r56, r54, #160	; 160 bytes, 20 loaded
	sub r56, r56, r48
	blt .checkloop

.endoftable:
	;; At this stage we are out of table or space
	;; If this is our second iteration through the system, we can't
	;; load any code so fail
	sub r56, r53, #1
	beq .cachefail
	;; Set itercount and jump back
	mov r62, #1
	b .unload

.copy:
	;; At this stage we have:
	;; r46  = srcaddr
	;; r51	= destdir
	;; r47  = size
	mov r56, r51	 	; current destaddr
;;; endaddr = srcdir+size
	add r57, r46, r47	; endaddr + 1
	mov r58, r46		; current srcdir
.copyloop:
;;; if (current_srcdir >= endaddr) goto .copyloopend
	sub r59, r58, r57
	bgte .copyloopend	; finished copying
;;; *currentdestdir++ = *currentsrcdir++
;;; ???: Can't do double load because functions may not be 8 byte aligned
	ldr r60, [r58], #1
	str r60, [r56], #1
	b .copyloop
.copyloopend:
	;; Store loaded memory table entry and sort
	mov r52, r51
	mov r53, r56
;;; row = CACHESPACEBOT + 152
	add r54, r48, #152		; row pointer for final entry
;;; *row = currentrow
	strd r52, [r54]
.copysortloop:
;;; previousrow = *(row-1)
	ldrd r50, [r54, #-1]
;;; if currentrow[start] > previousrow[start] goto editplt
	sub r56, r52, r50
	bgte .editplt
;;; *row = previousrow; *(row-1) = currentrow
	strd r50, [r54]
	strd r52, [r54, #-1]
;;; *row--
	sub r54, r54, #8
;;; if (row != CACHESPACETOP) goto copysortloop
	sub r56, r54, r48
	bne .copysortloop
.editplt:
	;; diff = destaddr - (lr-8)
	sub r50, r52, lr
	add r50, r50, #8
;;; diff >>=1, <<=8  (mask and shift bits)
	lsr r50, r50, #1
	lsl r50, r50, #8
;;; diff |= 0xe8 (unconditional branch)
	mov r56, 0xe8
	orr r50, r50, r56
;;; *(lr-1) = diff
	str r50, [lr, #-1]

	;; Restore original link register and stack
	ldr lr, [sp, #2]
	add sp, sp, #8
	;; branch to moved fn
	jr r52

.cachefail:
	;; If we are loading this, then we have failed to load the cache.
	;; Currently we trap and set R3 to 'PIC1'
	mov r3, 0x4331
	movt r3, 0x5049
	trap 3

;;;
;;; UNLOADING STARTS HERE
;;; Note: don't clobber r46 (addr), r47 (size), r48 (top), r49 (bot), r62/63
;;; (Using same register meanings as above)
;;;
.unload:
	add r54, r48, #8	; First table entry is second row
.unloadloop:
	;; Load table entry
	ldrd r52, [r54], #1
	;; If the start address equals the end of space, we are done
	sub r56, r52, r49
	bgte .unloadfinish

	;; For each stack frame, if lr is within range, then this is still loaded
	;; r56 - lr, r57 - fp,   if (lr-min)<0 or (max-lr)<0 then out of range
	;; FIXME: offsets may be wrong, but algorithm is right
	ldr r56, [sp, #+2]	; first lr
	mov r57, sp     	; first sp
.unloadframeloop:
	sub r55, r56, r52
	blt .unloadnextframe
	sub r55, r53, r56
	blt .unloadnextframe
	b .unloadnextentry

.unloadnextframe:
	;; FIXME: Check whether ldrd is valid
	;; sub {at top}
	;; bne .unloaddelframe
	ldr r56, [r57, #+2]
	ldr r57, [r57, #+1]
	sub r57, r57, #0
	bne .unloadframeloop

.unloaddelframe:
	;; We have a frame we want to clean out, fill it with endtable value
	str r49, [r54, #-2]
	str r49, [r54, #-1]
;;; Sort
	mov r50, r54
	;; If we are beyond the end of the table, we are done
.unloadsort:
	sub r55, r50, r63	; (current row - end of table)
	;; 	sub r55, r55, #8
	bgte .unloadsortend
	;; Load row and previous row
	ldrd r56, [r50, #-1]
	ldrd r58, [r50]
	;; if previous is less than next, swap
	sub r55, r58, r56
	bgt .unloadnoswap
	strd r56, [r50]
	strd r58, [r50, #-1]
.unloadnoswap:
	add r50, r50, #8
	b .unloadsort

.unloadsortend:
	;; Finally, we need to check this entry again and clear its PLT entry
	sub r54, r54, #8
	;; We load the top of the PLT and search for the start address of
	;; the cache we are unloading
	mov r56, %low(_PROCEDURE_LINKAGE_TABLE+4)
	movt r56, %high(_PROCEDURE_LINKAGE_TABLE+4)
.unloadnextsort:
	ldr r57, [r56], #4
	sub r57, r57, r52
	bne .unloadnextsort

	;; Calculate and write new PLT
	sub r56, r56, #16
	mov r57, %low(cacheman)
	movt r57, %high(cacheman)
	sub r57, r57, r56
	lsr r57, r57, #1
	lsl r57, r57, #8
	mov r58, 0xf8
	orr r57, r57, r58
	str r57, [r56]
	;; (And put back in original store and branch)
	;; str lr, [sp], -0x2
	mov r57, 0xd55c
	movt r57, 0x2700
	str r57, [r56, #-1]

.unloadnextentry:
	;; Check next entry if not done
	sub r56, r54, #160
	sub r56, r56, r48
	blt .unloadloop

.unloadfinish:
	;; Branch to second iteration
	b .cachebegin

	.size  cacheman, .-cacheman

	;; Starting Cache Entry Table
	;; ???: Should this be elsewhere?
	.balign 8
	.section .cachedata,"awx"
	.4byte cachespacebot+160
	.4byte cachespacebot+160
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
