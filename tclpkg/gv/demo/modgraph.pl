#!/usr/bin/perl -w
# #!/usr/local/bin/perl -w
# Change ^^ to the version of Perl you installed the SWIG modules / Graphviz with
#
# Change this to point to your installed graphviz lib dir
#   Normally either /usr/local/lib/graphviz/perl or /usr/lib/graphviz/perl
#use lib '/home/maxb/lib/graphviz/perl';
use gv;

use Getopt::Long;
GetOptions(\%Args, 'h|help','d|debug');
$Debug   = $Args{d} || 0;
$Modules = shift @ARGV || '/proc/modules';

die &usage if $Args{h};
die "Cannot read $Modules. $!\n" unless (-r $Modules);

$G = gv::digraph("G");
$N = gv::protonode($G);
$E = gv::protoedge($G);

gv::setv($G, "rankdir", "LR");
gv::setv($G, "nodesep", "0.05");
gv::setv($N, "shape", "box");
gv::setv($N, "width", "0");
gv::setv($N, "height", "0");
gv::setv($N, "margin", ".03");
gv::setv($N, "fontsize", "8");
gv::setv($N, "fontname", "helvetica");
gv::setv($E, "arrowsize", ".4");

open (M,"<$Modules") or die "Can't open $Modules. $!\n";
while (<M>) {
    chomp;
    # parport                36832   1 (autoclean) [parport_pc lp]
    my @f = split(/\s+/);
    # Should be at least three columns
    next unless scalar @f >= 3;

    my $module  = shift @f;
    my $size    = shift @f;
    my $used_by = shift @f;
    my $deps    = join (' ',@f);

    Debug("$module");
    my $n = gv::node($G,$module);

    # look for and get rid of brackets.  ignore parens, etc.
    if ($deps =~ s/\[(.*)\]/$1/) {
        foreach my $dep (split(/\s+/,$deps)) {
            Debug(" $dep -> $module");
            gv::edge($n, gv::node($G, $dep) );
        }
    }
}

gv::layout($G, "dot");
gv::render($G, "xlib");

sub Debug {
    return unless $Debug;
    warn join(" ",@_), "\n";
}

sub usage {
    return << "end_usage";
modgraph.pl

Displays Linux kernel module dependencies from $Modules

Author:    John Ellson <ellson\@research.att.com>
Perl Port: Max Baker <max\@warped.org>

Usage: $0 [--debug] [/proc/modules]

end_usage
}
