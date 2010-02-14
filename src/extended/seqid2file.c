/*
  Copyright (c) 2007-2010 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2007-2008 Center for Bioinformatics, University of Hamburg

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

#include "extended/seqid2file.h"

void gt_seqid2file_options(GtOptionParser *op, GtStr *seqfile, bool *usedesc,
                           GtStr *regionmapping)
{
  GtOption *seqfile_option, *usedesc_option, *regionmapping_option;
  gt_assert(op && seqfile && usedesc && regionmapping);

  /* -seqfile */
  seqfile_option = gt_option_new_string("seqfile", "set the sequence file from "
                                        "which to extract the features",
                                        seqfile, NULL);
  gt_option_parser_add_option(op, seqfile_option);

  /* -usedesc */
  usedesc_option = gt_option_new_bool("usedesc", "use sequence descriptions to "
                                      "map the sequence IDs (in GFF3) to "
                                      "actual sequence entries.\nIf a "
                                      "description contains a sequence range "
                                      "(e.g., III:1000001..2000000), the first "
                                      " part is used as sequence ID ('III') "
                                      "and the first range as offset "
                                      "('1000001')", usedesc, false);
  gt_option_is_development_option(usedesc_option);
  gt_option_parser_add_option(op, usedesc_option);

  /* -regionmapping */
  regionmapping_option = gt_option_new_string("regionmapping", "set file "
                                              "containing sequence-region to "
                                              "sequence file mapping",
                                              regionmapping, NULL);
  gt_option_parser_add_option(op, regionmapping_option);

  /* either option -seqfile or -regionmapping is mandatory */
  gt_option_is_mandatory_either(seqfile_option, regionmapping_option);

  /* the options -seqfile and -regionmapping exclude each other */
  gt_option_exclude(seqfile_option, regionmapping_option);

  /* option -usedesc implies option -seqfile */
  gt_option_imply(usedesc_option, seqfile_option);
}

GtRegionMapping* gt_seqid2file_regionmapping_new(GtStr *seqfile, bool usedesc,
                                                 GtStr *regionmapping,
                                                 GtError *err)
{
  gt_error_check(err);
  gt_assert(seqfile && regionmapping);
  gt_assert(gt_str_length(seqfile) || gt_str_length(regionmapping));
  gt_assert(!(gt_str_length(seqfile) && gt_str_length(regionmapping)));
  /* create region mapping */
  if (gt_str_length(seqfile))
    return gt_region_mapping_new_seqfile(seqfile, usedesc);
  else
    return gt_region_mapping_new_mapping(regionmapping, err);
}
