#!/usr/bin/perl

use gv;

$g = gv::digraph "G";
print gv::setv($g, "aaa", "xxx"), "\n";
print gv::getv($g, "aaa"), "\n";
$n = gv::node $g, "hello";
print gv::getv($n,"label"), "\n";
print gv::setv($n, "aaa", "xxx"), "\n";
print gv::getv($n, "aaa"), "\n";
$m = gv::node $g, "world";
print gv::getv($m, "aaa"), "\n";
$e = gv::edge $n, $m;
print gv::setv($e, "aaa", "xxx"), "\n";
print gv::getv($e, "aaa"), "\n";
gv::rm $e;
gv::rm $n;
gv::rm $m;
gv::rm $g;

$g = gv::readstring "digraph G {a->b}";
gv::rm $g;

$g = gv::read "hello.gv";
gv::layout($g, "dot");
gv::render($g, "png", "hello.png");
gv::rm $g;
