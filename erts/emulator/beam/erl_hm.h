#ifndef __ERL_HASHMAP_H__
#define __ERL_HASHMAP_H__

typedef struct hashmap_s {
    Eterm thing_word;
    Eterm value;
} hashmap_t;

#define HASHMAP_HEADER_SZ  (sizeof(hashmap_t) / sizeof(Eterm))
#define HEADER_HASHMAP	_make_header(HASHMAP_HEADER_SZ, _TAG_HEADER_HASHMAP)

#endif
