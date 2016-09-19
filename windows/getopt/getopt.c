#include <stdio.h>
#include <string.h>

#define ERR(s, c)	if(opterr) fprintf (stderr, "%s%s'%c'\n", argv[0], s, c)

char *optarg;

int	optind = 1;
int	optopt;
int	opterr = 1;


int getopt(int argc, char *const *argv, const char *opts)
{
    static int sp = 1;
    register int c;
    register char *cp;

    if (sp == 1) {
        if (optind >= argc ||
            argv[optind][0] != '-' || argv[optind][1] == '\0')
            return(EOF);
        else if (strcmp(argv[optind], "--") == 0) {
            optind++;
            return(EOF);
        }
    }

    optopt = c = argv[optind][sp];
    if (c == ':' || (cp = strchr(opts, c)) == 0) {
        ERR(": illegal option -- ", c);
        if (argv[optind][++sp] == '\0') {
            optind++;
            sp = 1;
        }
        return('?');
    }
    if (*++cp == ':') {
        if (argv[optind][sp + 1] != '\0')
            optarg = &argv[optind++][sp + 1];
        else if (++optind >= argc) {
            ERR(": option requires an argument -- ", c);
            sp = 1;
            return('?');
        }
        else
            optarg = argv[optind++];
        sp = 1;
    }
    else {
        if (argv[optind][++sp] == '\0') {
            sp = 1;
            optind++;
        }
        optarg = 0;
    }
    return(c);
}
