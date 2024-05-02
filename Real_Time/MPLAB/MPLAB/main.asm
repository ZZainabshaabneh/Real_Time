		PROCESSOR 16F877A
;	Clock = XT 4MHz, standard fuse settings
	__CONFIG 0x3731

;	LABEL EQUATES	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	#INCLUDE "P16F877A.INC" 	; standard labels 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	

	cblock 0x25

 
COUNT
value_one
d22
RESULT_HIGH  
RESULT_LOW  
	endc
;;;;;; PROGRAM BEGINS ;;;;;;;

	ORG	0		; Default start address 
	GOTO start 
;Inerupt service routine
ISR
	ORG 004
	BCF INTCON,7 ;clear GIE to deny other interupt
	BCF	Select,RS	; set display command mode
RETFIE
start
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MOVLW 0
	MOVWF value_one
	MOVWF d22
	MOVLW 1

	;some config. for Master and co-prosser
    DATA_OUT equ PORTC     ; PortC for data transfer
    DATA_IN equ PORTC      ; PortC for data transfer
 
	CLRF   COUNT
	CLRF	value_one
	CLRF	d22
	CLRF	RESULT_HIGH  
	CLRF    RESULT_LOW
 
main_loop
   

    ; Step 1: Receive the first number (tenth and unit parts) from the master CPU
    call receive_first_number_from_master

    ; Step 2: Receive the second number (ones part) and multiply
    call receive_second_number_from_master
   
    ; Step 3: multiplication 
    call MultiplicationOp

    ; Step 4: Send most_significant_digits back to the master CPU
    call send_most_significant_digits_to_master
    ; Step 5: send the least significant digit to the master CPU
    call send_least_significant_digit_to_master
    ; Optionally, introduce a delay or other logic between iterations
    ; For example, you might want to wait for a button press to restart the loop

    goto main_loop  ; Repeat the main loop
   


; Subroutine to wait for the master CPU to set the data ready flag
wait_for_master_data
    btfss DATA_IN, 3    ; Check if the data ready flag is set by master CPU
    goto wait_for_master_data
    return

;===================Multiplication ===========================
 

MultiplicationOp
	CLRF	RESULT_HIGH  
	CLRF	RESULT_LOW  
	MOVLW	D'8'
	MOVWF	COUNT
	
	MOVF	value_one , W
	BCF		STATUS, C

MultiplicationOp_LOOP
	RRF		d22, F
	BTFSC	STATUS, C
	ADDWF   RESULT_HIGH , F
	RRF		RESULT_LOW  , F
	RRF		RESULT_LOW  , F
	DECFSZ	COUNT, F  
	GOTO	MultiplicationOp_LOOP

	RETURN
;=================================================================
 


; Subroutine to receive the first number from the master CPU
receive_first_number_from_master
    ; Wait for the master CPU to set the data ready flag
    call wait_for_master_data

    ; Read the first number from the master CPU
    movf DATA_IN, W
    movwf value_one

    ; Clear the data ready flag
    bcf DATA_IN, 3

 return

; Subroutine to receive the second number from the master CPU
receive_second_number_from_master
    ; Wait for the master CPU to set the data ready flag
    call wait_for_master_data

    ; Read the second number from the master CPU
    movf DATA_IN, W
    movwf d22

    ; Clear the data ready flag
    bcf DATA_IN, 3

    return

;=================================================================
; Subroutine to send the most significant digits to the master CPU
send_most_significant_digits_to_master
   ; Send the most significant digit to the master CPU
   movf RESULT_HIGH , W
   
    movwf DATA_OUT
    bsf DATA_OUT, 3     ; Set the data ready flag
    bcf DATA_OUT, 3     ; Clear the data ready flag
    return
;=====================================================================
; Subroutine to send the least significant digit to the master CPU
send_least_significant_digit_to_master 
 ; Send the least significant digit to the master CPU
    movf RESULT_LOW, W
  
    movwf DATA_OUT
    bsf DATA_OUT, 3     ; Set the data ready flag
    bcf DATA_OUT, 3     ; Clear the data ready flag
    return
;========================================================================
 
 

	#INCLUDE "LCDIS.INC"

finl
	END	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
