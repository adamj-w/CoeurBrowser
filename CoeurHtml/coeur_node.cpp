#include "coeur_node.h"

#include "coeur_file.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

CoeurNode coeur_parse_file(CoeurFile file, coeur_parse_flags_t flags) {
	assert(file && file->buffer);

	// Tokenize the file
	node_token_v1* head = tokenize_file_buffer(file);

	CoeurNode node = node_create_v1();

	return node;
}

void coeur_free_node(CoeurNode node) {
	node_destroy_v1(node);
}

// PRIVATE METHODS

coeur_node_v1* node_create_v1(void) {
	coeur_node_v1* node = (coeur_node_v1*)malloc(sizeof(coeur_node_v1));
	assert(node);
	memset(node, 0, sizeof(coeur_node_v1));
	return node;
}

// returns next node in list or NULL if none
coeur_node_v1* node_destroy_v1(coeur_node_v1* node) {
	if (node) {
		if (node->properties) free(node->properties);

		if (node->prev) {
			node->prev->next = node->next;
		}

		if (node->next) {
			node->next->prev = node->prev;
		}

		coeur_node_v1* child = node->firstChild;
		while (child) {
			child = node_destroy_v1(child);
		}

		child = node->next;
		free(node);

		return child;
	}

	return NULL;
}

node_token_v1* tokenize_file_buffer(coeur_file_v1* file) {
	file->nextNode = strchr(file->buffer, '<');
	node_token_v1* curr = NULL;

	while (file->nextNode) {
		file->nextNode = tokenize_next_in_buffer(curr, file);
	}

	return curr;
}

char* tokenize_next_in_buffer(node_token_v1* prev, coeur_file_v1* file) {
	if (file->nextNode == NULL) {
		assert(false && "Didn't set nextNode");
	}

	node_token_v1* token = (node_token_v1*)malloc(sizeof(node_token_v1));
	assert(token);
	memset(token, 0, sizeof(node_token_v1));

	token->prev = prev;
	if (prev->next) prev->next->prev = token;
	prev->next = token;

	token->closingTag = file->nextNode[1] == '/';
	
	token->type = 

	return strchr(file->nextNode + 1, '<');
}

coeur_node_type_t parse_node_type_v1(const char* typeName, size_t* oTagLen) {
	size_t len = 0;
	while (isalnum(typeName[len])) {
		len++;
	}

	*oTagLen = len;

	if (len == 0) return COEUR_NODE_UNKNOWN;

	size_t i = 0;
	while (i < COEUR_NODE_UNKNOWN) {
		size_t j = 0;
		for (;;) {
			if (tolower(typeName[j]) != _nodeTypeLookup[i].lowercaseName[j]) {
				break;
			}
			j++;
			if (j >= len) {
				return _nodeTypeLookup[i].typeIndex;
			}
		}
		i++;
	}

	return COEUR_NODE_UNKNOWN;
}

static struct node_type_entry _nodeTypeLookup[COEUR_NODE_UNKNOWN] = {
	{ "html", COEUR_NODE_HTML },
	{ "head", COEUR_NODE_HEAD },
	{ "body", COEUR_NODE_BODY },
	{ "title", COEUR_NODE_TITLE },
	{ "h1", COEUR_NODE_H1 },
};