/*
  Copyright (c) 2007      Christin Schaerfer <cschaerfer@zbh.uni-hamburg.de>
  Copyright (c)      2008 Sascha Steinbiss <ssteinbiss@zbh.uni-hamburg.de>
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

#include <string.h>
#include "libgtcore/array.h"
#include "libgtcore/ensure.h"
#include "libgtcore/ma.h"
#include "libgtcore/strand.h"
#include "libgtext/feature_type_factory_builtin.h"
#include "libgtext/genome_feature.h"
#include "libgtext/genome_feature_type.h"
#include "libgtview/element.h"

struct Element {
  GenomeFeatureType *type;
  Strand strand;
  GenomeNode *gn;
  Range range;
  bool mark;
};

Element* element_new(GenomeNode *gn)
{
  Element *element;
  GenomeFeature *gf = (GenomeFeature*) gn;
  assert(gn);
  element = element_new_empty();
  element_set_type(element, genome_feature_get_type(gf));
  element_set_range(element, genome_node_get_range(gn));
  element->strand = genome_feature_get_strand(gf);
  element->mark = genome_node_is_marked(gn);
  element->gn = genome_node_ref(gn);
  return element;
}

Element* element_new_empty(void)
{
  return ma_calloc(1, sizeof (Element));
}

Range element_get_range(const Element *element)
{
  assert(element);
  return element->range;
}

void element_set_range(Element *element, Range r)
{
  assert(element);
  element->range = r;
}

GenomeFeatureType* element_get_type(const Element *element)
{
  assert(element);
  return element->type;
}

void element_set_type(Element *element, GenomeFeatureType *type)
{
  assert(element);
  element->type = type;
}

Strand element_get_strand(const Element *element)
{
  assert(element);
  return element->strand;
}
bool element_is_marked(const Element *element)
{
  assert(element);
  return element->mark;
}

bool elements_are_equal(const Element *e1, const Element *e2)
{
  assert(e1 && e2);
  if (e1->type == e2->type && !range_compare(e1->range, e2->range))
    return true;
  return false;
}

int element_render(Element *elem, Canvas *canvas)
{
  int had_err = 0;
  assert(elem && canvas);
  canvas_visit_element(canvas, elem);
  return had_err;
}

GenomeNode* element_get_node_ref(const Element *elem)
{
  assert(elem);
  return elem->gn;
}

int element_unit_test(Error *err)
{
  FeatureTypeFactory *feature_type_factory;
  GenomeFeatureType *type;
  Range r1, r2, r_temp;
  GenomeNode *gn, *gn2;
  Element *e, *e2, *e3;
  int had_err = 0;
  error_check(err);

  feature_type_factory = feature_type_factory_builtin_new();

  r1.start = 10UL;
  r1.end = 50UL;

  r2.start = 20UL;
  r2.end = 50UL;

  type = feature_type_factory_create_gft(feature_type_factory, gft_exon);
  gn = genome_feature_new(type, r1, STRAND_BOTH, NULL, 0);
  gn2 = genome_feature_new(type, r2, STRAND_BOTH, NULL, 0);

  e = element_new(gn);
  e2 = element_new(gn);
  e3 = element_new(gn2);

  /* tests element_get_range */
  r_temp = element_get_range(e);
  ensure(had_err, (0 == range_compare(r1, r_temp)));
  ensure(had_err, (1 == range_compare(r2, r_temp)));

  /* tests element_get_type and element_set_type*/
  ensure(had_err, (type == element_get_type(e)));
  type = feature_type_factory_create_gft(feature_type_factory, gft_intron);
  ensure(had_err, (type != element_get_type(e)));
  element_set_type(e, type);
  ensure(had_err, (type == element_get_type(e)));
  element_set_type(e2, type);

  /* tests elements_are_equal */
  ensure(had_err, elements_are_equal(e, e2));
  ensure(had_err, !elements_are_equal(e, e3));
  ensure(had_err, !elements_are_equal(e2, e3));

  element_delete(e);
  element_delete(e2);
  element_delete(e3);
  genome_node_delete(gn);
  genome_node_delete(gn2);
  feature_type_factory_delete(feature_type_factory);

  return had_err;

}

void element_delete(Element *element)
{
  if (!element) return;
  if (element->gn)
    genome_node_delete(element->gn);
  ma_free(element);
}
