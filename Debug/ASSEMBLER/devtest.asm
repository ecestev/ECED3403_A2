;
; Device registers
;

OutSC	byte	0			; Dev 0 status and control reg
OutData	byte	0			; Dev 0 data reg (output)
InSC	byte	0			; Dev 1 status and control reg
InData	byte	0			; Dev 1 data reg (input)

COFF	equ	$10	        	; Turn CPU off until interrupt
INIA	equ	$03	        	; Input and IE (H/W should set input bit)
OUTIA	equ	$01	      	  	; Output and IE (H/W should clear output bit)
FALSE	equ	0          	 	; Boolean (signal device status)
TRUE	equ	1          	 	; Boolean (signal device status)

; -------------------------------------
; Mainline
; -------------------------------------

org	$1000

; -------------------------------------
; Initialize device registers and flags
; -------------------------------------

mov.b	#OUTIA,OutSC		; Dev 0: output and interrupts enabled
mov.b	#INIA,InSC			; Dev 1: input and interrupts enabled
mov.b	&InData,R10			; Read Dev 1 data register to clear pending intr
mov.b	#TRUE,R14			; R14 - no char available 
mov.b	#FALSE,R15			; R15 - output buffer available
	
; -------------------------------------
; Turn on CPU interrupts and wait
; -------------------------------------

BIS #8,R2 					; SR.GIE = 1 - Interrupts enabled
Loop 	bis		#COFF,R2    ; SR.COFF = 1 - CPU Asleep
; Wait input
RdLoop 	cmp.b	#FALSE,R14	; Data available?
jnz		RdLoop  			; No input data yet
		mov.b	#TRUE,R14	; Clear flag (R15)
; Char available
WrLoop 	cmp.b	#FALSE,R15	; Output buffer available?
jnz		WrLoop  			; Buffer not available
		mov.b	R10,OutData	; Write data to output buffer
		mov.b	#TRUE,R15	; Clear flag (R15)
; Next char
jmp	Loop
	
END
	