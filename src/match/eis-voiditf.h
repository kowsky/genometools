/*
  Copyright (c) 2007 Stefan Kurtz <kurtz@zbh.uni-hamburg.de>
  Copyright (c) 2007 Center for Bioinformatics, University of Hamburg

  Permission to use, copy, modify, and distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef EIS_VOIDITF_H
#define EIS_VOIDITF_H

#include "core/unused_api.h"
#include "core/encseq_api.h"

#include "splititv.h"
#include "procmatch.h"

/* The following type is just used to stronlgy type the functions
   using the FMindex (rather than using void pointers) */

typedef struct FMindex FMindex;

unsigned long gt_bwtseqfirstmatch(const FMindex *fmindex, unsigned long bound);

/** Iterator for positions defined by a lower and upper bound
 */

typedef struct Bwtseqpositioniterator Bwtseqpositioniterator;

Bwtseqpositioniterator *gt_Bwtseqpositioniterator_new(const FMindex *fmindex,
                                                      unsigned long lowerbound,
                                                      unsigned long upperbound);

bool gt_Bwtseqpositioniterator_next(unsigned long *pos,
                                    Bwtseqpositioniterator *bspi);

bool gt_BwtseqpositionwithoutSEPiterator_next(unsigned long *pos,
                                              Bwtseqpositioniterator *bspi);

GtUchar gt_bwtseqgetsymbol(unsigned long bound,const FMindex *fmindex);

void gt_Bwtseqpositioniterator_delete(Bwtseqpositioniterator *bspi);

typedef struct BwtSeqpositionextractor BwtSeqpositionextractor;

BwtSeqpositionextractor *gt_newBwtSeqpositionextractor(
                                                     const FMindex *voidbwtseq,
                                                     unsigned long upperbound);

unsigned long gt_BwtSeqpositionextractor_extract(BwtSeqpositionextractor *bspex,
                                                 unsigned long lowerbound);

void gt_freeBwtSeqpositionextractor(BwtSeqpositionextractor *bspex);

typedef struct Bwtseqcontextiterator Bwtseqcontextiterator;

Bwtseqcontextiterator *gt_Bwtseqcontextiterator_new(const FMindex *fmindex,
                                                    unsigned long bound);

GtUchar gt_Bwtseqcontextiterator_next(unsigned long *bound,
                                      Bwtseqcontextiterator *bsci);

void gt_Bwtseqcontextiterator_delete(Bwtseqcontextiterator *bsci);

void gt_bwtrangesplitwithoutspecial(GtArrayBoundswithchar *bwci,
                                    unsigned long *rangeOccs,
                                    const FMindex *fmindex,
                                    unsigned long lbound,
                                    unsigned long ubound);

FMindex *gt_loadvoidBWTSeqForSA(const char *indexname,
                                bool withpckbt,
                                GtError *err);

void gt_deletevoidBWTSeq(FMindex *packedindex);

/* the parameter is const void *, as this is required by the other
   indexed based methods */

unsigned long gt_voidpackedindexuniqueforward(const void *fmindex,
                                              GT_UNUSED unsigned long offset,
                                              GT_UNUSED unsigned long left,
                                              GT_UNUSED unsigned long right,
                                              GT_UNUSED unsigned long
                                                        *witnessposition,
                                              const GtUchar *qstart,
                                              const GtUchar *qend);

/* the parameter is const void *, as this is required by the other
   indexed based methods */

unsigned long gt_voidpackedindexmstatsforward(const void *fmindex,
                                              GT_UNUSED unsigned long offset,
                                              GT_UNUSED unsigned long left,
                                              GT_UNUSED unsigned long right,
                                              unsigned long *witnessposition,
                                              const GtUchar *qstart,
                                              const GtUchar *qend);

bool gt_pck_exactpatternmatching(const FMindex *fmindex,
                                 const GtUchar *pattern,
                                 unsigned long patternlength,
                                 unsigned long totallength,
                                 const GtUchar *dbsubstring,
                                 Processmatch processmatch,
                                 void *processmatchinfo);

unsigned long gt_voidpackedfindfirstmatchconvert(const FMindex *fmindex,
                                                 unsigned long witnessbound,
                                                 unsigned long matchlength);

typedef struct
{
  unsigned long lowerbound, upperbound;
} Mbtab;

unsigned long gt_bwtrangesplitallwithoutspecial(Mbtab *mbtab,
                                                unsigned long *rangeOccs,
                                                const FMindex *fmindex,
                                                unsigned long lbound,
                                                unsigned long ubound);

unsigned int gt_bwtseq2maxdepth(const FMindex *fmindex);

const Mbtab **gt_bwtseq2mbtab(const FMindex *fmindex);

unsigned long gt_bwtrangesplitallwithspecial(Mbtab *mbtab,
                                             unsigned long *rangeOccs,
                                             const FMindex *voidBwtSeq,
                                             unsigned long lbound,
                                             unsigned long ubound);

unsigned int gt_bwtseq2maxdepth(const FMindex *voidbwtseq);

unsigned long gt_voidpackedindex_totallength_get(const FMindex *fmindex);

#endif
