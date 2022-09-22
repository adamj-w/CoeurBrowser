#include "coeur_common.h"

#include "coeur.h"

static char _lastErrorBuf[512];
static const char* _lastError = _lastErrorBuf;

const char* coeur_get_last_error(void) {
	return _lastError ? _lastError : "";
}

// PRIVATE

void setLastError(const char* str) {
	_lastError = str;
}

char* getLastErrorBuffer(void) {
	_lastError = _lastErrorBuf;
	return &_lastErrorBuf[0];
}

void buffer_free(coeur_buffer_v1* buffer) {
	if (buffer) {
		if (buffer->content) free(buffer->content);

		free(buffer);
	}
}
