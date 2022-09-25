#include "coeur.h"

#include "coeur_file.h"
#include "coeur_common.h"
#include "coeur_node.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

CoeurObject coeur_open(const char* filename, coeur_file_flags_t flags) {
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
		strerror_s(getLastErrorBuffer(), 512, err);
		file_close_v1(file);
		return NULL;
	}
	file->fp = fp;

	assert(file_buffer_file_v1(file));

	ZeroAlloc(coeur_object, object);
	object->revision = COEUR_V1;
	object->typeId = COEUR_FILE;
	object->object = (void*)file;
	return object;
}

CoeurObject coeur_parse_file(CoeurObject fileObj, coeur_parse_flags_t flags) {
	if (!fileObj || fileObj->typeId != COEUR_FILE || !OBJ_CAST(coeur_file_v1, fileObj)->buffer) {
		setLastError("Invalid file passed to method");
		assert(false);
		return NULL;
	}

	coeur_file_v1* file = OBJ_CAST(coeur_file_v1, fileObj);

	coeur_node_v1* node = node_parse_from_buffer(file->buffer);
	ZeroAlloc(coeur_object, obj);
	obj->revision = COEUR_V1;
	obj->typeId = COEUR_NODE;
	obj->object = node;
	return obj;
}

const char* coeur_get_file_contents(CoeurObject file) {
	if (file) {
		if (file->typeId != COEUR_FILE || file->revision != COEUR_V1) {
			setLastError("Invalid Argument");
			return NULL;
		}

		if (((coeur_file_v1*)file->object)->buffer) return OBJ_CAST(coeur_file_v1, file)->buffer->content;
	}
	return NULL;
}

// PRIVATE METHODS

void file_close_v1(coeur_file_v1* file) {
	if (file) {
		if (file->filename) free(file->filename);
		if (file->fp) fclose((FILE*)file->fp);
		if (file->buffer) buffer_free(file->buffer);

		free(file);
	}
}

int file_buffer_file_v1(coeur_file_v1* file) {
	assert(file && file->fp && !file->buffer);

	ZeroAlloc(coeur_buffer_v1, buffer);
	
	FILE* fp = (FILE*)file->fp;
	fseek(fp, 0, SEEK_END);
	buffer->contentLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer->content = (char*)malloc(buffer->contentLen);
	assert(buffer->content);
	memset(buffer->content, 0, buffer->contentLen);

	fread_s(buffer->content, buffer->contentLen, 1, buffer->contentLen, fp);
	if (errno) {
		strerror_s(getLastErrorBuffer(), 512, errno);
		return 0;
	}

	file->buffer = buffer;

	return 1;
}
