require 'rbconfig'

CONFIG = Config::MAKEFILE_CONFIG


case ARGV[0]
when "archdir"
    puts Config::expand(CONFIG["archdir"])
when "lib"
    puts Config::expand(CONFIG["libdir"])
when "vendorarchdir"
    puts Config::expand(CONFIG["vendorarchdir"])
when "sitelib"
    puts Config::expand(CONFIG["sitedir"])
end

