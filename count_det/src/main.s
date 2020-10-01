.text

.global count_det

count_det:

	push {r4-r6}

	mov r1, #0

	ldr r2, [r0, #0]
	ldr r3, [r0, #16]
	ldr r4, [r0, #32]
	ldr r5, [r0, #20]
	ldr r6, [r0, #28]

	mul r3, r3, r4
	mul r5, r5, r6
	sub r3, r3, r5
	mul r2, r2, r3
	add r1, r1, r2

	ldr r2, [r0, #4]
	ldr r3, [r0, #12]
	ldr r4, [r0, #32]
	ldr r5, [r0, #20]
	ldr r6, [r0, #24]

	mul r3, r3, r4
	mul r5, r5, r6
	sub r3, r3, r5
	mul r2, r2, r3
	sub r1, r1, r2

	ldr r2, [r0, #8]
	ldr r3, [r0, #12]
	ldr r4, [r0, #28]
	ldr r5, [r0, #16]
	ldr r6, [r0, #24]

	mul r3, r3, r4
	mul r5, r5, r6
	sub r3, r3, r5
	mul r2, r2, r3
	add r1, r1, r2

	mov r0 ,r1

	pop {r4-r6}

	bx lr
