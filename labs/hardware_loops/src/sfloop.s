//------- start -------------
//int sfloop(count);

.file "sfloop.s";
.section .text;
.type   _sfloop, %function;
.global _sfloop;
.extern _A;   A global array A[] defined in one of the C modules
.extern _B;
.extern _result;

.balign 4;
_sfloop:


	mov 	r2,  0x0		; change the cal type
	movt	r2,  0x8
	gid
	movfs	r3,  config
	orr	  r3,  r3, r2
	movts	config, r3
	gie

	lsr   r0,  r0, 0x1
	mov	  r45, r0

	mov	  r0,  %low(_A)  ;A[N] defined in emain.c
	movt  r0,  %high(_A)
	mov	  r1,  %low(_B)		;B[N] defined in emain.c
	movt  r1,  %high(_B)
	mov	  r44, 0x0		;sum = 0
	
loop_start:	
	ldr   r46, [r0], +0x1
	ldr   r48, [r1], +0x1
	ldr   r47, [r0], +0x1
	ldr   r49, [r1], +0x1
	fmul  r50,  r46,  r48
	fmul  r51,  r47,  r49
	fadd  r44,  r50,  r44
	fadd	r44,  r51,  r44
	sub	  r45,  r45, +0x1
	bne	  loop_start

	mov 	r0,   r44		;save the result into r0

 
	rts; return


.size _sfloop, .-_sfloop;
//------- end -------------

