.thumb 		@processor mode

.equ GPIOC_BASE, 0x48000800 @ (AHB2PERIPH_BASE + 0x00000800)
.equ RCC_BASE , 0x40021000 @(AHBPERIPH_BASE + 0x00001000)
.equ GPIO_HIGH_OFFSET, 0x18 @ < GPIO port bit set/reset  register,     Address offset: 0x18      */
.equ GPIO_LOW_OFFSET, 0x28
.equ ECC_AHB2ENR_OFFSET, 0x4c @ RCC AHB2 peripheral clocks enable register, Address offset: 0x4C */

.global main 	@pointer for compiller
.thumb_func 	@pointer for compiller


			@main function
main:
			@ configure clock
	LDR R0, =(RCC_BASE + ECC_AHB2ENR_OFFSET)
    	LDR R1, =(1 << 2)
    	STR R1, [R0]
			@Enable GPIOC pin 6 as output
    	LDR R0, =GPIOC_BASE
    	LDR R1, =(1 << (6*2))
    	STR R1, [R0]
			@blink function
blink:
			@set led to high
	LDR R0, =GPIOC_BASE
    	LDR R1, =(1 << (3*2))
    	STR R1, [R0, #GPIO_HIGH_OFFSET]

   			@jump with back to delay
	BL delay

   			@set led to low
    	LDR R1, =(1 << (3*2))
    	STR R1, [r0, #GPIO_LOW_OFFSET]

   			@jump with back to delay
	BL delay
			@jump to blink
   	B blink
			@delay function
delay:
	LDR r2, =666666 @load register - value from memory to register
	clock:
			@r2-r2
		SUB r2, r2, #1
   	BNE clock 	@if not 0 jump to clock branch not equal (coonditional) 
   	BX LR		@back to entry point branch and exchabge (jump)