#!/usr/bin/perl

use gv;

$g = gv::digraph "G";
print $g, "\n";
print gv::set($g, "aaa", "xxx"), "\n";
print gv::set($g, "aaa"), "\n";
$n = gv::node $g, "hello";
print $n, "\n";
print gv::set($n,"label"), "\n";
print gv::set($n, "aaa", "xxx"), "\n";
print gv::set($n, "aaa"), "\n";
$m = gv::node $g, "world";
print $m, "\n";
print gv::set($m, "aaa"), "\n";
$e = gv::edge $n, $m;
print $e, "\n";
print gv::set($e, "aaa", "xxx"), "\n";
print gv::set($e, "aaa"), "\n";
gv::rm $e;
gv::rm $n;
gv::rm $m;
gv::rm $g;

$g = gv::stringgraph "digraph G {a->b}";
print $g, "\n";
gv::rm $g;

$g = gv::readgraph "hello.dot";
print $g, "\n";
gv::layout($g, "dot");
gv::writegraph($g, "hello_perl.png", "png");
gv::rm $g;
