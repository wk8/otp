#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "sys.h"
#include "erl_vm.h"
#include "global.h"
#include "erl_process.h"
#include "error.h"
#include "bif.h"
#include "erl_binary.h"
#include "erl_map.h"

#include <Judy.h>

#define ERL_WANT_HIPE_BIF_WRAPPER__
#include "bif.h"
#undef ERL_WANT_HIPE_BIF_WRAPPER__

# include "erl_hm.h"

#define WK_DEBUG_MODE 1

#ifndef WK_DEBUG_MODE_LOADED
#define WK_DEBUG_MODE_LOADED

#if WK_DEBUG_MODE

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define WK_DEBUG_LOG_FILE "/tmp/wk_debug.log"

void wk_debug(const char *format, ...)
{
	FILE *f ;
	va_list args;
	struct timeval tv;

	f = fopen(WK_DEBUG_LOG_FILE, "a+");

  // OR
  // char filename[200];
  // sprintf(filename, "/tmp/wk_debug-%d.log", (int) getpid());
  // f = fopen(filename, "a+");

	gettimeofday(&tv, NULL);
	fprintf(f, "[ %lu-%lu (%d) ] ", (unsigned long)tv.tv_sec, (unsigned long)tv.tv_usec, (int) getpid());
	va_start(args, format);
	vfprintf(f, format, args);
	fprintf(f, "\n");
	va_end(args);
	fclose(f);
}

#define WK_DEBUG(format, ...) wk_debug(format, ##__VA_ARGS__)

#else
#define WK_DEBUG(format, ...)
#endif
#endif

// TODO wkpo fix identation....

// TODO wkpo used?
static Eterm mkatom(const char *str)
{
    return am_atom_put(str, sys_strlen(str));
}

BIF_RETTYPE hm_new_0(BIF_ALIST_0) {
  Eterm* hp;
  hashmap_t *hm;

  hp = HAlloc(BIF_P, (HM_HEADER_SIZE + 3)); // TODO wkpo +1 ?

  hm = (hashmap_t*)hp;
  hm->thing_word = HEADER_HM;
  hm->j_array = (Pvoid_t) NULL;
  WK_DEBUG("apres creation %d", hm->j_array);

  BIF_RET(make_hm(hm));
}

BIF_RETTYPE hm_set_3(BIF_ALIST_3) {
  hashmap_t* hm = (hashmap_t*)hm_val(BIF_ARG_3);
  Eterm value = BIF_ARG_2;
  Eterm* p_value = &value;

  // TODO wkpo
  Sint i = signed_val(value);
  WK_DEBUG("on va set value %d", i);
  Sint* pi;

  Uint32 hash = 12; // TODO wkpo hashmap_make_hash(BIF_ARG_1);
  // TODO wkpo ca marche pas cette histoire la, collisions?
  WK_DEBUG("avant set %d", hm->j_array);
  JLI(pi, hm->j_array, hash);
  WK_DEBUG("apres set %d", hm->j_array);

  BIF_RET(BIF_ARG_3);
}

BIF_RETTYPE hm_get_2(BIF_ALIST_2) {
  Sint* pi;
  hashmap_t* hm;

  hm = (hashmap_t*)hm_val(BIF_ARG_2);
  WK_DEBUG("avant get %d", hm->j_array);

  Uint32 hash = 12; // TODO wkpo hashmap_make_hash(BIF_ARG_1);
  JLG(pi, hm->j_array, hash);
  WK_DEBUG("apres get %d", hm->j_array);

  // TODO wkpo NEXT: theorie: en fait le pointer vers l'arg est bon que la durée du call... regarder comment les flatmaps font? est-ce qu'on peut copier?
  // TODO wkpo
  WK_DEBUG("apres get value %d", *pi);

  BIF_RET(make_small(*pi));
}
// TODO wkpo
// hm:get(wk, hm:set(wk, 28, hm:new())).

//// Below: timing for Judy

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

struct timeval TBeg, TEnd;
#define STARTTm gettimeofday(&TBeg, NULL)
#define ENDTm gettimeofday(&TEnd, NULL)
#define DeltaUSec \
 ( ((double)TEnd.tv_sec * 1000000.0 + (double)TEnd.tv_usec) \
 - ((double)TBeg.tv_sec * 1000000.0 + (double)TBeg.tv_usec) )
// End of timing routines ========================================
// Define Hash table size if not in compile line ===================
// Set HASHSIZE 1 for straight Judy
#ifndef HASHSIZE
#define HASHSIZE (1 << 8) // hash table size 256
#endif
// Seed for pseudo-random counter ==================================
#define INITN 123456 // first Index to store
static uint32_t // Placed here for INLINE possibility
Random(uint32_t Seed) // produce 2^32 -1 numbers by different counting
{
 if ((int32_t)Seed < 0) { Seed += Seed; Seed ^= 16611; }
 else { Seed += Seed; }
 return(Seed);
}
// Hash Table ======================================================
Pvoid_t JArray = (Pvoid_t) NULL;

BIF_RETTYPE hm_wk_0(BIF_ALIST_0) {
  Eterm* hp;
  Eterm res;
  FloatDef a;

  Word_t Count;
  Word_t Index;
  Word_t *PValue;
  Word_t NumIndexes = 10000; // default first parameter
 // Load up the CPU cache for small measurements:
 printf("Begin storing %lu random numbers in a Judy scalable hash array\n",
  NumIndexes);
  Index = INITN;
  STARTTm;
  for (Count = 0; Count < NumIndexes; Count++)
  {
  Index = Random(Index);
  JLI(PValue, JArray, Index); 
  *PValue += 1; // bump count of duplicate Indexes
   }
   ENDTm;
  printf("Insertion of %lu indexes took %6.3f microseconds per index\n",
   NumIndexes, DeltaUSec/NumIndexes);
   Index = INITN; // start the same number sequence over
   STARTTm;
   for (Count = 0; Count < NumIndexes; Count++)
   {
   Index = Random(Index);
   JLG(PValue, JArray, Index);
   if (*PValue != 1)
  printf("%lu dups of %lu\n", *PValue - 1, Index);
   }
   ENDTm;
  printf("Retrieval of %lu indexes took %6.3f microseconds per index\n",
   NumIndexes, DeltaUSec/NumIndexes);

  hp = HAlloc(BIF_P, FLOAT_SIZE_OBJECT);
  res = make_float(hp);

  a.fd = DeltaUSec/NumIndexes;
  PUT_DOUBLE(a, hp);

  BIF_RET(res);
}
