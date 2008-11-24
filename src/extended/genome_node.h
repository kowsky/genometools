/*
  Copyright (c) 2006-2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2006-2008 Center for Bioinformatics, University of Hamburg

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

#ifndef GENOME_NODE_H
#define GENOME_NODE_H

#include "core/bittab.h"
#include "core/phase_api.h"
#include "core/range.h"
#include "core/str.h"
#include "extended/genome_node_api.h"
#include "extended/node_visitor.h"

void          gt_genome_node_set_origin(GtGenomeNode*, GtStr *filename,
                                        unsigned int line_number);
void*         gt_genome_node_cast(const GtGenomeNodeClass*, GtGenomeNode*);
void*         gt_genome_node_try_cast(const GtGenomeNodeClass*, GtGenomeNode*);
const char*   gt_genome_node_get_filename(const GtGenomeNode*);
unsigned int  gt_genome_node_get_line_number(const GtGenomeNode*);
/* used to sort nodes */
GtStr*        gt_genome_node_get_idstr(GtGenomeNode*);
void          gt_genome_node_change_seqid(GtGenomeNode*, GtStr*);
int           gt_genome_node_accept(GtGenomeNode*, GtNodeVisitor*, GtError*);
int           gt_genome_node_cmp(GtGenomeNode*, GtGenomeNode*);
int           gt_genome_node_compare(GtGenomeNode**, GtGenomeNode**);
int           gt_genome_node_compare_with_data(GtGenomeNode**, GtGenomeNode**,
                                               void *unused);
/* <delta> has to point to a variable of type unsigned long */
int           gt_genome_node_compare_delta(GtGenomeNode**, GtGenomeNode**,
                                           void *delta);

void          gt_genome_nodes_sort(GtArray*);
void          gt_genome_nodes_sort_stable(GtArray*);
bool          gt_genome_nodes_are_equal_region_nodes(GtGenomeNode*,
                                                     GtGenomeNode*);
bool          gt_genome_nodes_are_sorted(const GtArray*);

#endif
