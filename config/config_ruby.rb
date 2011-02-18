require 'rbconfig'

CONFIG = Config::MAKEFILE_CONFIG


case ARGV[0]
when "INCLUDES"
    puts Config::expand(CONFIG["archdir"])
when "lib"
    puts Config::expand(CONFIG["libdir"])
when "archsitelib"
    puts Config::expand(CONFIG["sitearchdir"])
when "sitelib"
    puts Config::expand(CONFIG["sitedir"])
end

