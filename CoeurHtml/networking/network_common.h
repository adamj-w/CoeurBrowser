#pragma once

#include "coeur.h"

#include "../coeur_common.h"
#include "coeur_url.h"

struct connection_v1;

connection_v1* connection_open(coeur_url_v1* url);
void connection_close(connection_v1* connection);

int connection_send(connection_v1* conn, void* buf, int bufsize);
coeur_buffer_v1* connection_recv(connection_v1* conn, bool closeAfter); 
