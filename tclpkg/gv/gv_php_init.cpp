#include <php.h>
#include <gvc.h>
#include <gvplugin.h>
#include <gvcjob.h>
#include <gvcint.h>

static size_t gv_php_writer (const char *s, int len)
{
	return PHPWRITE(s, len);
}

void gv_binding_init ( GVC_t *gvc)
{
	gvc->write_fn = gv_php_writer;
}
