#pragma once

#include "coeur.h"
#include "coeur_common.h"

#define COEUR_NODE_HTML (0)
#define COEUR_NODE_HEAD (1)
#define COEUR_NODE_BODY (2)
#define COEUR_NODE_TITLE (3)
#define COEUR_NODE_H1 (4)
#define COEUR_NODE_UNKNOWN (5)

typedef uint8_t coeur_node_type_t;
typedef uint8_t coeur_token_type_t;

struct node_type_entry {
	const char* lowercaseName;
	coeur_node_type_t typeIndex;
};

extern struct node_type_entry _nodeTypeLookup[COEUR_NODE_UNKNOWN];

struct coeur_node_prop_v1 {
	const char* propName;
	size_t propNameLen;

	const char* propVal;
	size_t propValLen;
};

#define NODE_TOKEN_RAW (1)
#define NODE_TOKEN_TAG_START (2)
#define NODE_TOKEN_TAG_END (3)
#define NODE_TOKEN_TAG_PROP (4)
#define NODE_TOKEN_STRING (5)
#define NODE_TOKEN_UNKNOWN (6)

// Intermediary
struct node_token_v1 {
	coeur_token_type_t type;
	
	const char* content;
	size_t contentLen;

	bool closer;

	node_token_v1* next;
	node_token_v1 *prev;
};

struct coeur_node_v1 {
	coeur_node_type_t type;
	bool selfClose = false;

	const char* openTag;
	size_t openTagLen;

	const char* rawContent;
	size_t rawContentLen;

	const char* closeTag;
	size_t closeTagLen;

	coeur_node_prop_v1* properties = NULL;
	size_t propertiesLen = 0;

	coeur_node_v1* next = NULL;
	coeur_node_v1* prev = NULL;

	coeur_node_v1* parent = NULL;
	coeur_node_v1* firstChild = NULL;
};

coeur_node_v1* node_parse_from_buffer(coeur_buffer_v1* buffer);

coeur_node_v1* node_create_v1(void);
coeur_node_v1* node_destroy_v1(coeur_node_v1* node);
coeur_node_v1* node_construct_tree(node_token_v1* head);

node_token_v1* tokenize_file_buffer(coeur_buffer_v1* buffer);
void token_list_free(node_token_v1* head);

coeur_node_type_t node_parse_type(const char* typeName, size_t* oTagLen);
