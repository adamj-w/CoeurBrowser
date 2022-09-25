#include "coeur_url.h"

#include "../coeur_node.h"
#include "http.h"

#include <assert.h>
#include <ctype.h>

CoeurObject coeur_create_url(const char* url, coeur_url_flags_t flags) {
	coeur_url_v1* urlObj = url_parse(url);
	if (!urlObj) return NULL;
	ZeroAlloc(coeur_object, obj);
	obj->revision = COEUR_V1;
	obj->typeId = COEUR_URL;
	obj->object = urlObj;
	return obj;
}

CoeurObject coeur_parse_from_url(CoeurObject urlObj, coeur_parse_flags_t flags) {
	if (!urlObj || urlObj->typeId != COEUR_URL) {
		setLastError("Invalid argument");
		return NULL;
	}

	coeur_url_v1* url = OBJ_CAST(coeur_url_v1, urlObj);
	if (!url->buffer) {
		url_fetch_contents(url);
		if (!url->buffer) {
			return NULL;
		}
	}

	coeur_node_v1* node = node_parse_from_buffer(url->buffer);
	if (!node) return NULL;
	ZeroAlloc(coeur_object, obj);
	obj->revision = COEUR_V1;
	obj->typeId = COEUR_NODE;
	obj->object = node;
	return obj;
}

// PRIVATE METHODS

#define PARSE_DOMAIN (1)
#define PARSE_PORT (2)
#define PARSE_PATH (3)
coeur_url_v1* url_parse(const char* url) {
	size_t len = strnlen_s(url, 512);
	if (0 == len) {
		setLastError("no url passed into coeur_create_url");
		return NULL;
	}

	ZeroAlloc(coeur_url_v1, out);
	out->fullUrl = (char*)calloc(len + 1, sizeof(char));
	assert(out->fullUrl);
	out->fullUrl[0] = '\0';
	strncpy_s(out->fullUrl, len + 1, url, len);
	out->fullUrl[len] = '\0';

	char* schemeEnd = strstr(out->fullUrl, "://");
	if (!schemeEnd) {
		setLastError("Could not determine the url's scheme (scheme://domain:port/path)");
		url_free(out);
		return NULL;
	}

	size_t schemeLen = (size_t)((uintptr_t)schemeEnd - (uintptr_t)&out->fullUrl[0]);
	out->scheme = url_parse_scheme(out->fullUrl, schemeLen);

	out->domainName = &schemeEnd[3];
	size_t curr = 0;
	while (out->domainName[curr] && out->domainName[curr] != ':' && out->domainName[curr] != '/') {
		curr++;
	}

	if (curr == 0) {
		setLastError("Could not determine the url's domain (scheme://domain:port/path)");
		url_free(out);
		return NULL;
	}
	out->domainNameLen = curr;

	char* nextComp = (char*) & out->domainName[curr];
	if (out->domainName[curr] == ':') {
		out->port = (uint16_t)strtoul(&out->domainName[curr + 1], &nextComp, 10); // TODO safe cast
		if (errno) {
			errno = 0;
			setLastError("Could not determine the url's port (scheme://domain:port/path)");
			url_free(out);
			return NULL;
		}
	}

	if (nextComp) {
		out->path = nextComp;
		out->pathLen = strnlen(nextComp, len);
	}

	// TODO: query strings, header things, etc

	return out;
}

void url_free(coeur_url_v1* url) {
	if (url) {
		if (url->fullUrl) free(url->fullUrl);
		if (url->buffer) buffer_free(url->buffer);
		
		free(url);
	}
}

void url_fetch_contents(coeur_url_v1* url) {
	assert(url);
	if (url->scheme == URL_SCHEME_HTTP) {
		url->buffer = http_fetch_from_url(url);
	} else {
		setLastError("Unsupported url scheme");
		url->buffer = NULL;
	}
}

struct scheme_id_table {
	const char* lowerName;
	size_t nameLen;
	url_scheme_t schemeId;
};

static scheme_id_table schemeIdTable[URL_SCHEME_UNKNOWN] = {
	{"http", 4, URL_SCHEME_HTTP},
	
};

url_scheme_t url_parse_scheme(const char* scheme, size_t len) {
	for (size_t i = 0; i < URL_SCHEME_UNKNOWN; i++) {
		if (len == schemeIdTable[i].nameLen) {
			for (size_t j = 0; j < len; j++) {
				if (tolower(scheme[j]) != schemeIdTable[i].lowerName[j]) {
					break;
				}

				if (j + 1 == len) {
					return schemeIdTable[i].schemeId;
				}
			}
		}
	}

	return URL_SCHEME_UNKNOWN;
}
