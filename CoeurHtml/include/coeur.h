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

typedef struct coeur_object* CoeurObject;
COEURC_API void coeur_destroy(CoeurObject object);

typedef uint8_t coeur_file_flags_t;
COEURC_API CoeurObject coeur_open(const char* filename, coeur_file_flags_t flags);

typedef uint8_t coeur_url_flags_t;
COEURC_API CoeurObject coeur_create_url(const char* url, coeur_url_flags_t flags);

typedef uint8_t coeur_parse_flags_t;
COEURC_API CoeurObject coeur_parse_file(CoeurObject file, coeur_parse_flags_t flags);
COEURC_API CoeurObject coeur_parse_from_url(CoeurObject url, coeur_parse_flags_t flags);

//COEURC_API const char* coeur_get_file_contents(CoeurFile file);

#endif
