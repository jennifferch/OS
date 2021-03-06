/*
 */
	.syntax unified

	.text

	.global PendSV_Handler

	.extern getNextContext

	.thumb_func

PendSV_Handler:
    cpsid i
    /*FPU context*/
    tst lr, 0x10
    it eq
    vpusheq {s16-s31}

    /*Integer context*/
	push {r4-r11,lr}

	/*Get next context using current context as parameter (R0)*/
	mrs r0,msp
	bl getNextContext
	msr msp,r0
	/*Recover integer context*/
	pop {r4-r11,lr}

    /* Recover FPU context*/
	tst lr, 0x10
    it eq
    vpopeq {s16-s31}

    cpsie i

    bx lr
