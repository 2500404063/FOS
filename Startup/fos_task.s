.cpu cortex-m4
.syntax unified
.thumb
.section .text

.global _FOS_SwitchTo
.extern cur_task
.extern next_task

_FOS_SwitchTo:
    cpsid I
    pop {r7}
    add sp, 0x4
    mrs r0, PSP
    cbz r0, _fos_task_jump
    stmdb r0!, {r4-r11}
    ldr r1, =cur_task
    ldr r1, [r1]
    str r0, [r1]
_fos_task_jump:
    ldr r0, =cur_task
    ldr r1, =next_task
    ldr r1, [r1]
    str r1, [r0]
    ldr r0, [r1]
    ldmia r0!, {r4-r11}
    msr PSP, r0
    mov lr, 0xFFFFFFFD
    cpsie I
    bx lr
