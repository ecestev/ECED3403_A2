; --------------------------------
;		STEPHEN SAMPSON
;	MSP-430 EMULATOR TEST FILE
;		ALL INSTRUCTIONS
; --------------------------------

; --------------------------------
; devices in low memory (0-32)
; --------------------------------

DEVREGS	equ	$0      ; Device register addresses
INTVECT	equ	$FFC0	; Interrupt addresses
COFF	equ	$10	    ; Turn CPU off until interrupt
INIA	equ	$03	    ; Input and IE (H/W should set input bit)
OUTIA	equ	$01	    ; Output and IE (H/W should clear output bit)

; --------------------------------
; data in low memory (>32)
; --------------------------------

ORG $FFC
DATA WORD $1357
DATA2 WORD $1234

; --------------------------------
; set address of first instruction
; --------------------------------

ORG $1000
mov.b	#OUTIA,OutSC; Dev 0: output and interrupts enabled
mov.b	#INIA,InSC	; Dev 1: input and interrupts enabled

; --------------------------------
; one operand instructions
; --------------------------------

rrc		@R4+			
swpb	R5			
rra		R6			
sxt		R7				
push	R7
;call	R4			;					
;reti				 
					
; --------------------------------					
; two operand instructions
; --------------------------------

mov		#$1234,R8	
add		R6,R9				
addc	R6,R10						
subc	R6,R11						
sub		R6,R12							
cmp		R5,R12		
bit		R5,R12							
bic		R5,R13							
bis		R5,R14				
xor		R5,R15		
and	#$F0F0,&DATA		

; --------------------------------
; jump instructions
; --------------------------------
	
jmp 	DATA			; jump unconditional		NEW PC = 0x0FFC
jne		DATA			; jump if(!srptr->zero)		NEW PC = 0x0FFC
jeq		DATA			; jump if (srptr->zero)		NEW PC = OLD PC
jnc		DATA			; jump if (!srptr->carry)	NEW PC = OLD PC
jc		DATA			; jump if (srptr->carry)	NEW PC = 0x0FFC
jn		DATA			; jump if (srptr->negative)	NEW PC = OLD PC 
jge		DATA			; jump if (N .XOR. V) = 0	NEW PC = 0x0FFC
jl		DATA			; jump if (N .XOR. V) = 1	NEW PC = OLD PC

; --------------------------------
;		END OF TEST FILE
; --------------------------------

END
