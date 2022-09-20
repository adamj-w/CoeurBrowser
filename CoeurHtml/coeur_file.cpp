#include "coeur.h"

#include "coeur_file.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

static char _last_error[512];

CoeurFile coeur_open(const char* filename, coeur_open_flags_t flags) {
	struct coeur_file_v1* file = (struct coeur_file_v1*) malloc(sizeof(coeur_file_v1));
	assert(file);
	memset(file, 0, sizeof(coeur_file_v1));
	
	size_t len = strnlen(filename, MAX_PATH_LEN);
	file->filename = (char*)malloc(len + 2);
	assert(file->filename);
	strncpy_s(file->filename, len + 1, filename, len);
	file->filename[len + 1] = '\0';

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, file->filename, "r");
	if (!fp || err) {
		strerror_s(_last_error, 512, err);
		file_close_v1(file);
		return NULL;
	}
	file->fp = fp;

	assert(file_buffer_file_v1(file));
	return file;
}

void coeur_close(CoeurFile file) {
	file_close_v1(file);
}

const char* coeur_get_file_contents(CoeurFile file) {
	return file->buffer;
}

// PRIVATE METHODS

void file_close_v1(coeur_file_v1* file) {
	if (file) {
		if (file->filename) free(file->filename);
		if (file->fp) fclose((FILE*)file->fp);
		if (file->buffer) free(file->buffer);

		free(file);
	}
}

int file_buffer_file_v1(coeur_file_v1* file) {
	assert(file && file->fp);
	
	FILE* fp = (FILE*)file->fp;
	fseek(fp, 0, SEEK_END);
	file->filelen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file->buffer = (char*)malloc(file->filelen);
	assert(file->buffer);
	memset(file->buffer, 0, file->filelen);

	fread_s(file->buffer, file->filelen, 1, file->filelen, fp);
	if (errno) {
		strerror_s(_last_error, 512, errno);
		return 0;
	}

	return 1;
}
