;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Project:		Interfacing PICs 
;	Source File Name:	VINTEST.ASM		
;	Status:			Final version
; zainab shaabneh 1182820
; baraa hammad 1191495
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 	Demonstrates simple analogue input
;	using an external reference voltage of 2.56V
;	The 8-bit result is converted to BCD for display
;	as a voltage using the standard LCD routines.
;	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	PROCESSOR 16F877A
;	Clock = XT 4MHz, standard fuse settings
	__CONFIG 0x3731

;	LABEL EQUATES	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	#INCLUDE "P16F877A.INC" 	; standard labels 
;==================================================


cblock 0x25
RESULT_PART2
RESULT_PART1
RESULT_1
RESULT_2
value_recivemost
value_recivelest
multiplier 
COUNT
d1
temp_result
d2
d3
d11	;tens of first num
d12	;ones of first num
d21  ;tens of second num
d22 ; ones of second num
counter
num1
num2
num 
temp_high   
temp_low     
x
dgt
x2
digit_pos
op_num
RB0_count
value_one
thous
huns
tens
ones
 numresult
 clkNum
endc
 
;============================================= PROGRAM BEGINS ============================================

	ORG	0		; Default start address 
	GOTO start 
 
;==========================================Inerupt service routine========================================
ISR
	ORG 004
	BCF INTCON,7 ;clear GIE to deny other interupt
	BCF	Select,RS	; set display command mode
    
   
 ;=================================== first number tenth part ===========================
	CALL	inid		; Initialise the display

	MOVLW	080	; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
    call printNum1
  
	BSF	Select,RS	; and restore data mode
	MOVLW	0C0		; code to home cursor at first position
	CALL	send	; output it to display
 
    MOVLW 1
	SUBWF digit_pos,W
	BTFSC STATUS,2 ; check if digit_pos is 2
	GOTO  tens1
  
 

;=========================================first number ones part ==========================
   	MOVLW 2
	SUBWF digit_pos,W
	BTFSC STATUS,2 ; check if digit_pos is 1
	GOTO  ones1
;===========================================operation pos ===============================
	;check if we at operation pos	
	MOVLW 3
	SUBWF digit_pos,W
	BTFSC STATUS,2 ; check if digit_pos is 3
	MOVLW	0C2		; code to home cursor at first position
	CALL	send	; output it to display
	BSF	Select,RS	; and restore data mode
	GOTO  printX
	CLRF	PORTD		; Clear display outputs
    MOVLW 1
	MOVWF digit_pos
    MOVLW	080		; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
	call printNum2

;====================================tenth part of first number =======================================
 
;====================================tenth part of first number =======================================
 
	tens1
    	MOVLW	0C0		; code to home cursor at first position
		CALL	send	; output it to display
		BSF	Select,RS	; and restore data mode
		BTFSS INTCON,1  ; check if interrupt is from RB0 or Timer0 
		GOTO TM0_INT	; handle interrupt Timer 0
	; handle interrupt RB0
	RBO_INTR
		INCF d11
		MOVLW 10
		SUBWF d11,W
		BTFSC STATUS,2 ;  
		GOTO zero_tenthous
		GOTO dis_RB0
	zero_tenthous
		CLRF d11
		GOTO dis_RB0
	TM0_INT
		INCF counter
		MOVLW d'40' 
		SUBWF counter,W
		BTFSC STATUS,2 ;check if counter value is 20
		GOTO dis_TM0
		GOTO cont
	dis_TM0
		;here timer is arrive 2 second
		CLRF counter 
		INCF digit_pos
		MOVF d11,w
		MOVWF dgt
		call MUL_BY_10
		MOVF x2,w
		ADDWF num1,f
		CLRF x2
		GOTO dis_RB0
	dis_RB0
		MOVLW	030		; load ASCII offset
		ADDWF	d11,W
		CALL	send
		GOTO cont
 
;========================================ones part of first number ============================================
 
	ones1
        MOVLW	0C1		; code to home cursor
		CALL	send		; output it to display
		BSF	Select,RS	; and restore data mode
		BTFSS INTCON,1 ; check if interrupt is from RB0 or Timer0 
		GOTO TM0_INT1 
	RBO_INTR1
		INCF d12
		MOVLW 10
		SUBWF d11,W
		BTFSC STATUS,2
		GOTO rolling_9_0
		MOVLW d'10'		
		SUBWF d12,W
		BTFSC STATUS,2 
		GOTO zero
		GOTO dis_RB01
	rolling_9_0
		MOVLW 10
		SUBWF d12,W		; here means that ones is 10
		GOTO zero
		GOTO dis_RB01
	zero
		CLRF d12
		GOTO dis_RB01
	TM0_INT1
		INCF counter
		MOVLW d'40'
		SUBWF counter,W
		BTFSC STATUS,2 ;check if counter value is 20;;;;;;;;;;;;;;;;;;;;;;;
		GOTO dis_TM01
		GOTO cont
	dis_TM01
		CLRF counter
		INCF digit_pos
		MOVF d12,w
		MOVWF dgt
		MOVF x2,w
		ADDWF num1,f
		CLRF x2
		GOTO dis_RB01
	dis_RB01
		MOVLW	030		; load ASCII offset
		ADDWF	d12,W
		CALL	send
		GOTO cont
 
 

 ;==================================== second number =======================================
   
    BCF Select, RS    ; set display command mode
    MOVLW 0xC0        ; move the cursor to the second line (address 0xC0)
    CALL send         ; output it to the display
;==========================tenth part=======================================
 	MOVLW 1
	SUBWF digit_pos,W
	BTFSC STATUS,2 ; check if digit_pos is 1
	GOTO  tens2
 ;==========================ones part=======================================
 	MOVLW 2
	SUBWF digit_pos,W
	BTFSC STATUS,2 ; check if digit_pos is 1
	GOTO  ones2
   
 
;====================================tenth part of second number=======================================
	tens2
    	MOVLW	0C0	; code to home cursor at first position
		CALL	send	; output it to display
		BSF	Select,RS	; and restore data mode
		BTFSS INTCON,1  ; check if interrupt is from RB0 or Timer0 
		GOTO TM0_INT7	; handle interrupt Timer 0
	; handle interrupt RB0
	RBO_INTR7
		INCF d21
		MOVLW 10
		SUBWF d21,W
		BTFSC STATUS,2 ; check if tens is more than 9
		GOTO zero2
		GOTO dis_RB07
	zero2
		CLRF d21
		GOTO dis_RB07
	TM0_INT7
		INCF counter
		MOVLW d'40'  
		SUBWF counter,W
		BTFSC STATUS,2 ;check if counter value is 20
		GOTO dis_TM07
		GOTO cont
	dis_TM07
		;here timer is arrive 2 second
		CLRF counter 
		INCF digit_pos
		MOVF d21,w
		MOVWF dgt
		call MUL_BY_10
		MOVF x2,w
		ADDWF num2,f
		CLRF x2
		GOTO dis_RB07
	dis_RB07
		MOVLW	030		; load ASCII offset
		ADDWF	d21,W
		CALL	send
		GOTO cont
	 
;====================================ones part of second number=======================================
 
	ones2
        MOVLW	0C1	; code to home cursor
		CALL	send		; output it to display
		BSF	Select,RS	; and restore data mode
		BTFSS INTCON,1 ; check if interrupt is from RB0 or Timer0 
		GOTO TM0_INT8 
	RBO_INTR8
		INCF d22
		MOVLW 6
		SUBWF d21,W
		BTFSC STATUS,2
		GOTO rolling1_9_0
		MOVLW d'10'		
		SUBWF d22,W
		BTFSC STATUS,2 
		GOTO zero_2
		GOTO dis_RB08
	rolling1_9_0
		MOVLW 10
		SUBWF d22,W		; here means that tens is 10
		GOTO zero_2
		GOTO dis_RB08
	zero_2
		CLRF d22
		GOTO dis_RB08
	TM0_INT8
		INCF counter
		MOVLW d'40'
		SUBWF counter,W
		BTFSC STATUS,2 ;check if counter value is 20
		GOTO dis_TM08
		GOTO cont
	dis_TM08
		CLRF counter
		INCF digit_pos
		MOVF d22,w
		MOVWF dgt
		MOVF x2,w
		ADDWF num2,f
		CLRF x2
		GOTO dis_RB08
	dis_RB08
		MOVLW	030		; load ASCII offset
		ADDWF	d22,W
		CALL	send
		GOTO cont
 
here
		CLRF counter
		INCF clkNum, 1
		MOVLW 3
		SUBWF clkNum,W
		BTFSC STATUS,2  
		GOTO zero_clks
zero_clks
		CLRF clkNum
		return
dis_TM0_k
		;here timer is arrive 2 second
		CLRF counter
		MOVLW 0
		SUBWF clkNum,w
		BTFSC STATUS, 2
		GOTO displayNums
;====================================================================================================
displayNums
		MOVLW 0C0
		call send
		MOVLW 030
		addwf d11,w
		call send
		MOVLW 030
		addwf d12,w
		call send
        addwf d21,w
		call send
		MOVLW 030
		addwf d22,w
		call send

return 
;=================================================cont================================================


cont
	MOVLW d'61'
	MOVWF TMR0
	BCF INTCON,1
	BCF INTCON,2
	BSF INTCON,7
RETFIE
 
 
 

; MAIN LOOP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

start
	;initialize all digits to zero
	MOVLW 0
	MOVWF d11
	MOVWF d12
	MOVWF d21
	MOVWF d22
	MOVWF counter
    CLRF clkNum
	MOVLW 1
	MOVWF digit_pos
 
	
 
	;some config. for LCD that connected with portD
    DATA_OUT equ PORTC     ; PortC for data transfer
    DATA_IN equ PORTC      ; PortC for data transfer
	BANKSEL	TRISD		; Select bank 1
	CLRF	TRISD		; Display port is output 
	BANKSEL PORTD		; Select bank 0
	CLRF	PORTD		; Clear display outputs
    BSF TRISB, 0 		; Make RB0 as an input for the push button
    BCF TRISD, 1 		 
    BANKSEL PORTB		; Select bank 0
	CLRF	PORTB		; Clear display outputs
    CLRF	PORTD		; Clear display outputs
	CALL	inid		; Initialise the display
	CLRF	dgt
	CLRF	x2
	CLRF	tens
	CLRF	ones
	CLRF	num
	CLRF 	num1
	CLRF 	num2
	CLRF    numresult
    
;===============blinking===============
    call BlinkLoop
    call BlinkLoop
    call BlinkLoop

;configuration for interupt on RB0 and TIMR0
	banksel OPTION_REG
	MOVLW B'11000111'
	MOVWF OPTION_REG
	BANKSEL TRISB
	MOVLW H'FF'
	MOVWF TRISB
	BANKSEL INTCON
	BSF INTCON,7 ; Globak=l interrupt enable
	BSF INTCON,4 ; RB0 interrupt enable
	BCF INTCON,1 ; RB0 interrrupt flag
	BSF INTCON,5 ;TIMR0 interrupt enable
	BCF INTCON,2 ;TIMR0 interrupt flag
	MOVLW d'61'
	MOVWF TMR0 ;TMR0 = 256 - (Fosc/4*F*PRE)-->Fosc=4MHZ, F = 1/(50ms),PRE = 256
;=======================================================================================
    start2
   
    call send_first_number_to_co_processor
    call send_unit_digit_of_second_number_to_co_processor
    call MultiplicationOp
    call receive_most_significant_digits_from_co_processor
    call receive_least_significant_digit_from_co_processor
    BCF	Select,RS	; set display command mode
	MOVLW	080		; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
   

   ; Add the least significant digits
    movf RESULT_PART1, W
    addwf value_recivelest, F ;least significant digits final result  
    ; Add the carry from the least significant digits
    btfss STATUS, C
    goto skip_carry_addition
    incf value_recivemost, F

skip_carry_addition:
    ; Add the most significant digits
    movf RESULT_PART2, W
    addwf value_recivemost, F ;  most significant final result 
    return



    BCF	 Select,RS	; set display command mode
	MOVLW	080		; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
    call print_result

    BCF Select, RS    ; set display command mode
    MOVLW 0xC0        ; move the cursor to the second line (address 0xC0)
    CALL send         ; output it to the display
    BSF Select, RS    ; and restore data mode
    call  Combine   
    call get_digits
   	call display
	GOTO CheckPushButton



CheckPushButton:
    BANKSEL PORTB         ; Select the bank containing the PORTB register
    BTFSS PORTB, 1       ; Check if RB0 (where push button P is connected) is high (not pressed)
    goto start
    goto CheckPushButton
BlinkLoop:
	CALL	inid		; Initialise the display
	BCF	Select,RS	; set display command mode
	MOVLW	080		; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
	CALL	putLCD		; display input

    BCF Select, RS    ; set display command mode
    MOVLW 0xC0        ; move the cursor to the second line (address 0xC0)
    CALL send         ; output it to the display
    BSF Select, RS    ; and restore data mode
    CALL printlcd       ; display input on the second line
    
;   Delay for 1 second (adjust as needed)
   MOVLW 0x64           ; Load W with delay value (100 in decimal)
   CALL d_1sec           ; Call a delay subroutine
   CLRF 	PORTD		; Clear display outputs
    

   return 
 
 ;=====================================
 

;===========================================  dealy 1 sec  ==================================



d_1sec
	movlw d'4'
	movwf d3
	loop3d
	movlw d'200'
	movwf d2
	loop2d
	movlw d'250'
	movwf d1
	loopd
	nop
	nop
	decfsz d1,f
	goto loopd 
	decfsz d2,f
	goto loop2d
	decfsz d3,f
	GOTO loop3d
return 

;===================Multiplication ===========================
 

MultiplicationOp
	CLRF	RESULT_PART2
	CLRF	RESULT_PART1
	MOVLW	D'8'
	MOVWF	COUNT
	MOVF	num1 , W
	BCF		STATUS, C
    call    MultiplicationOp_LOOP

MultiplicationOp_LOOP
	RRF		d22, F
	BTFSC	STATUS, C
	ADDWF	RESULT_PART2, F
	RRF		RESULT_PART2, F
	RRF		RESULT_PART1, F
	DECFSZ	COUNT, F  
	GOTO	MultiplicationOp_LOOP
	RETURN
;=================================================================
; Subroutine to wait for the co-processor to set the data ready flag
wait_for_co_processor_data
    btfss DATA_IN, 3   ; Check if the data ready flag is set by co-processor
    goto wait_for_co_processor_data
    return
;============================================================================
; Subroutine to receive most significant digits from co-processor
receive_most_significant_digits_from_co_processor
    ; Wait for the co-processor to send the most significant digits
    call wait_for_co_processor_data
    movf DATA_IN, W
    movf value_recivemost, W
    ; Process and display most significant digits as needed
    return
;==========================================================================================
; Subroutine to receive the least significant digit from the co-processor
receive_least_significant_digit_from_co_processor
    ; Wait for the co-processor to set the data ready flag
    call wait_for_co_processor_data

    ; Read the least significant digit from the co-processor
    movf DATA_IN, W
    movwf value_recivelest  ; Store the least significant digit in the result

    ; Clear the data ready flag in the co-processor
    bcf DATA_IN, 3

    return


;=======================================final result====================================================


;=================================================================
; Subroutine to send the first number to the co-processor
send_first_number_to_co_processor
    ; Combine the tenth and unit parts into a single byte
    movf num1, W
    movwf DATA_OUT

    ; Set the data ready flag to notify the co-processor
    bsf DATA_OUT, 3
    bcf DATA_OUT, 3   ; Clear the data ready flag
    return
;==================================================================
; Subroutine to send the unit digit of the second number to the co-processor
send_unit_digit_of_second_number_to_co_processor
    movf d22, W     ; Move the unit digit of the second number to W
    movwf DATA_OUT  ; Store W in DATA_OUT for sending to co-processor

    ; Set the data ready flag to notify the co-processor
    bsf DATA_OUT, 3
    bcf DATA_OUT, 3   ; Clear the data ready flag
    return
;====================================================================

;===========================================printNum1=============================================
printNum1
	MOVLW	'N'		; load volts code
	CALL	send		; and output
	MOVLW	'U'		; load volts code
	CALL	send		; and output
	MOVLW	'M'		; load volts code
	CALL	send		; and output
	MOVLW	'B'		; load volts code
	CALL	send		; and output
	MOVLW	'E'		; load volts code
	CALL	send		; and output
	MOVLW	'R'		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	'1'		; load volts code
	CALL	send		; and output
	RETURN			; done
;============================================printNum2============================================
printNum2
	MOVLW	'N'		; load volts code
	CALL	send		; and output
	MOVLW	'U'		; load volts code
	CALL	send		; and output
	MOVLW	'M'		; load volts code
	CALL	send		; and output
	MOVLW	'B'		; load volts code
	CALL	send		; and output
	MOVLW	'E'		; load volts code
	CALL	send		; and output
	MOVLW	'R'		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	'2'		; load volts code
	CALL	send		; and output
	RETURN			; done
 ;============================================printX============================================
printX
	MOVLW	'X'		; load volts code
	CALL	send		; and output
	RETURN			; done
;==============================================printlcd =========================================
printlcd 
	MOVLW	'M'		; load volts code
	CALL	send		; and output
	MOVLW	'U'		; load volts code
	CALL	send		; and output
	MOVLW	'L'		; load volts code
	CALL	send		; and output
	MOVLW	'T'		; load volts code
	CALL	send		; and output
	MOVLW	'I'		; load volts code
	CALL	send		; and output
	MOVLW	'P'		; load volts code
	CALL	send		; and output
	MOVLW	'L'		; load volts code
	CALL	send		; and output
	MOVLW	'I'		; load volts code
	CALL	send		; and output
	MOVLW	'C'		; load volts code
	CALL	send		; and output
	MOVLW	'A'		; load volts code
	CALL	send		; and output
	MOVLW	'T'		; load volts code
	CALL	send		; and output
	MOVLW	'I'		; load volts code
	CALL	send		; and output
	MOVLW	'O'		; load volts code
	CALL	send		; and output
	MOVLW	'N'		; load volts code
	CALL	send		; and output

	RETURN			; done
;===============================================putLCD=========================================
putLCD
    
	MOVLW	'W'		; load volts code
	CALL	send		; and output
	MOVLW	'E'		; load volts code
	CALL	send		; and output
	MOVLW	'L'		; load volts code
	CALL	send		; and output
	MOVLW	'C'		; load volts code
	CALL	send		; and output
	MOVLW	'O'		; load volts code
	CALL	send		; and output
	MOVLW	'M'		; load volts code
	CALL	send		; and output
	MOVLW	'E'		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	'T'		; load volts code
	CALL	send		; and output
	MOVLW	'O'		; load volts code
	CALL	send		; and output

	RETURN			; done
;=============================================printsign=========================================== 
printsign
 MOVLW	' '		; load volts code
 CALL	send		; and output
 MOVLW	'='		; load volts code
 CALL	send		; and output
 RETURN			; done
;============================================print_result==========================================
print_result
	BCF	Select,RS	; and restore data mode
    MOVLW	080		; code to home cursor
    CALL    send		; output it to display
    BSF	Select,RS	; and restore data mode
    MOVLW	'R'		; load volts code
	CALL	send		; and output
	MOVLW	'E'		; load volts code
	CALL	send		; and output
	MOVLW	'S'		; load volts code
	CALL	send		; and output
	MOVLW	'U'		; load volts code
	CALL	send		; and output
	MOVLW	'L'		; load volts code
	CALL	send		; and output
	MOVLW	'T'		; load volts code
	CALL	send		; and output
    MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
	MOVLW	' '		; load volts code
	CALL	send		; and output
    
    RETURN			; done
    
 
;============================================MUL_BY_10============================================
MUL_BY_10
	MOVLW d'10'
	MOVWF x
	MOVF dgt,w
	loop3
		ADDWF x2,f 
		decfsz x,f
	GOTO loop3
RETURN			; done
 
;=============================================get all digits===========================================
 Combine:
 ; Combine least significant and most significant parts into a single variable num
movf value_recivemost, W     ; Move most significant byte to W register
movwf numresult           ; Store it in num

; Left shift num by 8 bits (make room for the least significant byte)
swapf numresult , W        ; Swap the upper and lower nibbles
movwf numresult         ; Store the result back in num

movf value_recivelest, W     ; Move least significant byte to W register
iorwf numresult , F        ; OR it with num to combine the two parts

 
return 


get_digits																																	 


; ======================================= Calculate hundredth digit=====================================
	BSF	STATUS,C	; repeat process for tens
	MOVLW	D'100'		; load 100
sub5
	SUBWF	numresult 		    ; and subtract from result
	INCF	thous		; count number of loops
	BTFSC	STATUS,C	; and check if done
	GOTO	sub5		; no, carry on
	ADDWF	numresult 		    ; yes, add 100 back on
	DECF	huns		; and correct loop count

;==================================== Calculate tens digit ========================================

	BSF	STATUS,C	; repeat process for tens
	MOVLW	D'9'		; load 10
sub6
	SUBWF	numresult 		; and subtract from result
	INCF	tens		; count number of loops
	BTFSC	STATUS,C	; and check if done
	GOTO	sub6		; no, carry on
	ADDWF   numresult 		; yes, add 100 back on
	DECF	tens		; and correct loop count
; ========================= Calculate oness digit ====================================================
	MOVF	numresult ,W		; load remainder
	MOVWF	ones		; and store as ones digit

	RETURN			; done
 
;===========================================displaynumbers=============================================
display
	BCF	Select,RS	; and restore data mode
	MOVLW	0C8		; code to home cursor
	CALL	send		; output it to display
	BSF	Select,RS	; and restore data mode
	MOVLW 030
	ADDWF thous
	ADDWF huns
	ADDWF tens
	ADDWF ones
	call send
	MOVF thous,W
	call send
	MOVF huns,W
	call send
	MOVF tens,W
	call send
	MOVF ones,W
	call send
RETURN
;========================================================================================

	#INCLUDE "LCDIS.INC"


finl

	END	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;