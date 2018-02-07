zeroSC      byte 0
zeroDR      byte 0
oneSC      	byte 0
oneDR		byte 0
twoSC   	byte 0
twoDR   	byte 0
threeSC     byte 0
threeDR     byte 0

array equ $5000

org $100
main
mov #$FFBE,R1

mov #3,&zeroSC
mov #1,&oneSC
mov #1,&twoSC
mov #3,&threeSC

mov #$25,&oneDR
add #1,R5
mov #$50,&twoDR
add #1,R5

BIS #8,R2

Loop
add #1,R6
cmp #$0A,R6
jne Loop

final jmp final

org $200
zeroISR
mov &zeroDR,array(R4)
add #1,R4
reti

oneISR
mov &oneDR,array(R4)
add #1,R4
reti

twoISR
mov R5,&twoDR
add #1,R5
reti

threeISR
mov R5,&threeDR
add #1,R5
reti

org $FFC0

word 512;oneISR
word 524;twoISR
word 536;threeISR
word 546;fourISR

end