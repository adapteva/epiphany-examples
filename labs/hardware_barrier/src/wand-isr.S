#define WAND_MASK    #0xfffffff7

.type _wand_isr,%function
.global _wand_isr
.align 3
_wand_isr:
	str	r0,[sp,#-1]
	str	r1,[sp,#-2]
	movfs	r0,status
	mov	r1,%low(WAND_MASK)
	movt	r1,%high(WAND_MASK)
	and	r0,r0,r1
	movts	status,r0
	ldr	r0,[sp,#-1]
	ldr	r1,[sp,#-2]
	rti
.size _wand_isr, .-_wand_isr
