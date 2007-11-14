#!/usr/bin/php
<?
#FIXME  - I don't know how to extend php's paths.  Needs:
#   ln -s /usr/lib64/graphviz/php/libgv_php.so /usr/lib64/php/modules/gv.so
#   ln -s /usr/lib64/graphviz/php/php/gv.php /usr/share/php/gv.php

include("gv.php");

$g = digraph("G");

print setv($g, "aaa", "xxx");
print "\n";
print getv($g, "aaa");
print "\n";
$n = node($g, "hello");
print getv($n,"label");
print "\n";
print setv($n, "aaa", "xxx");
print "\n";
print getv($n, "aaa");
print "\n";
$m = node($g, "world");
print getv($m, "aaa");
print "\n";
$e = edge($n, $m);
print setv($e, "aaa", "xxx");
print "\n";
print getv($e, "aaa");
print "\n";
rm($e);
rm($n);
rm($m);
rm($g);

$g = readstring("digraph G {a->b}");
layout($g, "dot");
render($g, "png", "ab.png");
rm($g);

$g = read("hello.dot");
layout($g, "dot");
render($g, "png", "hello.png");
rm($g);

?>
