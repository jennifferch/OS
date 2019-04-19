/*
 */
	.syntax unified

	.text

	.global PendSV_Handler

	.extern getNextContext

	.thumb_func

PendSV_Handler:

    tst lr, 0x10
    it eq
    vpusheq {s16-s31}

	push {r4-r11,lr}
	mrs r0,msp
	bl getNextContext
	msr msp,r0
	pop {r4-r11,lr}

	tst lr, 0x10
    it eq
    vpopeq {s16-s31}

    bx lr
