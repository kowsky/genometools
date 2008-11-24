/*
  Copyright (c) 2008 Sascha Steinbiss <steinbiss@zbh.uni-hamburg.de>
  Copyright (c) 2008 Center for Bioinformatics, University of Hamburg

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

#ifndef CUSTOM_TRACK_GC_CONTENT_API_H
#define CUSTOM_TRACK_GC_CONTENT_API_H

#include "annotationsketch/custom_track_api.h"
#include "core/seq.h"

/* Implements the GtCustomTrack interface. This custom track draws a plot of
   the GC content of a given sequence in the displayed range. As a window
   size for GC content calculation, <windowsize> is used. */
typedef struct GtCustomTrackGcContent GtCustomTrackGcContent;

GtCustomTrack* gt_custom_track_gc_content_new(GtSeq *seq,
                                              unsigned long windowsize,
                                              unsigned long height,
                                              double avg,
                                              bool show_scale);
#endif
