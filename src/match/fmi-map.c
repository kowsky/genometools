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

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "core/error.h"
#include "core/fa.h"
#include "core/array.h"
#include "core/str.h"
#include "sfx-ri-def.h"
#include "esa-fileend.h"
#include "fmindex.h"
#include "sarr-def.h"
#include "verbose-def.h"
#include "spacedef.h"
#include "stamp.h"

#include "opensfxfile.pr"
#include "esa-map.pr"
#include "fmi-keyval.pr"
#include "fmi-mapspec.pr"

bool fmindexexists(const GtStr *indexname)
{
  if (!indexfilealreadyexists(indexname,FMASCIIFILESUFFIX))
  {
    return false;
  }
  if (!indexfilealreadyexists(indexname,FMDATAFILESUFFIX))
  {
    return false;
  }
  return true;
}

static int scanfmafileviafileptr(Fmindex *fmindex,
                                 bool *storeindexpos,
                                 const GtStr *indexname,
                                 FILE *fpin,
                                 Verboseinfo *verboseinfo,
                                 GtError *err)
{
  bool haserr = false;
  GtArray *riktab;
  unsigned int intstoreindexpos;

  gt_error_check(err);
  riktab = gt_array_new(sizeofReadintkeys());
  SETREADINTKEYS("bwtlength",&fmindex->bwtlength,NULL);
  SETREADINTKEYS("longest",&fmindex->longestsuffixpos,NULL);
  SETREADINTKEYS("storeindexpos",&intstoreindexpos,NULL);
  SETREADINTKEYS("log2blocksize",&fmindex->log2bsize,NULL);
  SETREADINTKEYS("log2markdist",&fmindex->log2markdist,NULL);
  SETREADINTKEYS("specialcharacters",
                 &fmindex->specialcharinfo.specialcharacters,NULL);
  SETREADINTKEYS("specialranges",
                 &fmindex->specialcharinfo.specialranges,NULL);
  SETREADINTKEYS("realspecialranges",
                 &fmindex->specialcharinfo.realspecialranges,NULL);
  SETREADINTKEYS("lengthofspecialprefix",
                 &fmindex->specialcharinfo.lengthofspecialprefix,NULL);
  SETREADINTKEYS("lengthofspecialsuffix",
                 &fmindex->specialcharinfo.lengthofspecialsuffix,NULL);
  SETREADINTKEYS("suffixlength",&fmindex->suffixlength,NULL);
  if (!haserr)
  {
    GtStr *currentline;
    unsigned int linenum;

    currentline = gt_str_new();
    for (linenum = 0; gt_str_read_next_line(currentline, fpin) != EOF;
         linenum++)
    {
      if (analyzeuintline(indexname,
                         FMASCIIFILESUFFIX,
                         linenum,
                         gt_str_get(currentline),
                         gt_str_length(currentline),
                         riktab,
                         err) != 0)
      {
        haserr = true;
        break;
      }
      gt_str_reset(currentline);
    }
    gt_str_delete(currentline);
  }
  if (!haserr && allkeysdefined(indexname,FMASCIIFILESUFFIX,riktab,
                                verboseinfo,err) != 0)
  {
    haserr = true;
  }
  if (!haserr)
  {
    if (intstoreindexpos == 1U)
    {
      *storeindexpos = true;
    } else
    {
      if (intstoreindexpos == 0)
      {
        *storeindexpos = false;
      } else
      {
        gt_error_set(err,"illegal value in line matching \"storeindexpos=\"");
        haserr = true;
      }
    }
  }
  gt_array_delete(riktab);
  return haserr ? -1 : 0;
}

void freefmindex(Fmindex *fmindex)
{
  if (fmindex->mappedptr != NULL)
  {
    gt_fa_xmunmap(fmindex->mappedptr);
  }
  if (fmindex->bwtformatching != NULL)
  {
    freeEncodedsequence(&fmindex->bwtformatching);
  }
  if (fmindex->alphabet != NULL)
  {
    freeAlphabet(&fmindex->alphabet);
  }
}

static Encodedsequence *mapbwtencoding(const GtStr *indexname,
                                       Verboseinfo *verboseinfo,
                                       GtError *err)
{
  Suffixarray suffixarray;
  bool haserr = false;
  Seqpos totallength;

  gt_error_check(err);
  if (mapsuffixarray(&suffixarray,&totallength,SARR_ESQTAB,indexname,
                     verboseinfo,err) != 0)
  {
    haserr = true;
  }
  freeAlphabet(&suffixarray.alpha);
  gt_strarray_delete(suffixarray.filenametab);
  FREESPACE(suffixarray.filelengthtab);
  if (haserr)
  {
    freeEncodedsequence(&suffixarray.encseq);
    return NULL;
  }
  return suffixarray.encseq;
}

int mapfmindex (Fmindex *fmindex,const GtStr *indexname,
                Verboseinfo *verboseinfo,GtError *err)
{
  FILE *fpin = NULL;
  bool haserr = false, storeindexpos = true;

  gt_error_check(err);
  fmindex->mappedptr = NULL;
  fmindex->bwtformatching = NULL;
  fmindex->alphabet = NULL;
  fpin = opensfxfile(indexname,FMASCIIFILESUFFIX,"rb",err);
  if (fpin == NULL)
  {
    haserr = true;
  }
  if (!haserr)
  {
    if (scanfmafileviafileptr(fmindex,
                             &storeindexpos,
                             indexname,
                             fpin,
                             verboseinfo,
                             err) != 0)
    {
      haserr = true;
    }
  }
  gt_fa_xfclose(fpin);
  if (!haserr)
  {
    fmindex->bwtformatching = mapbwtencoding(indexname,verboseinfo,err);
    if (fmindex->bwtformatching == NULL)
    {
      haserr = true;
    }
  }
  if (!haserr)
  {
    GtStr *tmpfilename;

    fmindex->specpos.nextfreePairBwtidx
      = (unsigned long) determinenumberofspecialstostore(
                                          &fmindex->specialcharinfo);
    fmindex->specpos.spacePairBwtidx = NULL;
    fmindex->specpos.allocatedPairBwtidx = 0;
    tmpfilename = gt_str_clone(indexname);
    gt_str_append_cstr(tmpfilename,ALPHABETFILESUFFIX);
    fmindex->alphabet = assigninputalphabet(false,
                                            false,
                                            tmpfilename,
                                            NULL,
                                            err);
    if (fmindex->alphabet == NULL)
    {
      haserr = true;
    }
    gt_str_delete(tmpfilename);
  }
  if (!haserr)
  {
    GtStr *tmpfilename;

    computefmkeyvalues (fmindex,
                        fmindex->bwtlength,
                        fmindex->log2bsize,
                        fmindex->log2markdist,
                        getmapsizeAlphabet(fmindex->alphabet),
                        fmindex->suffixlength,
                        storeindexpos,
                        &fmindex->specialcharinfo);
    tmpfilename = gt_str_clone(indexname);
    gt_str_append_cstr(tmpfilename,FMDATAFILESUFFIX);
    if (fillfmmapspecstartptr(fmindex,storeindexpos,tmpfilename,err) != 0)
    {
      haserr = true;
    }
    gt_str_delete(tmpfilename);
  }
  if (haserr)
  {
    freefmindex(fmindex);
  }
  return haserr ? -1 : 0;
}
