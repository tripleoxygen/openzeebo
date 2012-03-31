/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include "patch.h"

/* *****************************************************************************
 * NTSC color standard PATCH DATA - PATCH
 */


/* *****************************************************************************
 * 1.1.2
 */
static struct patch_data patch_112_data[] = {
	{
	"\x03\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x3d\xf2",
	"\x00\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x3d\xf2",
	16,
	0x162,
	"\x5c\x8f\xab\x43\xdd\x9c\xd2\x62\x89\xfd\x39\xce\x52\x0c\x69\x96\x50\x6b\xb3\x41",
	},
};

struct patch_info patch_112 = {
	.version = "1.1.2",
	.data = patch_112_data,
	.data_count = ARRAY_SIZE(patch_112_data),
};

/* *****************************************************************************
 * 1.1.0
 */
static struct patch_data patch_110_data[] = {
	{
	"\x03\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x53\xf2",
	"\x00\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x53\xf2",
	16,
	0x127,
	"\xe2\x40\x91\xde\x6d\x74\x4f\x16\x11\xd8\xe9\x08\xa4\x33\x7f\xc9\xd9\x36\x2c\xff",
	},
};

struct patch_info patch_110 = {
	.version = "1.1.0",
	.data = patch_110_data,
	.data_count = ARRAY_SIZE(patch_110_data),
};

/* *****************************************************************************
 * Patch list / supported versions
 */
struct patch_info *patch_list[] = {
	&patch_110,
	&patch_112,
	0
};
