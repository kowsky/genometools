/*
  Copyright (c) 2007 Sascha Steinbiss <ssteinbiss@stud.zbh.uni-hamburg.de>
  Copyright (c) 2007 Malte Mader <mmader@stud.zbh.uni-hamburg.de>
  Copyright (c) 2007 Christin Schaerfer <cschaerfer@stud.zbh.uni-hamburg.de>
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

#include <string.h>
#include <cairo.h>
#include "libgtcore/cstr.h"
#include "libgtcore/fileutils.h"
#include "libgtcore/gtdatapath.h"
#include "libgtcore/option.h"
#include "libgtcore/splitter.h"
#include "libgtcore/undef.h"
#include "libgtcore/versionfunc.h"
#include "libgtext/add_introns_stream.h"
#include "libgtext/gff3_in_stream.h"
#include "libgtext/gff3_out_stream.h"
#include "libgtview/canvas.h"
#include "libgtview/config.h"
#include "libgtview/diagram.h"
#include "libgtview/feature_index.h"
#include "libgtview/feature_stream.h"
#include "libgtview/gt_view.h"
#include "libgtview/image_info.h"

typedef struct {
  bool pipe,
       verbose,
       addintrons,
       showrecmaps;
  Str *seqid, *format;
  unsigned long start,
                end;
  unsigned int width;
} Gff3_view_arguments;

static OPrval parse_options(int *parsed_args, Gff3_view_arguments *arguments,
                            int argc, const char **argv, Error *err)
{
  OptionParser *op;
  Option  *option, *option2;
  OPrval oprval;
  bool force;
  static const char *formats[] = { "png",
#ifdef CAIRO_HAS_PDF_SURFACE
  "pdf",
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
  "svg",
#endif
#ifdef CAIRO_HAS_PS_SURFACE
  "ps",
#endif
  NULL };
  error_check(err);

  /* init */
  op = option_parser_new("[option ...] image_file [GFF3_file ...]",
                         "Create graphical representations of "
                         "GFF3 annotation files.");

  /* -v */
  option = option_new_verbose(&arguments->verbose);
  option_parser_add_option(op, option);

  /* -pipe */
  option = option_new_bool("pipe", "use pipe mode (i.e., show all gff3 "
                           "features on stdout)", &arguments->pipe, false);
  option_parser_add_option(op, option);

  /* -force */
  option = option_new_bool("force", "force writing to output file", &force,
                           false);
  option_parser_add_option(op, option);

  /* -seqid */
  option = option_new_string("seqid", "sequence region identifier\n"
                                      "default: first one in file",
                            arguments->seqid, NULL);
  option_parser_add_option(op, option);
  option_hide_default(option);

  /* -start */
  option = option_new_ulong_min("start", "start position\n"
                                         "default: first region start",
                            &arguments->start, UNDEF_ULONG, 1);
  option_parser_add_option(op, option);
  option_hide_default(option);

  /* -end */
  option2 = option_new_ulong("end", "end position\ndefault: last region end",
                            &arguments->end, UNDEF_ULONG);
  option_parser_add_option(op, option2);
  /* -start and -end must be given together */
  option_imply(option, option2);
  option_imply(option2, option);
  option_hide_default(option2);

  /* -width */
  option = option_new_uint_min("width", "target image width", &arguments->width,
                               800, 1);
  option_parser_add_option(op, option);

  /* -format */
  option = option_new_choice("format", "output graphics format\n"
                                       "choose from png"
#ifdef CAIRO_HAS_PDF_SURFACE
                                       "|pdf"
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
                                       "|svg"
#endif
#ifdef CAIRO_HAS_PS_SURFACE
                                       "|ps"
#endif
                                       "",
                             arguments->format, formats[0], formats);
  option_parser_add_option(op, option);

  /* -addintrons */
  option = option_new_bool("addintrons", "add intron features between "
                           "existing exon features (before drawing)",
                           &arguments->addintrons, false);
  option_parser_add_option(op, option);

  /* -showrecmaps */
  option = option_new_bool("showrecmaps", "show RecMaps after image creation",
                           &arguments->showrecmaps, false);
  option_is_development_option(option);
  option_parser_add_option(op, option);

  /* set contact mailaddress */
  option_parser_set_mailaddress(op, "<ssteinbiss@stud.zbh.uni-hamburg.de>");

  /* parse options */
  option_parser_set_min_args(op, 1);
  oprval = option_parser_parse(op, parsed_args, argc, argv, versionfunc, err);

  if (oprval == OPTIONPARSER_OK && !force && file_exists(argv[*parsed_args])) {
    error_set(err, "file \"%s\" exists already. use option -force to "
                   "overwrite", argv[*parsed_args]);
    oprval = OPTIONPARSER_ERROR;
  }

  /* free */
  option_parser_delete(op);

  return oprval;
}

int gt_view(int argc, const char **argv, Error *err)
{
  GenomeStream *gff3_in_stream = NULL,
               *add_introns_stream = NULL,
               *gff3_out_stream = NULL,
               *feature_stream = NULL,
               *last_stream;
  Gff3_view_arguments arguments;
  GenomeNode *gn = NULL;
  FeatureIndex *features = NULL;
  int parsed_args, had_err=0;
  const char *file, *seqid = NULL;
  Range qry_range, sequence_region_range;
  Array *results = NULL;
  Config *cfg = NULL;
  Str *config_file = NULL;
  Str *prog;
  Diagram *d = NULL;
  ImageInfo* ii = NULL;
  Canvas *canvas = NULL;

  error_check(err);

  /* option parsing */
  arguments.seqid = str_new();
  arguments.format = str_new();
  switch (parse_options(&parsed_args, &arguments, argc, argv, err)) {
    case OPTIONPARSER_OK: break;
    case OPTIONPARSER_ERROR:
      str_delete(arguments.seqid);
      str_delete(arguments.format);
      return -1;
    case OPTIONPARSER_REQUESTS_EXIT:
      str_delete(arguments.seqid);
      str_delete(arguments.format);
      return 0;
  }

  /* save name of output file */
  file = argv[parsed_args];

  /* check for correct order: range end < range start */
  if (!had_err &&
      arguments.start != UNDEF_ULONG &&
      arguments.end != UNDEF_ULONG &&
      !(arguments.start < arguments.end)) {
    error_set(err, "start of query range (%lu) must be before "
                   "end of query range (%lu)",
              arguments.start, arguments.end);
    had_err = -1;
  }

  if (!had_err) {
    /* create feature index */
    features = feature_index_new();
    parsed_args++;
    do {
      /* create a gff3 input stream */
      gff3_in_stream = gff3_in_stream_new_sorted(argv[parsed_args],
                                                 arguments.verbose);
      last_stream = gff3_in_stream;

      /* create add introns stream if -addintrons was used */
      if (arguments.addintrons) {
        add_introns_stream = add_introns_stream_new(last_stream);
        last_stream = add_introns_stream;
      }

      /* create gff3 output stream if -pipe was used */
      if (arguments.pipe) {
        gff3_out_stream = gff3_out_stream_new(last_stream, NULL);
        last_stream = gff3_out_stream;
      }

      /* create feature stream */
      feature_stream = feature_stream_new(last_stream, features);

      /* pull the features through the stream and free them afterwards */
      while (!(had_err = genome_stream_next_tree(feature_stream, &gn, err)) &&
             gn) {
        genome_node_rec_delete(gn);
      }

      genome_stream_delete(feature_stream);
      genome_stream_delete(gff3_out_stream);
      genome_stream_delete(add_introns_stream);
      genome_stream_delete(gff3_in_stream);

      if (!argv[parsed_args]) /* no GFF3 file was given at all */
        break;
      parsed_args++;
    } while (!had_err && argv[parsed_args]);
  }

  /* if seqid is empty, take first one added to index */
  if (!had_err && strcmp(str_get(arguments.seqid),"") == 0) {
    seqid = feature_index_get_first_seqid(features);
    if (seqid == NULL) {
      error_set(err, "GFF input file must contain a sequence region!");
      had_err = -1;
    }
  }
  else if (!had_err && !feature_index_has_seqid(features,
                                                str_get(arguments.seqid))) {
    error_set(err, "sequence region '%s' does not exist in GFF input file",
              str_get(arguments.seqid));
    had_err = -1;
  }
  else if (!had_err)
    seqid = str_get(arguments.seqid);

  results = array_new(sizeof (GenomeNode*));
  if (!had_err) {
    sequence_region_range = feature_index_get_range_for_seqid(features, seqid);
    qry_range.start = (arguments.start == UNDEF_ULONG ?
                         sequence_region_range.start :
                         arguments.start);
    qry_range.end   = (arguments.end == UNDEF_ULONG ?
                         sequence_region_range.end :
                         arguments.end);
  }

  if (!had_err) {
    if (arguments.verbose)
      fprintf(stderr, "# of results: %lu\n", array_size(results));

    /* find and load configuration file */
    prog = str_new();
    str_append_cstr_nt(prog, argv[0], cstr_length_up_to_char(argv[0], ' '));
    config_file = gtdata_get_path(str_get(prog), err);
    str_delete(prog);
    str_append_cstr(config_file, "/config/view.lua");
    if (!(cfg = config_new(arguments.verbose, err)))
      had_err = -1;
    if (!had_err && file_exists(str_get(config_file)))
      had_err = config_load_file(cfg, config_file, err);
  }

  if (!had_err) {
    /* create and write image file */
    d = diagram_new(features, seqid, &qry_range, cfg);
    ii = image_info_new();
    if (strcmp(str_get(arguments.format),"pdf")==0)
      canvas = canvas_new(cfg, GRAPHICS_PDF, arguments.width, ii);
    else if (strcmp(str_get(arguments.format),"ps")==0)
      canvas = canvas_new(cfg, GRAPHICS_PS, arguments.width, ii);
    else if (strcmp(str_get(arguments.format),"svg")==0)
      canvas = canvas_new(cfg, GRAPHICS_SVG, arguments.width, ii);
    else
      canvas = canvas_new(cfg, GRAPHICS_PNG, arguments.width, ii);
    diagram_render(d, canvas);
    if (arguments.showrecmaps) {
      unsigned long i;
      RecMap *rm;
      for (i = 0; i < image_info_num_of_recmaps(ii) ;i++) {
        GenomeFeatureType *type;
        char buf[BUFSIZ];
        rm = image_info_get_recmap(ii, i);
        recmap_format_html_imagemap_coords(rm, buf, BUFSIZ);
        type = genome_feature_get_type((GenomeFeature*) rm->gn);
        printf("%s, %s\n", buf, genome_feature_type_get_cstr(type));
      }
    }
    had_err = canvas_to_file(canvas, file, err);
  }

  /* free */
  canvas_delete(canvas);
  image_info_delete(ii);
  config_delete(cfg);
  str_delete(config_file);
  diagram_delete(d);
  str_delete(arguments.seqid);
  str_delete(arguments.format);
  array_delete(results);
  feature_index_delete(features);

  return had_err;
}
