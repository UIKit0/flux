/* Copyright 2010 Nick Johnson */

#include <stdio.h>
#include <stdlib.h>
#include <flux.h>
#include <signal.h>
#include <string.h>

FILE *stdin  = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int fclose(FILE *stream) {
	if (stream->buffer) {
		free(stream->buffer);
	}
	free(stream);

	return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	struct request *req = req_alloc(), *res;
	uint8_t *data = (void*) ptr;
	uint16_t datasize;
	uint32_t oldsize, i = 0;

	size *= nmemb;
	oldsize = size;

	sighold(SIG_REPLY);

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req->resource = stream->resource;
		req->datasize = datasize;
		req->transid  = i;
		req->dataoff  = 0;
		req->format   = REQ_WRITE;
		req->fileoff[0] = stream->position;
		req->fileoff[1] = 0;
		req->fileoff[2] = 0;
		req->fileoff[3] = 0;

		memcpy(req->reqdata, data, datasize);

		fire(stream->target, stream->wport, req_checksum(req));

		res = sigpull(SIG_REPLY);

		data = &data[res->datasize];
		size -= res->datasize;
		req_free(res);
		i++;
	}

	sigfree(SIG_REPLY);

	return oldsize;
}
