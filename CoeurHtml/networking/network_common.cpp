#include "network_common.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <assert.h>

#include <stdio.h>

struct connection_v1 {
	bool open;
	SOCKET s;
	struct sockaddr_in addr;
};

connection_v1* connection_open(coeur_url_v1* url) {
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		setLastError("internal networking error");
		return NULL;
	}
	ZeroAlloc(connection_v1, conn);

	if ((conn->s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		setLastError("internal networking error");
		free(conn);
		return NULL;
	}

	char ipAddrStr[17];
	strncpy_s(ipAddrStr, 17, url->domainName, url->domainNameLen);

	conn->addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, ipAddrStr, &conn->addr.sin_addr) == 0) {
		setLastError("DNS not supported, use ip address");
		connection_close(conn);
		return NULL;
	}
	conn->addr.sin_port = htons(url->port);

	//int size = sizeof(struct sockaddr_in);
	if (connect(conn->s, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) == SOCKET_ERROR) {
		printf("%d\n", WSAGetLastError());
		setLastError("failed to connect");
		closesocket(conn->s);
		free(conn);
		assert(false);
		return NULL;
	}

	conn->open = true;

	return conn;
}

void connection_close(connection_v1* conn) {
	if (conn) {
		if (conn->s) {
			closesocket(conn->s);
		}

		free(conn);
	}
}

int connection_send(connection_v1* conn, void* buf, int bufsize) {
	assert(conn && conn->open && buf && bufsize > 0);
	int sent = 0;
	if ((sent = send(conn->s, (char*)buf, bufsize, 0)) == SOCKET_ERROR) {
		setLastError("Failed to send message");
		printf("%d\n", WSAGetLastError());
		return -1;
	}
	return sent;
}

coeur_buffer_v1* connection_recv(connection_v1* conn, bool closeAfter) {
	// TODO: change to smart receive for HTTP
	assert(conn && conn->open);
	int recvd = 0, count = 0;
	size_t total = 0;
	char* bufs[16];
	
	do {
		bufs[count] = (char*)malloc(1024);
		assert(bufs[count]);
		memset(bufs[count], 0, 1024);

		if ((recvd = recv(conn->s, bufs[count], 1024, 0)) == SOCKET_ERROR) {
			setLastError("Failed to read server response");
			printf("%d\n", WSAGetLastError());
			return NULL;
		}
		total += recvd;
		count++;
	} while (recvd >= 1024 && count < 16);

	coeur_buffer_v1* buffer = buffer_alloc(total);

	for (size_t i = 0; i < count; i++) {
		memcpy_s(&buffer->content[i * 1024], buffer->contentLen - (i * 1024), bufs[i], i == count - 1 ? recvd : 1024);
		free(bufs[i]);
	}

	return buffer;
}
