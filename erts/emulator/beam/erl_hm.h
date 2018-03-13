#ifndef __ERL_HM_H__
#define __ERL_HM_H__

typedef struct hashmap_s {
    Eterm thing_word;
    Eterm value;
} hashmap_t;

#define HM_HEADER_SIZE  (sizeof(hashmap_t) / sizeof(Eterm))
#define HEADER_HM	_make_header(HM_HEADER_SIZE, _TAG_HEADER_HM)

#endif
