#!/usr/bin/php
<?
#FIXME  - I don't know how to extend php's paths.  Needs:
#   ln -s /usr/lib64/graphviz/php/libgv_php.so /usr/lib64/php/modules/gv.so
#   ln -s /usr/lib64/graphviz/php/php/gv.php /usr/share/php/gv.php

include("gv.php");

$g = gv::digraph("G");

print gv::setv($g, "aaa", "xxx");
print "\n";
print gv::getv($g, "aaa");
print "\n";
$n = gv::node($g, "hello");
print gv::getv($n,"label");
print "\n";
print gv::setv($n, "aaa", "xxx");
print "\n";
print gv::getv($n, "aaa");
print "\n";
$m = gv::node($g, "world");
print gv::getv($m, "aaa");
print "\n";
$e = gv::edge($n, $m);
print gv::setv($e, "aaa", "xxx");
print "\n";
print gv::getv($e, "aaa");
print "\n";
gv::rm($e);
gv::rm($n);
gv::rm($m);
gv::rm($g);

$g = gv::readstring("digraph G {a->b}");
gv::layout($g, "dot");
gv::render($g, "png", "ab.png");
gv::rm($g);

$g = gv::read("hello.gv");
gv::layout($g, "dot");
gv::render($g, "png", "hello.png");
gv::rm($g);

?>
