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
	add r63, r48, #320

	;; We store a table of used space, in a similar design to
	;; | startaddr | endaddr | ref count | plt line|
	;; |-----------+---------+-----------+---------|
	;; |      0x80 |    0x81 |           |         |
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
	ldr r56, [r48, #76]	; final entry start addr (304>>2)
	sub r56, r56, r49
	bne .endoftable

	;;  We don't use all the data *THIS* iteration, but doing a double
	;;  load here means we don't have to do lots of little loads later
;;; currentrow = *row++
	ldrd r52, [r54], #2
.checkloop:
;;; previousrow = currentrow
	mov r50, r52		; move element across
	mov r51, r53
;;; currentrow = *row++
	ldrd r52, [r54], #2	; load next entry
;;; availsize = currentrow[start]-previousrow[end]
	sub r55, r52, r51
;;; if (availsize >= neededsize) goto copy
	sub r56, r55, r47
	bgte .copy
	; check if we are at end of table
;;; if (row < CACHESPACETOP+320) goto checkloop
	sub r56, r54, #320	; 320 bytes, 20 loaded
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
	mov r54, r51
	mov r55, r56
;;; row = CACHESPACEBOT + 304
	add r58, r48, #304		; row pointer for final entry
;;; *row = currentrow
	strd r54, [r58]
;;; set ref count to 1
	mov r56, #1
;;; store plt entry
        mov r57, lr
        strd r56, [r58, #+1]
.copysortloop:
;;; previousrow = *(row-1)
	ldrd r50, [r58, #-2]
        ldrd r52, [r58, #-1]
;;; if currentrow[start] > previousrow[start] goto editplt
	sub r59, r54, r50
	bgte .editplt
;;; *row = previousrow; *(row-1) = currentrow
	strd r50, [r58]
        strd r52, [r58, #+1]
	strd r54, [r58, #-2]
        strd r56, [r58, #-1]
;;; *row--
	sub r58, r58, #16
;;; if (row != CACHESPACETOP) goto copysortloop
	sub r59, r58, r48
	bne .copysortloop
.editplt:
	mov r59, %low(countedcall)
	movt r59, %high(countedcall)
        sub r59, r59, #4
	;; diff = destaddr - (lr-8)
	sub r50, r59, lr
	add r50, r50, #8
;;; diff >>=1, <<=8  (mask and shift bits)
	lsr r50, r50, #1
	lsl r50, r50, #8
;;; diff |= 0xe8 (unconditional branch)
	mov r56, 0xf8
	orr r50, r50, r56
;;; *(lr-1) = diff
	str r50, [lr, #-1]

	str lr, [sp, #2]
        jalr r54

	;; Scan cachedata table for LR. We are called from the PLT.
	mov r48, %low(cachespacebot)
	movt r48, %high(cachespacebot)
	add r55, r48, #24

	ldr r53, [sp, #2]

.scanloop:
	ldrd r50, [r55], #2
	sub r52, r51, r53
        bne .scanloop

        sub r50, r50, #1
        str r50, [r55, #-4]

	ldr lr, [sp, #4]
        add sp, sp, #16
	rts

	/* ;; Restore original link register and stack */
	/* ldr lr, [sp, #2] */
	/* add sp, sp, #8 */
	/* ;; branch to moved fn */
	/* jr r54 */

.cachefail:
	;; If we are loading this, then we have failed to load the cache.
	;; Currently we trap and set R3 to 'PIC1'
	mov r3, 0x4331
	movt r3, 0x5049
	trap 3

;;; UNLOADING STARTS HERE
;;; Note: don't clobber r46 (addr), r47 (size), r48 (top), r49 (bot), r62/63
;;; (Using same register meanings as above)

.unload:
	add r51, r48, #16	; First table entry is second row
.unloadloop:
	;; Load table entry
	ldrd r52, [r51], #2
	;; If the start address equals the end of space, we are done
	sub r56, r52, r49
	bgte .unloadfinish

	ldr r56, [r51, #-2]
	sub r60, r56, #0
	bne .unloadloop

.unloaddelframe:
    ;; We have a frame we want to clean out, fill it with endtable value
    str r49, [r51, #-4]
    str r49, [r51, #-3]
;; Sort
    mov r50, r51
    ;; If we are beyond the end of the table, we are done
.unloadsort:
    sub r60, r50, r63	; (current row - end of table)
    bgte .unloadsortend
    ;; Load row and previous row
    ldrd r56, [r50, #-2]
    ldrd r52, [r50, #-1]
    ldrd r58, [r50]
    ldrd r54, [r50, #+1]
    ;; if previous is less than next, swap
    sub r60, r58, r56
    bgt .unloadnoswap
    strd r56, [r50]
    strd r52, [r50, #+1]
    strd r58, [r50, #-2]
    strd r54, [r50, #-1]
.unloadnoswap:
    add r50, r50, #16
    b .unloadsort

.unloadsortend:
    ;; Finally, we need to check this entry again and clear its PLT entry
    ;; We load the top of the PLT and search for the start address of
    ;; the cache we are unloading
    ldr r56, [r63, #-1]
    str r49, [r63, #-1]

    ;; Calculate and write new PLT
    sub r56, r56, #4
    mov r57, %low(cacheman)
    movt r57, %high(cacheman)
    sub r57, r57, r56
    lsr r57, r57, #1
    lsl r57, r57, #8
    mov r58, 0xf8
    orr r57, r57, r58
    str r57, [r56]

.unloadnextentry:
	sub r51, r51, #16
	b .unloadloop

.unloadfinish:
	;; Branch to second iteration
	b .cachebegin

        .global countedcall
countedcall:
	;; Scan cachedata table for LR. We are called from the PLT.
	mov r48, %low(cachespacebot)
	movt r48, %high(cachespacebot)
	add r55, r48, #24
.countedcallscanloop:
	ldrd r50, [r55], #2
	sub r52, r51, lr
        bne .countedcallscanloop

	;; inc ref count
        add r50, r50, #1
        str r50, [r55, #-4]

        ;; call function
        ldr r50, [r55, #-6]
        str lr, [sp, #2]
        jalr r50

	;; Scan cachedata table for LR. We are called from the PLT.
	mov r48, %low(cachespacebot)
	movt r48, %high(cachespacebot)
	add r55, r48, #24

	ldr r53, [sp, #2]

.countedcallscanloopagain:
	ldrd r50, [r55], #2
	sub r52, r51, r53
        bne .countedcallscanloopagain

        sub r50, r50, #1
        str r50, [r55, #-4]

        ldr lr, [sp, #4]
        add sp, sp, #16
	rts
	.size  cacheman, .-cacheman



	;; Starting Cache Entry Table
	;; ???: Should this be elsewhere?
	.balign 8
	.section .cachedata,"awx"
	.4byte cachespacebot+320
	.4byte cachespacebot+320
	.4byte 0
	.4byte cachespacebot+320
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
