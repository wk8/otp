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

BIF_RETTYPE hm_new_0(BIF_ALIST_0){
  Eterm* hp;
  hashmap_t *hm;

  hp = HAlloc(BIF_P, (HASHMAP_HEADER_SZ + 1));

  hm = (hashmap_t*)hp;
  hm->thing_word = HEADER_HASHMAP;
  // hm->value = NULL; // TODO wkpo

  BIF_RET(make_hashmap(hm));
}
