#include "coeur_node.h"

#include "coeur_file.h"
#include "coeur_common.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static const char* nodeTokenNames[NODE_TOKEN_UNKNOWN + 1] = {
	"",
	"raw",
	"tag_start",
	"tag_end",
	"tag_prop",
	"string",
	"unknown"
};

#define min(_x, _y) ((_x) > (_y) ? (_y) : (_x))

void __debugPrintList(node_token_v1* tokenList) {
	for (node_token_v1* curr = tokenList; curr != NULL; curr = curr->next) {
		char tmp[32];
		tmp[0] = '\0';
		if (curr->content) {
			strncpy_s(tmp, 32, curr->content, min(31, curr->contentLen));
		}
		if (curr->contentLen >= 31) {
			tmp[31] = '\0';
			tmp[30] = tmp[29] = tmp[28] = '.';
		}
		printf("%s: %llu \"%s\" %s\n", nodeTokenNames[curr->type], curr->contentLen, &tmp[0], curr->closer ? "true" : "false");
	}
}

// PRIVATE METHODS

coeur_node_v1* node_parse_from_buffer(coeur_buffer_v1* buffer) {
	// Tokenize the file
	node_token_v1* head = tokenize_file_buffer(buffer);
	if (!head) return NULL;

	__debugPrintList(head);

	coeur_node_v1* node = node_construct_tree(head);
	token_list_free(head);
	return node;
}

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

#define PEEK_NT(_token) ((_token)->next) && (_token)->next

node_token_v1* _node_parse_property(coeur_node_prop_v1* prop, node_token_v1* token) {
	assert(token && token->type == NODE_TOKEN_TAG_PROP);

	prop->propName = token->content;
	prop->propNameLen = token->contentLen;

	if (PEEK_NT(token)->type == NODE_TOKEN_STRING) {
		token = token->next;
		prop->propVal = token->content;
		prop->propValLen = token->contentLen;
	}

	return token->next;
}

node_token_v1* _node_construct(coeur_node_v1* prevNode, node_token_v1* currToken) {
	if (currToken->type != NODE_TOKEN_TAG_START) return _node_construct(prevNode, currToken->next);
	coeur_node_v1* currNode = node_create_v1();

	prevNode->next = currNode;
	currNode->prev = prevNode;

	currNode->openTag = currToken->content;
	currNode->type = node_parse_type(&currToken->content[1], NULL);
	currNode->openTagLen = currToken->contentLen;
	currToken = currToken->next;
	if (!currToken) { 
		setLastError("Invalid html tag at end");
		return currToken; 
	}

	node_token_v1* currProp = currToken;
	while (currToken->type == NODE_TOKEN_TAG_PROP) {
		currToken = currToken->next;
		currNode->propertiesLen++;
	}

	if (currNode->propertiesLen > 0) {
		currNode->properties = (coeur_node_prop_v1*)calloc(currNode->propertiesLen, sizeof(coeur_node_prop_v1));
		assert(currNode->properties);
		for (size_t i = 0; i < currNode->propertiesLen; ++i) {
			currProp = _node_parse_property(&currNode->properties[i], currProp);
		}
		assert(currProp == currToken);
	}

	if (currToken->type != NODE_TOKEN_TAG_END) {
		setLastError("Invalid html tag at end");
		return currToken;
	}

	currNode->openTagLen = currToken->content - currNode->openTag;
	currNode->selfClose

	return currToken;
}

coeur_node_v1* node_construct_tree(node_token_v1* head) {
	coeur_node_v1* root = node_create_v1();

	node_token_v1* front = head;
	/*node_token_v1* back = head;
	for (back = head; back->next != NULL; back = back->next);*/

	// TODO: try a recursive strategy
	node_token_v1* out = _node_construct(root, front); // consumes tokens and can call self
	assert(out == NULL);
	
	return root;
}

node_token_v1* tokenize_file_buffer(coeur_buffer_v1* buffer) {
	assert(buffer);
	size_t index = 0;
	bool inTag = false, inName = false, inProp = false;
	bool inString = false;
	ZeroAlloc(node_token_v1, head);
	node_token_v1* curr = head;
	head->type = NODE_TOKEN_UNKNOWN;

	while (index < buffer->contentLen && buffer->content[index]) {
		char c = buffer->content[index];

		if (inString) {
			if (c == '\"') {
				inString = false;
			}

			curr->contentLen++;
			index++;
			continue;
		}

		switch (c) {
		case '<': {
			assert(!inTag);
			inTag = inName = true;
			inProp = false;
			ZeroAlloc(node_token_v1, tagStart);
			tagStart->type = NODE_TOKEN_TAG_START;
			tagStart->content = &buffer->content[index];
			tagStart->contentLen = 1;
			tagStart->prev = curr;
			curr->next = tagStart;
			curr = tagStart;
		} break;
		case '>': {
			inTag = inName = false;
			ZeroAlloc(node_token_v1, tagEnd);
			tagEnd->type = NODE_TOKEN_TAG_END;
			tagEnd->prev = curr;
			curr->next = tagEnd;
			curr = tagEnd;
		} break;
		case '\"': {
			inString = true;
			inProp = false;
			ZeroAlloc(node_token_v1, stringToken);
			stringToken->type = NODE_TOKEN_STRING;
			stringToken->content = &buffer->content[index];
			stringToken->contentLen = 1;
			stringToken->prev = curr;
			curr->next = stringToken;
			curr = stringToken;
		} break;
		default: {
			if (inTag) {
				if (inName) {
					if (isalnum(c))
						curr->contentLen++;
					else if (isspace(c))
						inName = false;
					else if (c == '/') {
						curr->closer = true;
						curr->contentLen++;
					} else {
						curr->contentLen++;
					}
				} else if (inProp) {
					curr->contentLen++;
					break;
				} else if (c == '/') {
					if (index + 1 < buffer->contentLen && buffer->content[index + 1] == '>') {
						inTag = inName = false;
						ZeroAlloc(node_token_v1, tagEnd);
						tagEnd->type = NODE_TOKEN_TAG_END;
						tagEnd->closer = true;
						tagEnd->prev = curr;
						curr->next = tagEnd;
						curr = tagEnd;
						index++;
						break;
					} else {
						setLastError("malformed '/' in html tag");
						token_list_free(head);
						return NULL;
					}
				} else {
					inProp = true;
					ZeroAlloc(node_token_v1, propToken);
					propToken->type = NODE_TOKEN_TAG_PROP;
					propToken->content = &buffer->content[index];
					propToken->contentLen = 1;
					propToken->prev = curr;
					curr->next = propToken;
					curr = propToken;
					break;
				}
			} else {
				if (curr->type != NODE_TOKEN_RAW) {
					inTag = inName = inString = inProp = false;
					ZeroAlloc(node_token_v1, rawToken);
					rawToken->type = NODE_TOKEN_RAW;
					rawToken->content = &buffer->content[index];
					rawToken->contentLen = 1;
					rawToken->prev = curr;
					curr->next = rawToken;
					curr = rawToken;
				} else {
					curr->contentLen++;
				}
			}
		} break;
		}

		index++;
	}

	if (inTag || inString || inName) {
		setLastError("Unclosed html tag or string detected");
		token_list_free(head);
		return NULL;
	}

	return head;
}

void token_list_free(node_token_v1* head) {
	while (head != NULL) {
		node_token_v1* next = head->next;
		free(head);
		head = next;
	}
}

coeur_node_type_t node_parse_type(const char* typeName, size_t* oTagLen) {
	size_t len = 0;
	while (isalnum(typeName[len])) {
		len++;
	}

	if(oTagLen) *oTagLen = len;

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