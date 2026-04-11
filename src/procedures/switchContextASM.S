/*
 * @file switchContextASM.S
 * @brief Low-level Context Switching for ATmega328P. 
 * It saves the complete CPU state of the current task and restores 
 * the state of the next task to be executed.
*/

.section .text
.global switchContextASM

//Register definitions of I/O for ATmega328P
.equ SREG, 0x3F
.equ SPL,  0x3D
.equ SPH,  0x3E

switchContextASM:
    // r24:r25 -> Address of current->m_sp
    // r22:r23 -> Address of next->m_sp

    //Save current context 
    push r0
    in   r0, SREG       //Save interrupts state and flags
    push r0
    push r1
    clr  r1             //CLear register 1 C++ requirement 
    
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

    //Save stack pointer in the process that is going out 
    // X (r26:r27) now is the address in m_sp
    movw r26, r24
    in   r0, SPL
    st   X+, r0
    in   r0, SPH
    st   X, r0

    // Load the new stack pointer 
    // X (r26:r27) now is the m_sp dof the next process
    movw r26, r22
    ld   r0, X+
    out  SPL, r0        //Load low byte to hardware
    ld   r0, X
    out  SPH, r0        //Load high byte to hardware

    //Load new context 
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
    out SREG, r0        
    pop r0

    ret                 //Jump to the address stores in the new stack

