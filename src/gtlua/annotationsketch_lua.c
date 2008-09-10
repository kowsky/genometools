/*
  Copyright (c) 2007-2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
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

#include <assert.h>
#include "gtlua/canvas_lua.h"
#include "gtlua/diagram_lua.h"
#include "gtlua/feature_index_lua.h"
#include "gtlua/feature_stream_lua.h"
#include "gtlua/feature_visitor_lua.h"
#include "gtlua/image_info_lua.h"
#include "gtlua/annotationsketch_lua.h"

int gt_lua_open_annotationsketch(lua_State *L)
{
  assert(L);
  luaopen_canvas(L);
  luaopen_diagram(L);
  luaopen_feature_index(L);
  luaopen_feature_stream(L);
  luaopen_feature_visitor(L);
  luaopen_imageinfo(L);
  return 1;
}
