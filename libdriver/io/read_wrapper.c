/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

#include <driver/vfs.h>
#include <driver/io.h>

/*****************************************************************************
 * __read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void __read_wrapper(struct msg *msg) {
	struct vfs_obj *file;
	uint64_t offset;
	uint32_t size;
	struct msg *reply;

	if (msg->length != sizeof(uint64_t) + sizeof(uint32_t)) {
		merror(msg);
		return;
	}

	if (!_di_read) {
		merror(msg);
		return;
	}

	file = vfs_get_index(RP_INDEX(msg->target));

	if (!file || !(file->type & RP_TYPE_FILE)) {
		merror(msg);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & PERM_READ)) {
		merror(msg);
		return;
	}
	
	offset = ((uint64_t*) msg->data)[0];
	size   = ((uint32_t*) msg->data)[2];
	
	reply = aalloc(sizeof(struct msg) + size, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = size;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;

	free(msg);

	reply->length = _di_read(msg->source, file, reply->data, size, offset);

	msend(reply);
}
