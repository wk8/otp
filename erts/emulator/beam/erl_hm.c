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

#include <Judy.h>

#define ERL_WANT_HIPE_BIF_WRAPPER__
#include "bif.h"
#undef ERL_WANT_HIPE_BIF_WRAPPER__

# include "erl_hm.h"

// TODO wkpo fix identation....

// TODO wkpo used?
static Eterm mkatom(const char *str)
{
    return am_atom_put(str, sys_strlen(str));
}

BIF_RETTYPE hm_new_0(BIF_ALIST_0) {
  Eterm* hp;
  hashmap_t *hm;

  hp = HAlloc(BIF_P, (HM_HEADER_SIZE + 2)); // TODO wkpo +1 ?

  hm = (hashmap_t*)hp;
  hm->thing_word = HEADER_HM;
  hm->value = mkatom("coucou"); // TODO wkpo

  BIF_RET(make_hm(hm));
}

BIF_RETTYPE hm_get_1(BIF_ALIST_1) {
  hashmap_t* hm = (hashmap_t*)hm_val(BIF_ARG_1);
  BIF_RET(hm->value);
}
// TODO wkpo
// hm:get(hm:new()).

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
Pvoid_t JArray[HASHSIZE] = { NULL }; // Declare static hash table

BIF_RETTYPE hm_wk_0(BIF_ALIST_0) {
  Eterm* hp;
  Eterm res;
  FloatDef a;

  Word_t Count;
  Word_t Index;
  Word_t *PValue;
  Word_t NumIndexes = 10000; // default first parameter
  if (argc > 1) NumIndexes = strtoul(argv[1], NULL, 0);
 // Load up the CPU cache for small measurements:
  for (Count = 0; Count < HASHSIZE; Count++) JArray[Count] = NULL;
 printf("Begin storing %lu random numbers in a Judy scalable hash array\n",
  NumIndexes);
  Index = INITN;
  STARTTm;
  for (Count = 0; Count < NumIndexes; Count++)
  {
  Index = Random(Index);
  JLI(PValue, JArray[Index % HASHSIZE], Index/HASHSIZE); 
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
   JLG(PValue, JArray[Index % HASHSIZE], Index/HASHSIZE);
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
