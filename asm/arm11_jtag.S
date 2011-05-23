
/* arm11_jtag.S
 * Zeebo ARM11 Enabler
 *
 * Copyright (C) 2011 OpenZeebo
 * Copyright (C) 2011 Triple Oxygen
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
 
	@org 0x931000
	.code 32

	push 	{r0,r1}
	ldr	r1, =0xa9000000

	@ Pull-up on MODE pins
	ldr	r0, =0x57
	str	r0, [r1, #0x254]

	@ ARM11-only mode
	ldr	r0, =0b11
	str	r0, [r1, #0x26c]

	pop	{r0, r1}
	bx	lr

