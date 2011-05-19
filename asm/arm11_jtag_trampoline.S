
@00918870
@ROM:00918870 01 00 52 E3                 CMP     R2, #1
@ROM:00918874 01 00 A0 E3                 MOV     R0, #1
@ROM:00918878 00 02 81 15                 STRNE   R0, [R1,#0x200]

@org 0x931000
.code 32

ldr	r0, =0x00931000
mov	pc, r0
