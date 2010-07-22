/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <mutex.h>
#include <ipc.h>
#include <proc.h>
#include <io.h>

#include <stdio.h>
#include <string.h>

#include "inc/tar.h"

static uint8_t *initrd;
static size_t   initrd_size;
static bool   m_initrd;

static void initrd_info(uint32_t source, struct packet *packet) {
	struct info_query *query;

	if (!packet) {
		return;
	}

	query = packet_getbuf(packet);

	if (!strcmp(query->field, "size")) {
		sprintf(query->value, "%d", initrd_size);
	}

	send(PORT_REPLY, source, packet);
	packet_free(packet);
}

static void initrd_read(uint32_t source, struct packet *packet) {
	uintptr_t offset;

	if (!packet) {
		return;
	}

	offset = (uintptr_t) packet->offset;

	if (offset + packet->data_length > initrd_size) {
		if (offset > initrd_size) {
			packet->data_length = 0;
		}
		else {
			packet->data_length = initrd_size - offset;
		}
	}

	mutex_spin(&m_initrd);
	memcpy(packet_getbuf(packet), &initrd[offset], packet->data_length);
	mutex_free(&m_initrd);

	send(PORT_REPLY, source, packet);
}

void initrd_init() {
	initrd = (uint8_t*) BOOT_IMAGE;
	initrd_size = tar_size(initrd);

	when(PORT_READ, initrd_read);
	when(PORT_INFO, initrd_info);
}
