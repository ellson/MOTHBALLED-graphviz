#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include <stdbool.h>

#include "builddate.h"
#include "config.h"
#include "gvc.h"

static void redirect_all_std(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

lt_symlist_t lt_preloaded_symbols[] = { { 0, 0 } };
extern int GvExitOnUsage;

static char usage_info[] =
		"Usage: dot [-Vv?] [-(GNE)name=val] [-(KTlso)<val>] <dot files>\n"
		"(additional options for neato)    [-x] [-n<v>]\n"
		"(additional options for fdp)      [-L(gO)] [-L(nUCT)<val>]\n"
		"(additional options for memtest)  [-m<v>]\n"
		"(additional options for config)  [-cv]\n"
		"\n"
		" -V          - Print version and exit\n"
		" -v          - Enable verbose mode \n"
		" -Gname=val  - Set graph attribute 'name' to 'val'\n"
		" -Nname=val  - Set node attribute 'name' to 'val'\n"
		" -Ename=val  - Set edge attribute 'name' to 'val'\n"
		" -Tv         - Set output format to 'v'\n"
		" -Kv         - Set layout engine to 'v' (overrides default based on command name)\n"
		" -lv         - Use external library 'v'\n"
		" -ofile      - Write output to 'file'\n"
		" -O          - Automatically generate an output filename based on the input filename with a .'format' appended. (Causes all -ofile options to be ignored.) \n"
		" -P          - Internally generate a graph of the current plugins. \n"
		" -q[l]       - Set level of message suppression (=1)\n"
		" -s[v]       - Scale input by 'v' (=72)\n"
		" -y          - Invert y coordinate in output\n"
		"\n"
		" -n[v]       - No layout mode 'v' (=1)\n"
		" -x          - Reduce graph\n"
		"\n"
		" -Lg         - Don't use grid\n"
		" -LO         - Use old attractive force\n"
		" -Ln<i>      - Set number of iterations to i\n"
		" -LU<i>      - Set unscaled factor to i\n"
		" -LC<v>      - Set overlap expansion factor to v\n"
		" -LT[*]<v>   - Set temperature (temperature factor) to v\n"
		"\n"
		" -m          - Memory test (Observe no growth with top. Kill when done.)\n"
		" -m[v]       - Memory test - v iterations.\n"
		"\n"
		" -c          - Configure plugins (Writes $prefix/lib/graphviz/config \n"
		"               with available plugin information.  Needs write privilege.)\n"
		" -?          - Print usage and exit\n";

/**
 * Exit and output tests for `dot -V`
 */
Test(command_line, dash_V_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-V"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_V_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-V"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	sprintf(expected_stderr, "dot - graphviz version %s (%s)\n",
				PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}

/**
 * Exit and output tests for `dot -Vrandom`
 */
Test(command_line, dash_Vrandom_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-Vrandom"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_Vrandom_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-Vrandom"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	sprintf(expected_stderr, "dot - graphviz version %s (%s)\n",
				PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}

/**
 * Exit and output tests for `dot -randomV`
 */
Test(command_line, dash_randomV_exit,
		.init = redirect_all_std,
		.exit_code = 1)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-randomV"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_randomV_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-randomV"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[2000];
		sprintf(expected_stderr, "Error: dot: option -r unrecognized\n"
				"\n"
				"%s", usage_info);

	cr_assert_stderr_eq_str(expected_stderr);
}

/**
 * Exit and output tests for `dot -?V`
 */
Test(command_line, dash_questionmark_V_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-?V"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_questionmark_V_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-?V"};

	gvParseArgs(Gvc, argc, argv);

	fflush(stdout);
	cr_assert_stdout_eq_str(usage_info);
}

/**
 * Exit and output tests for `dot -V?`
 */
Test(command_line, dash_V_questionmark_exit,
		.init = redirect_all_std,
		.exit_code = 0)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 1;
	int argc = 2;
	char* argv[] = {"dot", "-V?"};

	gvParseArgs(Gvc, argc, argv);

	// Fail this test if the function above does not call exit.
	cr_assert(false);
}

Test(command_line, dash_V_questionmark_output,
		.init = redirect_all_std)
{
	GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
	GvExitOnUsage = 0;
	int argc = 2;
	char* argv[] = {"dot", "-V?"};

	gvParseArgs(Gvc, argc, argv);

	char expected_stderr[100];
	sprintf(expected_stderr, "dot - graphviz version %s (%s)\n",
				PACKAGE_VERSION, BUILDDATE);

	cr_assert_stderr_eq_str(expected_stderr);
}
