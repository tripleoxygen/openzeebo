/* arch/arm/mach-msm/include/mach/msm_ssbi.h
 *
 * Copyright (C) 2010 Google, Inc.
 * Author: Dima Zavin <dima@android.com>
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

#ifndef __ASM_ARCH_MSM_SSBI_H
#define __ASM_ARCH_MSM_SSBI_H

#include <boot/boot.h>

int msm_ssbi_write(u16 addr, u8 *buf, int len);
int msm_ssbi_read(u16 addr, u8 *buf, int len);

#endif
