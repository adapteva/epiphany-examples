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

/* Reserved registers. DO NOT CLOBBER THEM*/

#define ORIG_FN r46
#define ORIG_FN_SIZE r47

#define CACHE_SPACE_LOW r63
#define CACHE_SPACE_HIGH r49

#define CACHE_TABLE_LOW r48
#define CACHE_TABLE_HIGH r63

/* r48: CACHE_TABLE_LOW                    ^
                                           |
                                           | internal table
                                           |
                                           V
   r63: CACHE_TABLE_HIGH - CACHE_SPACE_LOW ^
                                           |
                                           | space for functions
                                           |
                                           V
   r49: CACHE_SPACE_HIGH
 */

#define ITER r62

#define CACHE_TABLE_SIZE 20
#define CACHE_TABLE_ENTRY_WIDTH 16

	.macro mov_label dest, label
	mov \dest, %low(\label)
	movt \dest, %high(\label)
        .endm

	.macro get_csp stack, base
	mov \base, %low(__cache_stack)
	movt \base, %high(__cache_stack)
        ldr \stack, [\base]
        .endm

        .macro gen_cachedata n
	.4byte cachespacetop
	.4byte cachespacetop
	.4byte 0
	.4byte cachespacetop
        .if \n > 1
        gen_cachedata \n - 1
        .endif
        .endm

	.section .cacheman,"ax",@progbits
	.balign 4
	.global cacheman
	.type cacheman, @function
cacheman:
	mov r17, %low(__cache_stack)
	movt r17, %high(__cache_stack)
        ldr r16, [r17]

        ldr r50, [sp, #2]
        add sp, sp, #8

        str r50, [r16], #2
        str lr, [r16, #-1]
        str r16, [r17]

	;; ORIG_FN = address of function to copy
        ;; ORIG_FN_SIZE = size of function to copy
	ldrd ORIG_FN,[lr]

	;; Set the limits of the cache space and table.
        mov_label CACHE_TABLE_LOW, cachespacebot
	add CACHE_TABLE_HIGH, CACHE_TABLE_LOW, #(CACHE_TABLE_SIZE * CACHE_TABLE_ENTRY_WIDTH)
        mov_label CACHE_SPACE_HIGH, cachespacetop

	;; Iter counter
	mov ITER, #0

.cachebegin:

#define ROW_POINTER r58
#define ROW_1   r50
#define START_1 r50
#define END_1   r51

#define REF_1   r52
#define PLT_1   r53

#define ROW_2   r54
#define START_2 r54
#define END_2   r55

#define REF_2   r56
#define PLT_2   r57

	;; Current row pointer
	mov ROW_POINTER, CACHE_TABLE_LOW

	;; We want to check that we actually have a free table space
	;; This is done by checking that the end entry has a start address
	;; equal to CACHE_SPACE_HIGH.

        ;; Load the start address field of the last table entry.
	ldr r56, [CACHE_TABLE_HIGH, #-(CACHE_TABLE_ENTRY_WIDTH>>2)]
	sub r56, r56, CACHE_SPACE_HIGH
	bne .endoftable

        ;; [1] (see algorithm in the top comment)

	;;  We don't use all the data *THIS* iteration, but doing a double
	;;  load here means we don't have to do lots of little loads later

	;; currentrow = *row++
	ldrd ROW_2, [ROW_POINTER], #2
.checkloop:
;;; previousrow = currentrow
	mov START_1, START_2		; move element across
	mov END_1, END_2

;;; currentrow = *row++
	ldrd ROW_2, [ROW_POINTER], #2	; load next entry

;;; availsize = currentrow[start]-previousrow[end]
	sub r60, START_2, END_1
;;; if (availsize >= neededsize) goto copy
	sub r61, r60, ORIG_FN_SIZE
	bgte .copy

	; check if we are at end of table
;;; if (row < CACHESPACETOP+320) goto checkloop
	sub r60, ROW_POINTER, #(CACHE_TABLE_SIZE * CACHE_TABLE_ENTRY_WIDTH)
	sub r60, r60, CACHE_TABLE_LOW
	blt .checkloop

.endoftable:
	;; At this stage we are out of table or space
	;; If this is our second iteration through the system, we can't
	;; load any code so fail
	sub r56, r62, #1
	beq .cachefail
	;; Set itercount and jump back
	mov r62, #1
	b .unload

.copy:
	;; At this stage we have:
	;; END_1 = dest
	mov r56, END_1	 	; current destaddr
;;; endaddr = srcdir+size
	add r57, ORIG_FN, ORIG_FN_SIZE	; endaddr + 1
	mov r59, ORIG_FN	; current srcdir
.copyloop:
;;; if (current_srcdir >= endaddr) goto .copyloopend
	sub r60, r59, r57
	bgte .copyloopend	; finished copying
;;; *currentdestdir++ = *currentsrcdir++
;;; ???: Can't do double load because functions may not be 8 byte aligned
	ldr r60, [r59], #1
	str r60, [r56], #1
	b .copyloop
.copyloopend:
	;; Store loaded memory table entry and sort
	mov START_2, r51
	mov END_2, r56
;;; row = CACHE_TABLE_HIGH - CACHE_TABLE_ENTRY_WIDTH
	sub ROW_POINTER, CACHE_TABLE_HIGH, #(CACHE_TABLE_ENTRY_WIDTH)		; row pointer for final entry
;;; *row = currentrow
	strd ROW_2, [ROW_POINTER]
;;; set ref count to 1
	mov REF_2, #1
;;; store plt entry
        mov PLT_2, lr
        strd REF_2, [r58, #+1]
.copysortloop:
;;; previousrow = *(row-1)
	ldrd ROW_1, [r58, #-2]
        ldrd REF_1, [r58, #-1]
;;; if currentrow[start] > previousrow[start] goto editplt
	sub r60, START_2, START_1
	bgte .editplt
;;; *row = previousrow; *(row-1) = currentrow
	strd ROW_1, [r58]
        strd REF_1, [r58, #+1]
	strd ROW_2, [r58, #-2]
        strd REF_2, [r58, #-1]
;;; *row--
	sub ROW_POINTER, ROW_POINTER, #(CACHE_TABLE_ENTRY_WIDTH)
;;; if (row != CACHESPACETOP) goto copysortloop
	sub r60, ROW_POINTER, CACHE_TABLE_LOW
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

        jalr r54

	;; Scan cachedata table for LR. We are called from the PLT.
        mov_label CACHE_TABLE_LOW, cachespacebot
	add r55, CACHE_TABLE_LOW, #(CACHE_TABLE_ENTRY_WIDTH + 8)

	mov r17, %low(__cache_stack)
	movt r17, %high(__cache_stack)
        ldr r16, [r17]

	ldr lr, [r16, #-2]
	ldr r53, [r16, #-1]
        sub r16, r16, #8
        str r16, [r17]

.scanloop:
	ldrd r50, [r55], #2
	sub r52, r51, r53
        bne .scanloop

        sub r50, r50, #1
        str r50, [r55, #-4]
	rts

.cachefail:
	ldr r50, [lr]
        jalr r50

	;; Scan cachedata table for LR. We are called from the PLT.
	mov r17, %low(__cache_stack)
	movt r17, %high(__cache_stack)
        ldr r16, [r17]

	ldr lr, [r16, #-2]
        sub r16, r16, #8
        str r16, [r17]
        rts
	;; If we are loading this, then we have failed to load the cache.
	;; Currently we trap and set R3 to 'PIC1'
	/* mov r3, 0x4331 */
	/* movt r3, 0x5049 */
	/* trap 3 */

;;; UNLOADING STARTS HERE
;;; Note: don't clobber r46 (addr), r47 (size), r48 (top), r49 (bot), r62/63
;;; (Using same register meanings as above)

.unload:
	add r51, CACHE_TABLE_LOW, #(CACHE_TABLE_ENTRY_WIDTH)	; First table entry is second row
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
    add r50, r50, #(CACHE_TABLE_ENTRY_WIDTH)
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
	mov r17, %low(__cache_stack)
	movt r17, %high(__cache_stack)
        ldr r16, [r17]

        ldr r50, [sp, #2]
        add sp, sp, #8

        str r50, [r16], #2
        str lr, [r16, #-1]
        str r16, [r17]

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
        jalr r50

	mov r17, %low(__cache_stack)
	movt r17, %high(__cache_stack)
        ldr r16, [r17]

	;; Scan cachedata table for LR. We are called from the PLT.
        mov_label CACHE_TABLE_LOW, cachespacebot
	add r55, CACHE_TABLE_LOW, #(CACHE_TABLE_ENTRY_WIDTH + 8)

	ldr lr, [r16, #-2]
	ldr r53, [r16, #-1]
        sub r16, r16, #8
        str r16, [r17]

.countedcallscanloopagain:
	ldrd r50, [r55], #2
	sub r52, r51, r53
        bne .countedcallscanloopagain

        sub r50, r50, #1
        str r50, [r55, #-4]
	rts
	.size  cacheman, .-cacheman

	.balign 8
	.section .csp,"awx"
        .4byte __cache_stack + 4

	;; Starting Cache Entry Table
	;; ???: Should this be elsewhere?
	.balign 8
	.section .cachedata,"awx"
	.4byte cachespacebot+(CACHE_TABLE_SIZE * CACHE_TABLE_ENTRY_WIDTH)
	.4byte cachespacebot+(CACHE_TABLE_SIZE * CACHE_TABLE_ENTRY_WIDTH)
	.4byte 0
	.4byte cachespacebot+(CACHE_TABLE_SIZE * CACHE_TABLE_ENTRY_WIDTH)
        gen_cachedata CACHE_TABLE_SIZE - 1
