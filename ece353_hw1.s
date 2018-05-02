; Filename:     ece353_hw1
; Author:       <Dan Bai> 
; Description:  

    export hw1
	export WS2812B_BYTES
	export UPDATE_LEDS
	import init_leds
	import rotate_leds
	import write_leds
;******************************************************************************** 
; SRAM
;******************************************************************************** 
    AREA    SRAM, READWRITE
		;; TODO -- allocate space for WS2812B_BYTES and for UPDATE_LEDS
WS2812B_BYTES SPACE 24
UPDATE_LEDS SPACE 4
    align
		
        
;******************************************************************************** 
; Constant Variables (FLASH) Segment
;******************************************************************************** 
    AREA    FLASH, CODE, READONLY
    align
  
;******************************************************************************** 
;******************************************************************************** 
hw1   PROC
	
	;; TODO -- Setup argument to pass to init_leds
	LDR R0, =(WS2812B_BYTES)
	LDR R1, =(UPDATE_LEDS)
	;; TODO -- call init_leds routine
	BL init_leds
	
	;; Sets up second and third parameters 
	;; Array Size and Write address
	MOV R1, #8
	MOV32 R2, #0x400073FC
	;; Init necessary constants for evaluatin and update UPDATE_LEDS
	LDR R5, =(UPDATE_LEDS)
	MOV R4, #0
infinite_loop
	;; TODO -- do stuff specified in HW1 problem statement
	;; Check if UPDATE_LEDS is 0
	LDR R6, [R5]
	CMP R6, #0
	BEQ infinite_loop
	
	CPSID I
	;; Set UPDATE_LEDS to 0
	STR R4, [R5]
	
	;; Rotate LED values
	;; BL rotate_leds
	
	;; Saves the necessary registers and write rotated LED values
	PUSH {R0-R3}
	BL write_leds
	POP {R0-R3}
	
	CPSIE I

	;; B	infinite_loop
	
	BX LR
    ENDP
    align
        

    END
        
        
