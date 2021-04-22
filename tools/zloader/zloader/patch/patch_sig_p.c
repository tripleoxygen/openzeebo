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
 * Homebrew patch/SIG disable PATCH DATA
 */

/* *****************************************************************************
 * 2.1.2 MX data
 */
static struct patch_data patch_212mx_data[] = {
	{
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf5\xf7\xbe\xef\x05\x1c\x00\xd0",
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf5\xf7\xbe\xef\x05\x1c\x00\xd0",
	16,
	0x13c,
	"\x38\x19\x47\xdf\x1a\x69\xfe\x3c\xbb\x12\xff\xb8\x9d\x71\x4d\x0f\xb2\x06\xcd\x3a",
	},
};

struct patch_info patch_212mx = {
	.version = "2.1.2 MX",
	.data = patch_212mx_data,
	.data_count = ARRAY_SIZE(patch_212mx_data),
};

/* *****************************************************************************
 * 1.1.2 data
 */
static struct patch_data patch_112_data[] = {
	{
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf4\xf7\x3e\xef\x05\x1c\x00\xd0",
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf4\xf7\x3e\xef\x05\x1c\x00\xd0",
	16,
	0x14a,
	"\x53\x34\x50\x38\xc6\xb1\x4f\x05\x3a\xbc\x9c\x76\x47\x3d\x87\x5b\x5a\xad\xf4\x9c",
	},
};

struct patch_info patch_112 = {
	.version = "1.1.2",
	.data = patch_112_data,
	.data_count = ARRAY_SIZE(patch_112_data),
};

/* *****************************************************************************
 * 1.1.1 data
 */
static struct patch_data patch_111_data[] = {
	{
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\x05\xf0\x2c\xfa\x05\x1c\x00\xd0",
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\x05\xf0\x2c\xfa\x05\x1c\x00\xd0",
	16,
	0x12c,
	"\x47\xa7\xf1\x50\x6f\x49\x6f\x9e\x2f\xe4\xe2\x05\x07\xc6\x9b\xf8\x43\xd0\x67\x4a",
	},
};

struct patch_info patch_111 = {
	.version = "1.1.1",
	.data = patch_111_data,
	.data_count = ARRAY_SIZE(patch_111_data),
};

/* *****************************************************************************
 * 1.1.0 data
 */
static struct patch_data patch_110_data[] = {
	{
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf8\xf7\x2e\xe9\x05\x1c\x00\xd0",
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf8\xf7\x2e\xe9\x05\x1c\x00\xd0",
	16,
	0x149,
	"\xf7\x5f\x30\x04\xac\xaa\x6c\xbf\xca\x5a\x1b\x24\x46\x3d\x35\x19\x3c\xa1\xd8\x00",
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
	&patch_111,
	&patch_112,
	&patch_212mx,
	0
};
