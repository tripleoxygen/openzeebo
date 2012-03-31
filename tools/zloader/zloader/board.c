/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <boot/boot.h>
#include <boot/flash.h>
#include <msm7k/uart.h>

ptentry PTABLE[] = {
	{
		.start = 0,
		.length = 0xa,
		.name = "mibib",
	},
	{
		.start = 0xe6,
		.length = 0xa9,
		.name = "apps",
	},
};

void board_init()
{
	unsigned n;

	/* if we already have partitions from elsewhere,
	** don't use the hardcoded ones
	*/
	if(flash_get_ptn_count() == 0) {
		for(n = 0; n < 2; n++) {
			flash_add_ptn(PTABLE + n);
		}
	}

#ifdef DEBUG
	/* UART configuration */
	/* UART1 */
	uart_init(0);
#endif
}
