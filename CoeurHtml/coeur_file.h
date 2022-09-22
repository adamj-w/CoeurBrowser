#pragma once

#include <stddef.h>
#include <stdint.h>

#include "coeur_common.h"

#define MAX_PATH_LEN 256

struct coeur_file_v1 {
	char* filename = NULL;
	void* fp;

	//size_t filelen = 0;
	//char* buffer = NULL;
	coeur_buffer_v1* buffer;
};

void file_close_v1(coeur_file_v1* file);
int file_buffer_file_v1(coeur_file_v1* file);
