#include "coeur_common.h"

#include "coeur.h"

#include "coeur_file.h"
#include "coeur_node.h"
#include "networking/coeur_url.h"

#include <assert.h>

static char _lastErrorBuf[512];
static const char* _lastError = _lastErrorBuf;

const char* coeur_get_last_error(void) {
	return _lastError ? _lastError : "";
}

void coeur_destroy(CoeurObject obj) {
	if (obj) {
		if (obj->revision == COEUR_V1) {
			switch (obj->typeId) {
			case COEUR_FILE:
				file_close_v1(OBJ_CAST(coeur_file_v1, obj));
				break;
			case COEUR_NODE:
				node_destroy_v1(OBJ_CAST(coeur_node_v1, obj));
				break;
			case COEUR_URL:
				url_free(OBJ_CAST(coeur_url_v1, obj));
				break;
			default:
				assert(false);
				break;
			}
		}
	}
}

// PRIVATE

void setLastError(const char* str) {
	_lastError = str;
}

char* getLastErrorBuffer(void) {
	_lastError = _lastErrorBuf;
	return &_lastErrorBuf[0];
}

coeur_buffer_v1* buffer_alloc(size_t size) {
	assert(size > 0);
	ZeroAlloc(coeur_buffer_v1, buffer);
	buffer->contentLen = size;
	buffer->content = (char*)malloc(size);
	assert(buffer->content);
	return buffer;
}

void buffer_free(coeur_buffer_v1* buffer) {
	if (buffer) {
		if (buffer->content) free(buffer->content);

		free(buffer);
	}
}
