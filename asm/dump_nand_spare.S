
/* dump_nand_spare.S
 * Zeebo NAND to RAM dumper (with spare bytes)
 *
 * Copyright (C) 2010 Triple Oxygen
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
        .code 32
        .org 0
        .global _start
        
	.equ MEM_SIZE_DATA,	(64*1024*1024)
	.equ PAGE_COUNT,        (MEM_SIZE_DATA/2048)
        .equ MEM_SIZE,          (PAGE_COUNT*2112)
        
	.equ EL_PANEL_ADDR,     0xA9200800
        .equ BLOCK_NUM,         0   
        .equ SECTOR_PAGE,       4
	.equ SECTOR_SIZE,	528	@ 512 no spare / 528 w/ spare
        
_start:

        mov     r3, #0
        ldr     r6, =SECTOR_PAGE                @ sector per page (4)
        ldr     r4, =0x10000000			@ dest. addr.
        mov     r7, #0
        
        @ supervisor
        msr     cpsr_c, #(0x00000080 | 0x00000040 | 0x00000013)
	
        @ disable mmu
        mrc     p15, 0, r0, c1, c0, 0
        bic     r0, #1
        mcr     p15, 0, r0, c1, c0, 0
        nop
        nop
        
        @ disable mpu
        mov     r1, #0
        ldr     r0, =0xA0B00000
        str     r1, [r0]
        
        bl      panel_off        
        bl      clear_sdram
        
        @ setup nand controller
        ldr     r0, =0xA0A00000

	@ chipset
	mov	r1, #0x4	@ CS0 + undocumented bit
	str	r1, [r0, #0xC]

        @ config register 1
	ldr	r1, =0xA25400C0
	str	r1, [r0, #0x20]

        @ config register 2
	ldr	r1, =0x0004745E	@ =0x0004745f ?
	str	r1, [r0, #0x24]

        @ reset nand
        mov     r1, #0x31	@ 0x31 - Abort / Page
        str     r1, [r0]       
        mov     r1, #1
        str     r1, [r0, #0x10]

        mov     r1, #0xD	@ 0xD - Reset NAND device
        str     r1, [r0]       
        mov     r1, #1
        str     r1, [r0, #0x10]
        nop
        nop
        nop
        nop
        
        @ check ID
        mov     r1, #0xB	@ 0xB - Fetch ID
        str     r1, [r0]
        mov     r1, #1
        str     r1, [r0, #0x10]
        nop
        nop
        nop
        nop
        ldr     r1, [r0, #0x40]
        ldr     r2, =0x5580b1ad
        cmp     r1, r2
        bne     blink_and_die
            
        @ initial addr
        mov     r1, #0
        str     r1, [r0, #4]
        str     r1, [r0, #8]
        
        @ chipsel
        @mov     r1, #0                          @ disable DM
        @str     r1, [r0, #0xC]
        
        @ read page (no ECC!)        
        mov     r1, #0x33
        str     r1, [r0]
        
        adr     r1, START_PAGE
        ldr     r1, [r1]
        lsl     r1, #16
        str     r1, [r0, #4]
        
_execute:
        @ read sector
        mov     r1, #1
        str     r1, [r0, #0x10]

	bl delay

@_wait:
@        ldr     r1, [r0, #0x14]
@	and	r1, r1, #0x20
@       tst     r1, #0x20
@        beq     _wait

        
@        tst     r1, #((1 << 4) | (1 << 8))
@        bne     blink_and_die
        
        add     r2, r0, #0x100
        ldr     r5, =SECTOR_SIZE
        lsr     r5, #2		@ / 4 (1 DWORD)

	bl delay
        
_buf_loop:
        ldr     r1, [r2], #4
        str     r1, [r4], #4
        subs    r5, #1

        bne     _buf_loop
        
        subs    r6, #1
        bne     _execute
        
                                @ finished reading sectors, increment
                                @ page.

	bl delay
                                
        adr     r1, START_PAGE
        ldr     r2, [r1]
        add     r2, #1
        str     r2, [r1]
        lsl     r2, #16
        str     r2, [r0, #4]
        
        ldr     r6, =SECTOR_PAGE
        
        ldr     r1, =PAGE_COUNT
        add     r7, #1
        cmp     r7, r1
        blt     _execute
        
        bl      panel_on
        
_done:
        b       _done

@ ----------------------------------------------------------------------

clear_sdram:
        push    {r0-r2}
        ldr     r1, =0xFFFFFFFF
        ldr     r0, =0x10000000
        ldr     r2, =MEM_SIZE
	lsr	r2, #2
_loop:
        str     r1, [r0], #4
        subs    r2, #1
        bne     _loop
        pop     {r0-r2}
        mov     pc, lr

@ ----------------------------------------------------------------------

panel_on:
        push    {r0, r1}
	ldr     r0, =EL_PANEL_ADDR
	ldr     r1, [r0]
	bic     r1, #0x8000
	str     r1, [r0]
        pop     {r0, r1}
        mov     pc, lr

@ ----------------------------------------------------------------------

panel_off:
        push    {r0, r1}
	ldr     r0, =EL_PANEL_ADDR
	ldr     r1, [r0]
	orr     r1, #0x8000
	str     r1, [r0]
        pop     {r0, r1}
        mov     pc, lr

@ ----------------------------------------------------------------------
        
blink_and_die:        
	ldr     r0, =0xC0000
_delay:
	subs    r0, #1
	bne     _delay
	ldr     r0, =EL_PANEL_ADDR
	ldr     r1, [r0]
	eor     r1, #0x8000
	str     r1, [r0]
        b       blink_and_die

@ ----------------------------------------------------------------------

delay:
	push    {r0}
	ldr     r0, =0xF000
_d1:
	subs    r0, #1
	bne     _d1
        pop     {r0}
        mov     pc, lr

@ ----------------------------------------------------------------------

START_PAGE:
                .word 0x0       @ Page = 2048 bytes data + 64 bytes spare / 1024 words data + 32 words spare
