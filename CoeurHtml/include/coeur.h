#ifndef COEUR_H
#define COEUR_H

#ifdef __cplusplus
#define COEUR_C extern "C"
#else
#define COEUR_C
#endif

#ifdef _WIN32
#ifdef COEURHTML_EXPORTS
#define COEUR_API __declspec(dllexport)
#else
#define COEUR_API __declspec(dllimport)
#endif
#define COEURC_API COEUR_C COEUR_API
#else
#define COEUR_API 
#define COEURC_API COEUR_C
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

COEURC_API const char* coeur_get_last_error(void);

typedef struct coeur_file_v1* CoeurFile;
typedef uint8_t coeur_file_flags_t;
COEURC_API CoeurFile coeur_open(const char* filename, coeur_file_flags_t flags);
COEURC_API void coeur_close(CoeurFile file);

typedef struct coeur_url_v1* CoeurUrl;
typedef uint8_t coeur_url_flags_t;
COEURC_API CoeurUrl coeur_create_url(const char* url, coeur_url_flags_t flags);
COEURC_API void coeur_destroy_url(CoeurUrl url);

typedef struct coeur_node_v1* CoeurNode;
typedef uint8_t coeur_parse_flags_t;
COEURC_API CoeurNode coeur_parse_file(CoeurFile file, coeur_parse_flags_t flags);
COEURC_API CoeurNode coeur_parse_from_url(CoeurUrl url, coeur_parse_flags_t flags);
COEURC_API void coeur_free_node(CoeurNode node);

//COEURC_API const char* coeur_get_file_contents(CoeurFile file);

#endif
