#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Altrep.h>
#include <R_ext/Itermacros.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "tdigest.h"

// next 2 ƒ() via <https://github.com/randy3k/xptr/blob/master/src/xptr.c>

void check_is_xptr(SEXP s) {
  if (TYPEOF(s) != EXTPTRSXP) {
    error("expected an externalptr");
  }
}

SEXP is_null_xptr_(SEXP s) {
  check_is_xptr(s);
  return Rf_ScalarLogical(R_ExternalPtrAddr(s) == NULL);
}

static void td_finalizer(SEXP ptr) {
  if(!R_ExternalPtrAddr(ptr)) return;
  td_free((td_histogram_t *)R_ExternalPtrAddr(ptr));
  R_ClearExternalPtr(ptr); /* not really needed */
}

SEXP Rtd_create(SEXP compression) {
  SEXP ptr;
  td_histogram_t *t = td_new(asReal(compression));
  if (t) {
    ptr = R_MakeExternalPtr(t, install("tdigest"), R_NilValue);
    R_RegisterCFinalizerEx(ptr, td_finalizer, TRUE);
    setAttrib(ptr, install("class"), mkString("tdigest"));
    return(ptr);
  } else {
    return(R_NilValue);
  }
}

SEXP Rtdig(SEXP vec, SEXP compression) {
  SEXP ptr;
  td_histogram_t *t = td_new(asReal(compression));
  if (t) {
    R_xlen_t n = Rf_xlength(vec);
    if (ALTREP(vec)) {
      ITERATE_BY_REGION(vec, x, i, nbatch, double, REAL, {
        for (R_xlen_t k = 0; k < nbatch; k++) {
          if (!ISNAN(x[k])) td_add(t, x[k], 1);
        }
      });
    } else {
      for (R_xlen_t i = 0; i < n; i++) {
        if (!ISNAN(REAL(vec)[i])) td_add(t, REAL(vec)[i], 1);
      }
    }
    ptr = R_MakeExternalPtr(t, install("tdigest"), R_NilValue);
    R_RegisterCFinalizerEx(ptr, td_finalizer, TRUE);
    setAttrib(ptr, install("class"), mkString("tdigest"));
    return(ptr);
  } else {
    return(R_NilValue);
  }
}

SEXP Rtquant(SEXP tdig, SEXP probs) {
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(tdig);
  if (t) {
    R_xlen_t n = xlength(probs);
    SEXP out = PROTECT(allocVector(REALSXP, n));
    double *o = REAL(out);
    for (R_xlen_t i = 0; i < n; i++) {
      o[i] = td_value_at(t, REAL(probs)[i]);
    }
    UNPROTECT(1);
    return(out);
  }
  return(R_NilValue);
}

SEXP Rtd_add(SEXP tdig, SEXP val, SEXP count) {
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(tdig);
  if (t) {
    td_add(t, asReal(val), asReal(count)); //void td_add(td_histogram_t *h, double val, double count);
    return(tdig);
  }
  return(R_NilValue);
}

SEXP Rtd_value_at(SEXP tdig, SEXP q) {
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(tdig);
  if (t) {
    return(ScalarReal(td_value_at(t, asReal(q)))); //double td_value_at(td_histogram_t *h, double q);
  }
  return(R_NilValue);
}

SEXP Rtd_quantile_of(SEXP tdig, SEXP val) {
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(tdig);
  if (t) {
    return(ScalarReal(td_quantile_of(t, asReal(val)))); //double td_quantile_of(td_histogram_t *h, double val);
  }
  return(R_NilValue);
}

SEXP Rtd_total_count(SEXP tdig) {
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(tdig);
  if (t) {
    return(ScalarReal(td_total_count(t))); //double td_total_count(td_histogram_t *h);
  }
  return(R_NilValue);
}

SEXP Rtd_merge(SEXP from, SEXP into) {
  td_histogram_t *f = (td_histogram_t *)R_ExternalPtrAddr(from);
  td_histogram_t *t = (td_histogram_t *)R_ExternalPtrAddr(into);
  if (t) {
    td_merge(t, f);
    return(into); //void td_merge(td_histogram_t *into, td_histogram_t *from);
  }
  return(R_NilValue);
}
