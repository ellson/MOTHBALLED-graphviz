require 'rbconfig'

CONFIG = Config::MAKEFILE_CONFIG


case ARGV[0]
when "archdir"
    puts Config::expand(Config::CONFIG["archdir"])
when "sitearchdir"
    puts Config::expand(Config::CONFIG["sitearchdir"])
when "includedir"
    puts Config::expand(Config::CONFIG["includedir"])
when "libdir"
    puts Config::expand(Config::CONFIG["libdir"])
when "vendorarchdir"
    puts Config::expand(Config::CONFIG["vendorarchdir"])
when "sitedir"
    puts Config::expand(Config::CONFIG["sitedir"])
end

