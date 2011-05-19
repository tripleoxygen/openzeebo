
@00918870
@ROM:00918870 01 00 52 E3                 CMP     R2, #1
@ROM:00918874 01 00 A0 E3                 MOV     R0, #1
@ROM:00918878 00 02 81 15                 STRNE   R0, [R1,#0x200]

@org 0x931000
.code 32

cmp	r2, #1
mov	r0, #1
strne	r0, [r1, #0x200]

ldr	r0, =0x57
str	r0, [r1, #0x254]
ldr	r0, =0x3
str	r0, [r1, #0x26c]

ldr	r0, =0x0091887c
mov	pc, r0
