//------- start -------------
//int hwloop(count);

.file "hwloop.s";
.section .text;
.type   hwloop, %function;
.global hwloop;
.extern A;   A global array A[] defined in one of the C modules
.extern B;
.extern result;

.balign 4;
hwloop:

	mov 	r2, 0x0		; change the cal type
	movt	r2, 0x8
	gid
	movfs	r3,  config
	orr	  r3,  r3, r2
	movts	config,  r3
	mov   r1,  %low(lstart)	;setting the hardware loop register
	movt  r1,  %high(lstart)	;
	movts ls,  r1;
	mov   r1,  %low(lend)	;
	movt  r1,  %high(lend)	;
	movts le,  r1;
	lsr   r0,  r0,  #1
	movts lc,  r0;

	mov	  r0,  %low(A)		;A[N] defined in emain.c
    movt  r0,  %high(A)
	mov	  r1,  %low(B)		;B[N] defined in emain.c
	movt  r1,  %high(B)
	mov	  r44, 0x0	;sum = 0
	
.balignw 8,0x000001a2;

lstart:	 
	ldr.l   r46, [r0], +0x1
	ldr.l   r48, [r1], +0x1
	
	ldr.l   r47, [r0], +0x1
	ldr.l   r49, [r1], +0x1
	fmul.l  r50,  r46,  r48
	fmul.l  r51,  r47,  r49
	
	
.balignw 8,0x000001a2;
	fadd.l  r44,  r50,  r44
lend:
	fadd.l	r44,  r51,  r44


	gie

	mov 	r0, r44		;save the result into r0

	rts; return


.size hwloop, .-hwloop;
//------- end -------------

