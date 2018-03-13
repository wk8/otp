#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "erl_process.h"
#define ERL_WANT_HIPE_BIF_WRAPPER__
#include "bif.h"
#undef ERL_WANT_HIPE_BIF_WRAPPER__

BIF_RETTYPE hm_new_0(BIF_ALIST_2){
  Eterm result = "42";
  BIF_RET(result);
}
