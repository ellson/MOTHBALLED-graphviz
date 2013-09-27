#!/usr/bin/tclsh

set DOT ../cmd/dot/dot
set DIFF diff
set DIFFIMG ../contrib/diffimg/diffimg
set TAR tar

set graphdir test_graphs
set resultdir test_results
set referencedir test_reference

file delete -force $graphdir $resultdir
file mkdir $graphdir $resultdir

set FAIL 0
set dodiff 0
if {$argc && [string equal [lindex $argv 0] reference]} {
  file delete -force $referencedir
  file mkdir $referencedir
  set resultdir $referencedir
} {
  if {[file exists $referencedir.tgz]} {
    exec $TAR xfz $referencedir.tgz
    set dodiff 1
  }
}

set state -1
set testnum 0
set subtestnum 0

set pass 0
set fail 0

set fn TESTS
set f [open $fn r]
while {![eof $f]} {
  set rec [gets $f]
  if {[string length $rec] == 0} {
    continue
  }
  if {[string first "#" $rec] == 0} {
    continue
  }
  if {[string first "\t" $rec] != 0} {
    set name $rec
    set state 0
    incr testnum
    set subtestnum 0
    continue
  }
  if {$state == -1} {
    puts stderr "The first non-comment record in the TESTS file"
    puts stderr "must be the name of a test without leading whitespace"
    exit
  }
  set rec [string trim $rec]
  if {$state == 0} {
    set graph $rec
    set state 1
  } {
    set command $rec
    set layout dot
    set format xdot
    regexp {\-K([a-z0-9]+)} $rec . layout
    regexp {\-T([a-z0-9]+)} $rec . format
    incr subtestnum
    set id [format %03d.%03d $testnum $subtestnum]
    set src $graphdir/$id.gv
    set ref $referencedir/$id.$format
    set res $resultdir/$id.$format
    set testf [open $graphdir/$id.gv w]
    puts $testf "// $id: $name"
    puts $testf "// $command"
    puts $testf $graph
    close $testf

    # execute dot
    switch $format {
      ps - svg {
	#FIXME - remove version string in output
	set rc [catch {exec $DOT -K$layout -T$format $src -o $res}]
      }
      default {
	set rc [catch {exec $DOT -K$layout -T$format $src -o $res}]
      }
    }
    if {! $dodiff} {
	continue
    }
    if {$rc} {
	incr FAIL
        puts "$id: \"$name\" CRASHED"
	continue
    }

    # execute diff
    switch $format {
      png {
	# can't use diff because PNG format includes a tIME record which changes
	#   diffimg produces an image which highlights the differences
        set rc [catch {exec $DIFFIMG $ref $res > $res.diff.png}]
      }
      default {
        set rc [catch {exec $DIFF -u $ref $res > $res.diff}]
      }
    }
    if {$rc} {
      incr fail
      puts "$id: \"$name\" FAILED"
    } {
      incr pass
      foreach fn [glob $src $res $res.*] {
        file delete $fn
      }
    }
  }
}
close $f

if {$dodiff} {
  set total [expr {$pass+$fail}]
  puts ""
  puts "Summary: PASS $pass : FAIL $fail : TOTAL $total"
} {
  exec $TAR cfz $referencedir.tgz $referencedir
}
