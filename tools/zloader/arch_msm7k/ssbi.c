/* arch/arm/mach-msm/ssbi.c
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2010, Google Inc.
 *
 * Original authors: Code Aurura Forum
 *
 * Author: Dima Zavin <dima@android.com>
 *  - Largely rewritten from original to not be an i2c driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <msm7k/ssbi.h>

#ifdef ARM9
#define MSM_SSBI_PHYS		 0xb8000300
#else
#define MSM_SSBI_PHYS		 0xA8100000
#endif

/* SSBI 2.0 controller registers */
#define SSBI2_CTL		0x0000
#define SSBI2_RESET		0x0004
#define SSBI2_CMD		0x0008
#define SSBI2_RD		0x0010
#define SSBI2_STATUS		0x0014
#define SSBI2_PRIORITIES	0x0018
#define SSBI2_MODE2		0x001C

/* SSBI_CMD fields */
#define SSBI_CMD_SEND_TERM_SYM	(1 << 27)
#define SSBI_CMD_WAKEUP_SLAVE	(1 << 26)
#define SSBI_CMD_USE_ENABLE	(1 << 25)
#define SSBI_CMD_RDWRN		(1 << 24)

/* SSBI_STATUS fields */
#define SSBI_STATUS_DATA_IN		(1 << 4)
#define SSBI_STATUS_RD_CLOBBERED	(1 << 3)
#define SSBI_STATUS_RD_READY		(1 << 2)
#define SSBI_STATUS_READY		(1 << 1)
#define SSBI_STATUS_MCHN_BUSY		(1 << 0)

/* SSBI_RD fields */
#define SSBI_RD_USE_ENABLE	(1 << 25)
#define SSBI_RD_RDWRN		(1 << 24)

/* SSBI_MODE2 fields */
#define SSBI_MODE2_SSBI2_MODE	(1 << 0)

#define SSBI_TIMEOUT_US		100

static inline u32 ssbi_readl(u32 reg)
{
	return readl(MSM_SSBI_PHYS + reg);
}

static inline void ssbi_writel(u32 val, u32 reg)
{
	writel(val, MSM_SSBI_PHYS + reg);
}

//poll_for_device_ready === SSBI_STATUS_READY
//poll_for_transfer_completed === SSBI_STATUS_MCHN_BUSY
//poll_for_read_completed === SSBI_STATUS_RD_READY
static int ssbi_wait_mask(u32 set_mask, u32 clr_mask)
{
	u32 timeout = SSBI_TIMEOUT_US;
	u32 val;

	while (timeout--) {
		val = ssbi_readl(SSBI2_STATUS);
		if (((val & set_mask) == set_mask) && ((val & clr_mask) == 0))
			return 0;
		//udelay(1);
	}

	dprintf("%s: timeout (status %x set_mask %x clr_mask %x)\n",
		__func__, ssbi_readl(SSBI2_STATUS), set_mask, clr_mask);
	return -1;
}

int msm_ssbi_read(u16 addr, u8 *buf, int len)
{
	u32 read_cmd = SSBI_CMD_RDWRN | ((addr & 0xff) << 16);
	//u32 mode2;
	int ret = 0;

	/*
	mode2 = ssbi_readl(SSBI2_MODE2);
	if (mode2 & SSBI_MODE2_SSBI2_MODE) {
		mode2 = (mode2 & 0xf) | (((addr >> 8) & 0x7f) << 4);
		ssbi_writel(mode2, SSBI2_MODE2);
	}
	*/

	while (len) {
		ret = ssbi_wait_mask(SSBI_STATUS_READY, 0);
		if (ret)
			goto err;
		
		ssbi_writel(read_cmd, SSBI2_CMD);
		ret = ssbi_wait_mask(SSBI_STATUS_RD_READY, 0);
		if (ret)
			goto err;
		*buf++ = ssbi_readl(SSBI2_RD) & 0xff;
		len--;
	}
	
err:
	return ret;
}

int msm_ssbi_write(u16 addr, u8 *buf, int len)
{
	//u32 mode2;
	int ret = 0;

	/*
	mode2 = ssbi_readl(SSBI2_MODE2);
	if (mode2 & SSBI_MODE2_SSBI2_MODE) {
		mode2 = (mode2 & 0xf) | (((addr >> 8) & 0x7f) << 4);
		ssbi_writel(mode2, SSBI2_MODE2);
	}
	*/

	while (len) {
		ret = ssbi_wait_mask(SSBI_STATUS_READY, 0);
		if (ret)
			goto err;

		ssbi_writel(((addr & 0xff) << 16) | *buf, SSBI2_CMD);
		ret = ssbi_wait_mask(0, SSBI_STATUS_MCHN_BUSY);
		if (ret)
			goto err;
		buf++;
		len--;
	}

err:
	return ret;
}
