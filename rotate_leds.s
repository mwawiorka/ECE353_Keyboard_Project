; Filename:     leds_rotate.s
; Author:       <Dan Bai> 
; Description:  

	export rotate_leds
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
; Passed in pointer to LED data on R0, and size of array on R1
; Will rotate led data with led_array_data[0] <-- led_array_data[3] and
; led_array_data[1] <-- led_array_data[4], ...  led_array[array_size-3] <-- 
; led_array_data[0] ... led_array[array_size-1] = led_array_data[2]
;********************************************************************************
rotate_leds   PROC
	;; Check if there are any LEDs to rotate
	;; If not then skip routine
	CMP R1, #0
	BLE SKIP
	
	;; Save registers used
	PUSH {R4-R10}
	
	;; Load the first 3 values to store later
	LDRB R4, [R0, #0]
	LDRB R5, [R0, #1]
	LDRB R6, [R0, #2]
	
	;; Sets the indexes for looping
	MOV R7, #0
	MOV R8, #3
	MOV R9, #0
	MUL R10, R1, R8
LOOP
	;; Condition for exit loop
	;; Done if reach the end of led value array
	CMP R8, R10
	BEQ DONE
	
	;; Load value and store into previous LED
	LDRB R9, [R0, R8]
	STRB R9, [R0, R7]
	
	;; Increment indexes
	;; Unconditionally branch to the start of loop
	ADD R7, R7, #1
	ADD R8, R8, #1
	B LOOP
DONE
	;; Use the previously loaded values for last LED 
	STRB R4, [R0, R7]
	ADD R7, R7, #1
	STRB R5, [R0, R7]
	ADD R7, R7, #1
	STRB R6, [R0, R7]
	
	;; Restores the save registers
	POP {R4-R10}
SKIP
	BX LR
	
	ENDP
	align
	END