/*
 * Copyright (C) 2011 Jaagup Repan
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

#include <wmanager.h>
#include <stdlib.h>
#include <ipc.h>
#include <page.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>

const size_t width = 256;
const size_t height = 30;
const size_t size = width * height * 4;
uint8_t *bitmap;

void draw(uint8_t alpha) {
	for (size_t i = 3; i <= size; i += 4)	{
		bitmap[i] = alpha;
	}
}

int main(int argc, char **argv) {
	bitmap = malloc(size);
	memset(bitmap, 0, size);
	for (size_t i = 0; i <= 0xff; i++)	{
		for (size_t line = 0; line < 3; line++) {
			for (size_t j = 0; j < 10; j++) {
				for (size_t c = 0; c < 3; c++) {
					bitmap[(i + (10 * line + j) * width) * 4 + c] = line == c ? i : 0;
				}
			}
		}
	}

	if (wm_init()) {
		fprintf(stderr, "%s: error: cannot initialize window manager\n", argv[0]);
		return 1;
	}

	wm_set_bitmap(0, bitmap, size);
	wm_add_window(0, width, height, 0);
	while (1) {
		for (int alpha = 0; alpha <= 0xff; alpha++) {
			draw(alpha);
		}
		for (int alpha = 0xfe; alpha >= 0; alpha--) {
			draw(alpha);
		}
	}

	return 0;
}
