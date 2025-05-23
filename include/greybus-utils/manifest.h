/*
 * Copyright (c) 2014-2015 Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GREYBUS_UTILS_MANIFEST_H_
#define _GREYBUS_UTILS_MANIFEST_H_

#include <zephyr/sys/dlist.h>
#include <zephyr/types.h>

struct gb_cport {
	sys_dnode_t node;
	int id;
	int bundle;
	int protocol;
};

typedef void (*manifest_handler)(unsigned char *manifest_file, int device_id, int manifest_number);
void foreach_manifest(manifest_handler handler);
void enable_cports(void);
void *get_manifest_blob(void);
void set_manifest_blob(void *blob);
bool manifest_parse(void *data, size_t size);
bool manifest_patch(uint8_t **mnfb, void *data, size_t size);
void parse_manifest_blob(void *manifest);
void enable_manifest(char *name, void *manifest, int device_id);
void disable_manifest(char *name, void *priv, int device_id);
void release_manifest_blob(void *manifest);
sys_dlist_t *get_manifest_cports(void);
int get_manifest_size(void);
size_t manifest_get_max_bundle_id(void);
size_t manifest_get_num_cports_bundle(int bundle_id);
unsigned int manifest_get_start_cport_bundle(int bundle_id);

#endif
