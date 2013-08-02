;;;	.extern _mailbox
	.set    _mailbox, 0x8f000000
	.global	_mpf_isr
	.set    _write_failed, 3
	.balign 4
_mpf_isr:
	strd  r0, [sp, #-1]
	movfs r0, status
	movfs r1, iret
	add   sp, sp, #-40
	gie
	strd  r0, [sp, #3]
	str   fp, [sp, #5]
	mov   fp, sp
	str   r0, [fp, #4]
	mov   r0, %low(_mailbox)
	movt  r0, %high(_mailbox)
;;;	ldr   r0, [r0]
	mov   r1, _write_failed
	str   r1, [r0]
	nop
	mov   sp, fp
	ldrd  r0, [sp, #3]
	ldr   fp, [sp, #5]
	gid
	add   sp, sp, #40
	movts status, r0
	movts iret,   r1
	ldrd  r0, [sp, #-1]
	rti
	.size	_mpf_isr, .-_mpf_isr

