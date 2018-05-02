; Filename:     leds_init.s
; Author:       <Dan Bai> 
; Description:  
	export init_leds
;******************************************************************************** 
; SRAM
;******************************************************************************** 
    AREA    SRAM, READWRITE
    align
        
;******************************************************************************** 
; Constant Variables (FLASH) Segment
;******************************************************************************** 
    AREA    FLASH, CODE, READONLY
    align
		
;******************************************************************************** 
;******************************************************************************** 
	;; TODO -- Implement EABI compliant function init_leds
init_leds PROC
	PUSH {R4}
	;; Store LED values with 75%
	MOV R4, #0xBF
	STRB R4, [R0, #21]
	STRB R4, [R0, #23]
	
	;; Store LED values with 50%
	MOV R4, #0x7F
	STRB R4, [R0, #0]
	STRB R4, [R0, #4]
	STRB R4, [R0, #8]
	STRB R4, [R0, #9]
	STRB R4, [R0, #13]
	STRB R4, [R0, #17]
	
	;; Store LED values with 25%
	MOV R4, #0x3F
	STRB R4, [R0, #10]
	STRB R4, [R0, #14]
	STRB R4, [R0, #15]
	STRB R4, [R0, #18]
	STRB R4, [R0, #19]
	STRB R4, [R0, #20]
	
	;; Store LED values with 0%	
	MOV R4, #0x00
	STRB R4, [R0, #1]
	STRB R4, [R0, #2]
	STRB R4, [R0, #3]
	STRB R4, [R0, #5]
	STRB R4, [R0, #6]
	STRB R4, [R0, #7]
	STRB R4, [R0, #11]
	STRB R4, [R0, #12]
	STRB R4, [R0, #16]
	STRB R4, [R0, #22]
	
	;; Set UPDATE_LEDS to 0
	STR R4, [R1]
	POP {R4}
	
	BX LR
    ENDP
	align
	END