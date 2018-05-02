; Filename:     leds_write.s
; Author:       <Dan Bai> 
; Description:  

	;; TODO -- do I need something here so ece353_hw1 can see write_leds?
	export write_leds
	import init_leds
;******************************************************************************** 
; Constant Variables (FLASH) Segment
;******************************************************************************** 
    AREA    FLASH, CODE, READONLY
    align
		
;****************************************
; Rename registers for coad readability *
;****************************************
LED_ARRAY_ADDR 	RN R0	; passed in as argument
NUM_LEDS		RN R1	; passed in as argument
GPIO_ADDR		RN R2	; passed in as argument
NUM_BYTES		RN R3	; count down counter for what byte of LED data we are accessing
LED_DATA		RN R4	; stores LED color data
LOGIC_HIGH		RN R5	; used to store a 0x80 for writing logic high to GPIO
LOGIC_LOW		RN R6	; used to store a 0x00 for wrting logic low to GPIO
DELAY_INDX		RN R7	; used as index in delay loops
BIT_INDX		RN R8	; used as bit index for 24-bit loop

;******************************************************************************** 
;********************************************************************************        
write_leds   PROC
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; LED_ARRAY_ADDR will be passed value of led_array_addr ;;
	;; NUM_LEDS will be passed array size (number of LEDs)   ;;
	;; GPIO_ADDR will be passed the address of the GPIO port ;;
	;; the LEDs are connected to                             ;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; TODO -- save off registers to make routine EABI compliant
	PUSH {R4-R8}
	MOV		LOGIC_HIGH, #0x80			; used to write a 1 to port pin
	MOV		LOGIC_LOW, #0x00			; used to write a 0 to port pin	
	ADD		NUM_BYTES, NUM_LEDS, NUM_LEDS, LSL #1	; effectively 3*NUM_LEDs
	ADD		NUM_BYTES, NUM_BYTES, #1	; Need to start NUM_BYTES at NUM_LEDS*3 + 1 since subtract right away

write_loop								; for number of NeoPixels to write
    SUBS	NUM_BYTES, NUM_BYTES, #1	; subtract 1 from NUM_BYTES
	BEQ		done_write					; Is NUMBYTES zero? if so we are done
	LDRB	LED_DATA, [LED_ARRAY_ADDR], #1	; read 8-bit LED data and increment pointer to next color/LED
	MOV		BIT_INDX, #8				; index on bit loop for 24 bits in a LED
	
bit_loop
	STRB	LOGIC_HIGH, [GPIO_ADDR]		; all NeoPixel bits begin with logic high write to GPIO
	TST		LED_DATA, #0x00000080		; is MSB of LED_DATA set
	MOV		LED_DATA, LED_DATA, LSL #1	; shift register left to get to next bit
	BEQ		write_zero					; if MSB of data was not set we write logic zero to serial out
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Below is write a "1" to NeoPixel ;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	MOV		DELAY_INDX, #8			; currently at 4 clock cycles of high
high_delay1							; loop below takes (N-1)*4+2 clock cycles.  Using N of 7, to get 30 cyles of delay
	SUBS	DELAY_INDX, DELAY_INDX, #1
	BNE		high_delay1

    ;; TODO -- add 1 or more NOPs to make timing proper
	;; 40 clock cycles of high to write a 1 bit
	;; 30 cycles of delay + 4 cycles before + 2 cycles to write low
	;; Need 4 extra clock cycles
	NOP
	NOP
	NOP
	NOP
	
	STRB	LOGIC_LOW,	[GPIO_ADDR]		; clear GPIO pin as part of a write of "1" to LED
	MOV		DELAY_INDX, #3				; (3-1)*4+2 = 10 clock cycles delay in loop below
low_delay1
	SUBS	DELAY_INDX, DELAY_INDX, #1
	BNE		low_delay1
	
    ;; TODO -- add 1 or more NOPs to make timing proper
	;; 22.5 clock cycles of low to write a 1 bit
	;; 1 mov + 10 cycles of delay + 1 sub + 3 branch + 7 cycle until write high
	;; Need 1 extra clock cycle
	NOP
	
	SUBS	BIT_INDX, BIT_INDX, #1		; decrement bit index
	BEQ		write_loop					; done with this 8-bits of LED data, move on to next 8-bits of LED data
	
    ;; TODO -- add 1 or more NOPs to make timing proper							; need 12 instructions overhead to acheive 22
	;; 1 mov + 10 cycles delay + 1 NOP + 1 sub + 1 branch + 3 branch + 2 cycle until write high
	;; Need 3 extra clock cycles
	NOP
	NOP
	NOP
    B		bit_loop
	
write_zero
	MOV		DELAY_INDX, #3			; (3-1)*4+2 = 10 clock cycles delay, at 6 by end of this instr
high_delay0
	SUBS	DELAY_INDX, DELAY_INDX, #1
	BNE		high_delay0
	
    ;; TODO -- add 1 or more NOPs to make timing proper
	;; 20 clock cycles of high to write a 0 bit
	;; 6 clock + 10 cycles of delay + 2 until write low
	;; Need 2 extra clock cycles
	NOP
	NOP
	
	STRB	LOGIC_LOW,	[GPIO_ADDR]	; clear GPIO pin
	MOV		DELAY_INDX, #8			; (8-1)*4+2 = 30 clock cycles
low_delay0
	SUBS	DELAY_INDX, DELAY_INDX, #1
	BNE		low_delay0
	
    ;; TODO -- add 1 or more NOPs to make timing proper
	;; 42.5 clock cycles of low to write a 0 bit
	;; 1 mov + 30 cycles of delay + 1 sub + 3 branch + 7 cycles until write high
	;; Need 1 extra clock cycle
	NOP
	
	SUBS	BIT_INDX, BIT_INDX, #1	; decrement bit index
	BEQ		write_loop				; done with this 24-bits of LED data, move on to next 24-bits of LED data
	
    ;; TODO -- add 1 or more NOPs to make timing proper
	;; 1 mov + 30 cycles of delay + 1 NOP + 1 sub + 1 branch + 3 branch + 2 cycles until write high
	;; Need 3 extra clock cycles
	NOP
	NOP
	NOP
    B		bit_loop
	
done_write
	;; TODO -- restore registers
	POP {R4-R8}
	;; TODO -- return from function
	BX LR
    ENDP
    align        
    
    END



