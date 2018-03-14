#ifndef __ERL_HM_H__
#define __ERL_HM_H__

#include <Judy.h>

typedef struct hashmap_s {
    Eterm thing_word;
    Pvoid_t j_array;
} hashmap_t;

#define HM_HEADER_SIZE  ((sizeof(hashmap_t) / sizeof(Eterm)) + 3) // TODO wkpo +1 ?
#define HEADER_HM	_make_header(HM_HEADER_SIZE, _TAG_HEADER_HM)

#endif
