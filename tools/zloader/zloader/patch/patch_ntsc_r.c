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
 * NTSC color standard PATCH DATA - RESTORE
 */


/* *****************************************************************************
 * 1.1.2
 */
static struct patch_data patch_112_data[] = {
	{
	"\x00\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x3d\xf2",
	"\x03\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x3d\xf2",
	16,
	0x162,
	"\x40\x4a\x80\xf0\x86\x31\xaf\xd8\x0d\x20\x08\xbe\x0d\xc2\xaa\x2a\xdb\x78\xc1\x9e",
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
	"\x00\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x53\xf2",
	"\x03\x24\x11\x98\x00\x28\x23\xdb\x11\x98\x01\x22\x0b\xa9\x53\xf2",
	16,
	0x127,
	"\x03\xfe\xc9\xfd\x8d\x4c\xbd\x5b\x78\x0c\xa8\xe3\x2f\x9e\xf9\xd0\x3f\xd6\x41\x2c",
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
