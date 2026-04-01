.global contextSwitchASM
.type   contextSwitchASM, @function 

;current process is r24:r25 and next is r22;r23 

contextSwitchASM:
   ;Save context of current process
    push r0                                     
    in   r0, __SREG__                           
    cli                                         
    push r0                                     

    push r1
    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r18
    push r19
    push r20
    push r21
    push r22
    push r23
    push r24
    push r25
    push r26
    push r27
    push r28
    push r29
    push r30
    push r31

   ;Save stack pointer in current->m_sp;
    mov r26, r24
    mov r27, r25 
   
    in r0, __SP_L__
    st X+, r0 

    in r0, __SP_H__
    st X, r0 

   ;load stack pointer next->m_sp;
    mov r26, r22
    mov r27, r23

    ld r28, X+
    ld r29, X 

    out __SP_L__, r28 
    out __SP_H__, r29 

   ;Reload new context 
    pop r31
    pop r30
    pop r29
    pop r28
    pop r27
    pop r26
    pop r25
    pop r24
    pop r23
    pop r22
    pop r21
    pop r20
    pop r19
    pop r18
    pop r17
    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2
    pop r1

    pop r0
    out __SREG__, r0
    pop r0

   ret
