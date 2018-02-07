OutSC byte 0
OutData byte 0
InSC byte 0
InData byte 0

org 34
main mov #$FFBE,r2


mov #$03,OutSC
mov #$01,InSC
mov &InData,r10

mov #1,r14
mov #0,r15

BIS #8,r2
Loop 
RdLoop cmp #0,r14
jnz RdLoop
mov #1,r14

WrLoop cmp #0,r15
jnz WrLoop
mov r10,OutData
mov #1,r15

jmp Loop

OutISR mov OutSC,R15
mov #0,R15
reti

InISR mov InData,R10
mov #0,R14
mov r2,R9
reti


org $FFC0
word $005C
word $0066

end