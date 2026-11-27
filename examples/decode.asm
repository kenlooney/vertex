mov al, 1
mov cl, 2
nop
start:
    jmp near forward
    nop
forward:
    jmp start
    jmp near start
