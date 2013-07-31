	.file	"hwb_isr.s"
	.comm	_state,4,4
	.section .text
	.balign 4
	.global	_wand_isr
_wand_isr:
	strd r0,[sp,#-1]
	movfs r0,status
	movfs r1,iret
	add sp,sp,#-184
	gie
	strd r0,[sp,#21]
	strd r2,[sp,#20]
	str fp,[sp,#39]
	str ip,[sp,#38]
	str lr,[sp,#37]
	strd r16,[sp,#17]
	strd r18,[sp,#16]
	strd r20,[sp,#15]
	strd r22,[sp,#14]
	strd r24,[sp,#13]
	strd r26,[sp,#12]
	strd r44,[sp,#11]
	strd r46,[sp,#10]
	strd r48,[sp,#9]
	strd r50,[sp,#8]
	strd r52,[sp,#7]
	strd r54,[sp,#6]
	strd r56,[sp,#5]
	strd r58,[sp,#4]
	strd r60,[sp,#3]
	str r62,[sp,#36]
	str r63,[sp,#5]
	mov fp,sp
	str r0,[fp,#5]
	mov r0,#1
	mov r1,#16
	mov r2, %low(_e_wait)
	movt r2, %high(_e_wait)
	jalr r2
	nop
	mov sp,fp
	ldrd r0,[sp,#21]
	ldrd r2,[sp,#20]
	ldr fp,[sp,#39]
	ldr ip,[sp,#38]
	ldr lr,[sp,#37]
	ldrd r16,[sp,#17]
	ldrd r18,[sp,#16]
	ldrd r20,[sp,#15]
	ldrd r22,[sp,#14]
	ldrd r24,[sp,#13]
	ldrd r26,[sp,#12]
	ldrd r44,[sp,#11]
	ldrd r46,[sp,#10]
	ldrd r48,[sp,#9]
	ldrd r50,[sp,#8]
	ldrd r52,[sp,#7]
	ldrd r54,[sp,#6]
	ldrd r56,[sp,#5]
	ldrd r58,[sp,#4]
	ldrd r60,[sp,#3]
	ldr r62,[sp,#36]
	ldr r63,[sp,#5]
	gid
	add sp,sp,#184
	movts status,r0
	movts iret,r1
	ldrd r0,[sp,#-1]
	rti
	.size	_wand_isr, .-_wand_isr
	.ident	"GCC: (Epiphany toolchain 20120120 (built 20130711)) 4.8.1 20130513 (prerelease)"

