
/*
 * Copyright (c) 2012, OpenZeebo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>

#include <boot/boot.h>
#include <boot/uart.h>
#include <boot/tags.h>
#include <boot/flash.h>
#include <boot/board.h>

#include <msm7k/ssbi.h>
#include <msm7k/nand.h>
#include <msm7k/gpio.h>

#include <../libc/sha.h>

#include "patch/patch.h"

#define BLOCK_SIZE		2048 * 64
#define BLOCK_PAGE_COUNT	64
#define PAGE_SIZE		2048
#define DEVICE_MAX_BLOCKS	1024
#define SPARE_SIZE		64

#ifdef DEBUG
#define D(x...) dprintf("[debug]: " x)
#else
#define D(x...) do {} while (0)
#endif

static void *block_data;
static void *page_data;
static void *spare_data;
static unsigned block_pages_status[BLOCK_PAGE_COUNT];

char bootloader_ver[] = "ver0.1";
char bootloader_author[][16] = {
	"OpenZeebo",
	"Triple Oxygen",
};

enum blink_types {
	STATUS_SUPPORTED,
	STATUS_UNSUPPORTED,
	STATUS_FAILED,
	STATUS_SUCCESS,
};

unsigned blink_delay[] = {
	[STATUS_SUPPORTED] = 2000000,
	[STATUS_UNSUPPORTED] = 320000,
	[STATUS_FAILED] = 80000,
	[STATUS_SUCCESS] = 0xffffffff,
};

static void nop_delay(unsigned d)
{
	while(d--) {
		asm ("nop;");
		wd();
	}
}

static inline void panel_off(void)
{
	writel(readl(GPIO_OUT_0) | (1 << 15), GPIO_OUT_0);
}

static inline void panel_on(void)
{
	writel(readl(GPIO_OUT_0) & ~(1 << 15), GPIO_OUT_0);
}

static void led_invert()
{
	unsigned char led1, led2;

	msm_ssbi_read(0xd4, &led1, sizeof(led1));
	msm_ssbi_read(0x5e, &led2, sizeof(led2));
	led1 ^= 1;
	led2 ^= 1;
	msm_ssbi_write(0xd4, &led1, sizeof(led1));
	msm_ssbi_write(0x5e, &led2, sizeof(led2));
}

static void led_on()
{
	unsigned char led1, led2;

	msm_ssbi_read(0xd4, &led1, sizeof(led1));
	msm_ssbi_read(0x5e, &led2, sizeof(led2));
	led1 |= 1;
	led2 |= 1;
	msm_ssbi_write(0xd4, &led1, sizeof(led1));
	msm_ssbi_write(0x5e, &led2, sizeof(led2));
}

static void led_off()
{
	unsigned char led1, led2;

	msm_ssbi_read(0xd4, &led1, sizeof(led1));
	msm_ssbi_read(0x5e, &led2, sizeof(led2));
	led1 &= ~(1 << 1);
	led2 &= ~(1 << 1);
	msm_ssbi_write(0xd4, &led1, sizeof(led1));
	msm_ssbi_write(0x5e, &led2, sizeof(led2));
}

static void blink(enum blink_types type, int (*wait_for)())
{
	unsigned speed = blink_delay[type];

	while(!wait_for()) {
		wd();
		led_invert();
		nop_delay(speed);
	}
}

static int wait_power_pressed()
{
	unsigned char status;

	wd();
	msm_ssbi_read(0x41, &status, sizeof(status));
	return (status & (1 << 3));
}

static int wait_forever()
{
	return 0;
}

static void *find_pattern(void *buf, uint32_t size,
			const void *pattern, uint32_t psize)
{
	char *ptr = buf;
	char *end = (char *)buf + size;

	while((ptr + psize) < end) {
		if(!memcmp(ptr, pattern, psize))
			return ptr;
		ptr++;
		wd();
	}

	return 0;
}

static int flash_read_block(void *buf, unsigned block, unsigned *page_status)
{
	unsigned page, lastpage, idx;

	if(!page_status)
		return -1;

	if(block > DEVICE_MAX_BLOCKS)
		return -1;

	page = block << 6;
	lastpage = page + BLOCK_PAGE_COUNT;
	idx = 0;

	memset(page_status, '\0', sizeof(unsigned) * 64);

	while(page < lastpage) {

		wd();

		if(flash_read_page(page, (char *)buf + PAGE_SIZE * idx, spare_data)) {
			D("failed reading page %d\n", page);
		} else {
			page_status[idx] = 1;
		}

		idx++;
		page++;
		wd();
	}

	return 0;
}

static int find_pattern_location(unsigned block_start, unsigned block_end,
				const char *pattern, unsigned size,
				unsigned *block_out, void **ptr_out)
{
	unsigned rc, block = block_start;
	void *ptr;

	if(!block_out || !ptr_out)
		return -1;

	while(block < block_end) {

		D("Processing block %x ...\n", block);

		if(flash_read_block(block_data, block, block_pages_status)) {
			D("Failed reading block %x ...\n", block);
			continue;
		}

		ptr = find_pattern(block_data, BLOCK_SIZE, pattern, size);
		if(ptr) {
			*ptr_out = ptr;
			*block_out = block;
			D("pattern found @ block %x ( ptr = %x, offset = %x )\n",
				block, ptr, ((char *)ptr - (char *)block_data));
			return 0;
		}
		wd();
		block++;
	}

	return -1;
}

static int flash_write_block(void *buf, unsigned block, unsigned *page_status)
{
	unsigned page, lastpage, idx;

	if(!page_status || !buf)
		return -1;

	if(block > DEVICE_MAX_BLOCKS)
		return -1;

	wd();

	page = block << 6;
	lastpage = page + BLOCK_PAGE_COUNT;
	idx = 0;

	if(flash_erase_block(block)) {
		D("cannot erase block %x\n", block);
		return -1;
	}

	while(page < lastpage) {

		wd();

		if(page_status[idx]) {
			if(flash_write_page(page, (char *)buf + PAGE_SIZE * idx, spare_data)) {
				D("failed writing page %d\n", page);
			} else {
				D("wrote page %d OK\n", page);
			}
		} else {
			D("originally unprogrammed page %x, skipping.\n", page);
		}

		idx++;
		page++;
		wd();
	}

	return 0;
}

static int check_compatible(void *buf, char *checksum)
{
	uint8_t sum[20];

	SHA(buf, BLOCK_SIZE, (uint8_t *)&sum);
	return memcmp(checksum, sum, 20);
}

static int apply_patch(void *ptr, unsigned block, struct patch_data *pd)
{
	/* ptr will point to the correct patch position inside the block_data
	 * i.e. ptr = block_data + offset_where_pattern_was_found */
	memcpy(ptr, pd->patch_pattern, pd->size);
	return flash_write_block(block_data, block, block_pages_status);
}

static int process_patch_data(struct patch_data *pd)
{
	unsigned block;
	void *ptr;

	if(find_pattern_location(pd->probable_block - 2, pd->probable_block + 2,
		pd->original_pattern, pd->size,	&block, &ptr)) {
		D("patch not found\n");
		return -1;
	}

	D("---- patch_data found\n");
	D("----- block %x, ptr %x\n", block, ptr);

	if(check_compatible(block_data, pd->block_sha1)) {
		D("Incorrect checksum.\n");
		return -1;
	}

	D("checksum OK. awaiting confirmation...\n");
	blink(STATUS_SUPPORTED, wait_power_pressed);

	if(apply_patch(ptr, block, pd)) {
		D("failed to apply patch!\n");
		blink(STATUS_FAILED, wait_forever);
	}

	return 0;
}

void die()
{
	while(1)
		wd();
}

int _main(void)
{
	uint8_t sha[20];
	unsigned i, p, block, supported = 0;
	void *ptr;
	struct patch_info *pi;
	struct patch_data *pd;

#ifdef DEBUG
	uart_clock_init();
	gpio_pin_cfg(0x44, 0x5);
	gpio_pin_cfg(0x45, 0x5);
	dprintf_set_putc(uart_putc);
#endif

	board_init();

	page_data = alloc(PAGE_SIZE);
	if(!page_data) {
		D("Failed to alloc page_data.\n");
		blink(STATUS_UNSUPPORTED, wait_forever);
	}

	block_data = alloc(BLOCK_SIZE);
	if(!block_data) {
		D("Failed to alloc block_data.\n");
		blink(STATUS_UNSUPPORTED, wait_forever);
	}

	spare_data = alloc(SPARE_SIZE);
	if(!spare_data) {
		D("Failed to alloc spare_data.\n");
		blink(STATUS_UNSUPPORTED, wait_forever);
	}

	wd();

#ifdef DEBUG
	nop_delay(2000000 * 3);
#endif

	D("init\n");
	D("block_data = %x\n", block_data);

	flash_init();
	p = 0;

	while(patch_list[p]) {
		pi = patch_list[p];
		D("patch_list[%d] is for ver '%s' and has %d patches.\n",
			p, pi->version, pi->data_count);

		for(i = 0; i < pi->data_count; i++) {

			pd = &pi->data[i];
			D("-- patch_data[%d] is %d long, probable block %x\n",
				i, pd->size, pd->probable_block);

			if(!process_patch_data(pd)) {
				D("patch applied.");
				supported = 1;
			}
		}
		p++;
	}

	if(!supported) {
		D("No applicable patch.\n");
		blink(STATUS_UNSUPPORTED, wait_forever);
	} else {
		led_on();
		die();
	}

	return 0;
}
