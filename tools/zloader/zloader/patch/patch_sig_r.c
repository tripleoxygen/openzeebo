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
 * SIG enable/restore PATCH DATA
 */

/* *****************************************************************************
 * 2.1.2 MX data
 */
static struct patch_data patch_212mx_data[] = {
	{
        "\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf5\xf7\xbe\xef\x05\x1c\x00\xd0",
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf5\xf7\xbe\xef\x05\x1c\x00\xd0",
	16,
	0x13c,
	"\x74\xfe\x6c\x5b\x75\x01\x73\x1e\xb2\xa8\x9b\x54\x59\x69\x3b\xf0\x91\xf4\x5e\x67",
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
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf4\xf7\x3e\xef\x05\x1c\x00\xd0",
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf4\xf7\x3e\xef\x05\x1c\x00\xd0",	
	16,
	0x14a,
	"\x21\x3b\x54\x7a\xc2\x2f\xf6\x48\x0e\x5f\x86\x23\xbb\xb5\xac\x6a\xf8\xec\x19\xf7",
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
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\x05\xf0\x2c\xfa\x05\x1c\x00\xd0",
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\x05\xf0\x2c\xfa\x05\x1c\x00\xd0",
	16,
	0x12c,
	"\x1a\xf4\xa9\x39\x95\x6b\xc9\x43\xb5\x29\xb0\x81\xb3\xda\x8f\xa5\x7b\x56\x14\xe2",
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
	"\x00\x25\x7d\xe0\x7f\x49\xa0\x68\xf8\xf7\x2e\xe9\x05\x1c\x00\xd0",
	"\xc0\x46\x0e\xaa\x7f\x49\xa0\x68\xf8\xf7\x2e\xe9\x05\x1c\x00\xd0",
	16,
	0x149,
	"\x5c\x1d\xdb\x82\x1f\xd6\x4c\x2a\xea\x86\xdf\x1e\x9a\x7c\xbf\x1e\xf6\x69\xb6\x99",
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
