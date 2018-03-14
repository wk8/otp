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

BIF_RETTYPE hm_wk_0(BIF_ALIST_0) {
  Eterm* hp;
  Eterm res;
  FloatDef a;

  hp = HAlloc(BIF_P, FLOAT_SIZE_OBJECT);
  res = make_float(hp);

  a.fd = 0.28;
  PUT_DOUBLE(a, hp);

  BIF_RET(res);
}
