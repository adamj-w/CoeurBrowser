#pragma once

#include <stdlib.h>
#include <string.h>

#define ZeroAlloc(_type, _name) _type* _name = (_type*)malloc(sizeof(_type)); \
	assert(_name); \
	memset(_name, 0, sizeof(_type))

void setLastError(const char* str);
char* getLastErrorBuffer(void);

struct coeur_buffer_v1 {
	char* content;
	size_t contentLen;
};

void buffer_free(coeur_buffer_v1* buffer);
