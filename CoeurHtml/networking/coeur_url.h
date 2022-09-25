#pragma once

#include "coeur.h"
#include "../coeur_common.h"

#define URL_SCHEME_HTTP (0)
#define URL_SCHEME_UNKNOWN (1)
typedef uint8_t url_scheme_t;

struct coeur_url_v1 {
	char* fullUrl;

	url_scheme_t scheme;
	const char* domainName;
	size_t domainNameLen;

	uint16_t port;

	const char* path;
	size_t pathLen;

	coeur_buffer_v1* buffer;
};

coeur_url_v1* url_parse(const char* url);
void url_free(coeur_url_v1* url);

void url_fetch_contents(coeur_url_v1* url);

url_scheme_t url_parse_scheme(const char* scheme, size_t len);
