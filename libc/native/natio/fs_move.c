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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <natio.h>

/*****************************************************************************
 * fs_move
 *
 * Attempts to move the file <file> into directory <dir> with name <name>. If 
 * <file> and <dir> are not in the same driver, this will likely fail, and the 
 * file will have to be manually copied. Returns a pointer to the new 
 * filsystem object (which may be the same as the old filesystem object) on 
 * success, NULL on failure.
 */

uint64_t fs_move(uint64_t dir, const char *name, uint64_t fobj) {
	struct fs_cmd command;

	command.op = FS_MOVE;
	command.v0 = fobj;
	command.v1 = 0;
	strlcpy(command.s0, name, 4000);
	
	if (!fs_send(dir, &command)) {
		return 0;
	}

	return command.v0;
}