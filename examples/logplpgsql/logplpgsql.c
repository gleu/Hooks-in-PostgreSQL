#include <stdio.h>
#include "postgres.h"
#include "plpgsql.h"

PG_MODULE_MAGIC;

void load_plugin( PLpgSQL_plugin * hooks );
static void my_func_beg(PLpgSQL_execstate * estate, PLpgSQL_function * func);

static PLpgSQL_plugin plugin_funcs = { my_func_beg };

void _PG_init( void )
{
	PLpgSQL_plugin ** var_ptr = (PLpgSQL_plugin **) find_rendezvous_variable("PLpgSQL_plugin");

	*var_ptr = &plugin_funcs;
}

void load_plugin( PLpgSQL_plugin * hooks )
{
	hooks->func_beg   = my_func_beg;
}

static void my_func_beg( PLpgSQL_execstate * estate, PLpgSQL_function * func )
{
	elog(LOG, "Execute function %s", func->fn_name);
}
