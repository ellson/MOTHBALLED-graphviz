use Config;

if ($ARGV[0] eq "PERL_LIBS") {
	$archlib = $Config{archlib};
	$libperl = $Config{libperl};
	$libperl =~ s/lib([^\.]+).*/$1/;
	print "-L$archlib/CORE -l$libperl";
}
if ($ARGV[0] eq "PERL_INCLUDES") {
	$archlib = $Config{archlib};
	print "-I$archlib/CORE";
}
if ($ARGV[0] eq "PERL_INSTALL_DIR") {
	print "$Config{vendorarch}"
}
