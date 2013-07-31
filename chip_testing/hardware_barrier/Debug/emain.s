	.file	"emain.c"
	.comm	_state,4,4
	.section .text
	.balign 4
	.global	_main
_main:
	str r4,[sp,#1]
	str fp,[sp],#-10
	str lr,[sp,#9]
	mov fp,sp
	mov r0,#0
	mov r1, %low(_e_irq_global_mask)
	movt r1, %high(_e_irq_global_mask)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r1
	mov r0,#8
	mov r1, %low(_wand_isr)
	movt r1, %high(_wand_isr)
	mov r2, %low(_e_irq_attach)
	movt r2, %high(_e_irq_attach)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r2
	mov r0,#8
	mov r1,#0
	mov r2, %low(_e_irq_mask)
	movt r2, %high(_e_irq_mask)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r2
	mov r0, %low(_e_group_config+28)
	movt r0, %high(_e_group_config+28)
	ldr r0,[r0]
	str r0,[fp,#6]
	mov r0, %low(_e_group_config+32)
	movt r0, %high(_e_group_config+32)
	ldr r0,[r0]
	str r0,[fp,#5]
	mov r0, %low(_e_group_config+24)
	movt r0, %high(_e_group_config+24)
	ldr r3,[r0]
	ldr r2,[fp,#6]
	mov ip, %low(#524288)
	movt ip, %high(#524288)
	movfs r0,config
	movfs r1,status
	gid
	movfs r16,config
	orr r16,r16,ip
	movts config,r16
	gie
	mov ip, %low(#-4337)
	movt ip, %high(#-4337)
	movfs r16,status
	eor r16,r16,r1
	and r16,r16,ip
	eor r16,r16,r1
	movts status,r16
	imul r2, r3, r2
	ldr r1,[fp,#5]
	add r1,r2,r1
	str r1,[fp,#4]
	ldr r1,[fp,#4]
	lsl r2,r1,#2
	mov r1, %low(#-1895825408)
	movt r1, %high(#-1895825408)
	add r1,r2,r1
	str r1,[fp,#3]
	ldr r1,[fp,#4]
	add r1,r1,#1
	lsl r1,r1,#16
	str r1,[fp,#2]
	ldr r1,[fp,#3]
	mov r2, %low(#-559038737)
	movt r2, %high(#-559038737)
	str r2,[r1]
	mov r1,#0
	str r1,[fp,#7]
	movfs r1,status
	gid
	movfs r2,config
	eor r2,r2,r0
	and r2,r2,r4
	eor r2,r2,r0
	movts config,r2
	gie
	mov ip, %low(#-4337)
	movt ip, %high(#-4337)
	movfs r16,status
	eor r16,r16,r1
	and r16,r16,ip
	eor r16,r16,r1
	movts status,r16
	b .L2
.L3:
	ldr r1,[fp,#7]
	ldr r0,[fp,#3]
	str r1,[r0]
	mov r0,#0
	ldr r1,[fp,#2]
	mov r2, %low(_e_wait)
	movt r2, %high(_e_wait)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r2
; 37 "src/emain.c" 1
	wand
; 0 "" 2
; 38 "src/emain.c" 1
	idle
; 0 "" 2
	mov r0, %low(#984068)
	movt r0, %high(#984068)
	mov r1, %low(_e_reg_read)
	movt r1, %high(_e_reg_read)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r1
	mov r1,r0
	mov r0, %low(_state)
	movt r0, %high(_state)
	str r1,[r0]
	mov r0, %low(_state)
	movt r0, %high(_state)
	ldr r1,[r0]
	mov r0, %low(#-9)
	movt r0, %high(#-9)
	and r1,r1,r0
	mov r0, %low(_state)
	movt r0, %high(_state)
	str r1,[r0]
	mov r0, %low(_state)
	movt r0, %high(_state)
	ldr r1,[r0]
	mov r0, %low(#984128)
	movt r0, %high(#984128)
	mov r2, %low(_e_reg_write)
	movt r2, %high(_e_reg_write)
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	jalr r2
	ldr r0,[fp,#7]
	add r0,r0,#1
	str r0,[fp,#7]
.L2:
	ldr r1,[fp,#7]
	mov r0,#65535
	sub r2,r1,r0
	blte .L3
	mov r0,#0
	mov r4, %low(#-917506)
	movt r4, %high(#-917506)
	mov sp,fp
	ldr fp,[sp,#10]
	ldr lr,[sp,#9]
	add sp,sp,#40
	ldr r4,[sp,#1]
	rts
	.size	_main, .-_main
	.ident	"GCC: (Epiphany toolchain 20120120 (built 20130711)) 4.8.1 20130513 (prerelease)"
