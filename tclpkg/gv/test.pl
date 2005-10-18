#!/usr/bin/perl

use gv;

$g = gv::digraph "G";
print gv::set($g, "aaa", "xxx"), "\n";
print gv::set($g, "aaa"), "\n";
$n = gv::node $g, "hello";
print gv::set($n,"label"), "\n";
print gv::set($n, "aaa", "xxx"), "\n";
print gv::set($n, "aaa"), "\n";
$m = gv::node $g, "world";
print gv::set($m, "aaa"), "\n";
$e = gv::edge $n, $m;
print gv::set($e, "aaa", "xxx"), "\n";
print gv::set($e, "aaa"), "\n";
gv::rm $e;
gv::rm $n;
gv::rm $m;
gv::rm $g;

$g = gv::readstring "digraph G {a->b}";
gv::rm $g;

$g = gv::read "hello.dot";
gv::layout($g, "dot");
gv::render($g, "png", "hello.png");
gv::rm $g;
