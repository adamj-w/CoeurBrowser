#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define COEUR_V1 (1)

#define COEUR_FILE (0)
#define COEUR_NODE (1)
#define COEUR_URL (2)
#define COEUR_TYPE_COUNT (3)
struct coeur_object {
	uint16_t revision;
	uint16_t typeId;
	void* object;
};

#define OBJ_CAST(_type, _name) ((_type*)_name->object)

#define ZeroAlloc(_type, _name) _type* _name = (_type*)malloc(sizeof(_type)); \
	assert(_name); \
	memset(_name, 0, sizeof(_type))

void setLastError(const char* str);
char* getLastErrorBuffer(void);

struct coeur_buffer_v1 {
	char* content;
	size_t contentLen;
};

coeur_buffer_v1* buffer_alloc(size_t size);
void buffer_free(coeur_buffer_v1* buffer);
