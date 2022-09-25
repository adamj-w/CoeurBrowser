#include "http.h"

#include "coeur_url.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#define BUFSIZE (2048)

coeur_buffer_v1* http_fetch_from_url(coeur_url_v1* url) {
	assert(url);
	connection_v1* conn = connection_open(url);
	if (!conn) return NULL;

	char* sendBuf = (char*)malloc(BUFSIZE);
	assert(sendBuf);
	sendBuf[0] = '\0';
	size_t written = 0;

	written += snprintf(&sendBuf[written], BUFSIZE - written, "GET ");

	strncpy_s(&sendBuf[written], BUFSIZE - written, url->path, url->pathLen);
	written += url->pathLen;

	written += snprintf(&sendBuf[written], BUFSIZE - written, " HTTP/1.1\r\nHost: ");

	strncpy_s(&sendBuf[written], BUFSIZE - written, url->domainName, url->domainNameLen);
	written += url->domainNameLen;

	written += snprintf(&sendBuf[written], BUFSIZE - written, "\r\n\r\n");

	connection_send(conn, sendBuf, written);

	free(sendBuf);

	coeur_buffer_v1* buffer = connection_recv(conn, true);

	connection_close(conn);
	return buffer;
}
