/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <driver.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * move_wrapper
 *
 * Performs the requested actions of a FS_MOVE command.
 */

void move_wrapper(struct mp_fs *cmd) {
	struct fs_obj *dir, *obj;	

	/* make sure the request is within the driver */
	if (RP_PID(cmd->v0) != getpid()) {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
		return;
	}

	/* get the requested object and new parent directory */
	dir = lfs_lookup(cmd->index);
	obj = lfs_lookup(RP_INDEX(cmd->v0));

	if (!dir || !obj) {
		/* return ERR_FILE on failure to find object or directory */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
		return;
	}

	mutex_spin(&dir->mutex);
	mutex_spin(&obj->mutex);

	/* check all permissions */
	if (((acl_get(dir->acl, gettuser()) & ACL_WRITE) == 0) || 
		((acl_get(obj->mother->acl, gettuser()) & ACL_WRITE) == 0)) {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_DENY;

		mutex_free(&dir->mutex);
		mutex_free(&obj->mutex);
		return;
	}

	mutex_free(&dir->mutex);

	/* remove object from its directory */
	lfs_pull(obj);

	/* add object to new directory with name <cmd->s0> */
	lfs_push(dir, obj, cmd->s0);

	mutex_free(&obj->mutex);

	/* return pointer to moved object on success */
	cmd->v0   = getpid();
	cmd->v0 <<= 32;
	cmd->v0  |= obj->inode;
}
