; --------------------------------
;		STEPHEN SAMPSON
;	MSP-430 EMULATOR TEST FILE
;	  WRITE IMMEDIATE TEST
; --------------------------------

; --------------------------------
; devices in low memory (0-32)
; --------------------------------


; --------------------------------
; data in low memory (>32)
; --------------------------------


; --------------------------------
; set address of first instruction
; --------------------------------

ORG $1000

; --------------------------------
; one operand instructions
; --------------------------------
					
swpb	#$FF00			; Loc: 4098 should have 00FF after running				
					
; --------------------------------
;		END OF TEST FILE
; --------------------------------

END