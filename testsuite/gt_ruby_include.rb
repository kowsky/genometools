Name "gtruby: genome_stream bindings (output stream)"
Keywords "gt_ruby"
Test do
  run_ruby "#{$testdata}/gtruby/gff3.rb #{$testdata}/gff3_file_1_short.txt"
  run "env LC_ALL=C sort #{$last_stdout}"
  run "diff #{$last_stdout} #{$testdata}gff3_file_1_short_sorted.txt"
end

Name "gtruby: genome_visitor bindings (output stream)"
Keywords "gt_ruby"
Test do
  run_ruby "#{$testdata}/gtruby/genome_visitor.rb #{$testdata}/gff3_file_1_short.txt"
  run "env LC_ALL=C sort #{$last_stdout}"
  run "diff #{$last_stdout} #{$testdata}gff3_file_1_short_sorted.txt"
end

if $arguments["libgtview"] then
  Name "gtruby: feature_index and feature_stream bindings"
  Keywords "gt_ruby"
  Test do
    run_ruby "#{$testdata}/gtruby/feature_stuff.rb #{$testdata}"
    run "env LC_ALL=C sort #{$last_stdout}"
    run "grep -v '^##sequence-region' #{$testdata}gff3_file_1_short_sorted.txt | diff #{$last_stdout} -"
  end

  Name "gtruby: libgtview bindings"
  Keywords "gt_ruby"
  Test do
    run_ruby "#{$testdata}/gtruby/view.rb test.png #{$testdata}gff3_file_1_short.txt"
  end

  Name "gtruby: show_seqids"
  Keywords "gt_ruby"
  Test do
    run_ruby "#{$testdata}/gtruby/show_seqids.rb #{$testdata}encode_known_genes_Mar07.gff3"
    run "diff #{$last_stdout} #{$testdata}encode_known_genes_Mar07.seqids"
  end
end
