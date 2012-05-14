/*-------------------------------------------------------------------------
 *
 * pg_log_superuser.c
 *		Log statement according to the user.
 *
 *
 * Copyright (c) 2011-2012, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/
#include "postgres.h"

#include <unistd.h>

#include "funcapi.h"
#include "miscadmin.h"
#include "tcop/utility.h"

PG_MODULE_MAGIC;


/*---- Local variables ----*/

/* Saved hook values in case of unload */
static ExecutorEnd_hook_type prev_ExecutorEnd = NULL;


/*---- Function declarations ----*/

void		_PG_init(void);
void		_PG_fini(void);

static void my_ExecutorEnd(QueryDesc *queryDesc);
static void my_log(const char *query);


/*
 * Module load callback
 */
void
_PG_init(void)
{
	/*
	 * Install hooks.
	 */
	prev_ExecutorEnd = ExecutorEnd_hook;
	ExecutorEnd_hook = my_ExecutorEnd;
}

/*
 * Module unload callback
 */
void
_PG_fini(void)
{
	/* Uninstall hooks. */
	ExecutorEnd_hook = prev_ExecutorEnd;
}

/*
 * ExecutorEnd hook: log if needed
 */
static void
my_ExecutorEnd(QueryDesc *queryDesc)
{
    my_log(queryDesc->sourceText);

	if (prev_ExecutorEnd)
		prev_ExecutorEnd(queryDesc);
	else
		standard_ExecutorEnd(queryDesc);
}

/*
 * Log statement according to the user that launched the statement.
 */
static void
my_log(const char *query)
{
	Assert(query != NULL);

    if (superuser())
        elog(LOG, "**SUPERUSER QUERY** %s: %s", GetUserNameFromId(GetUserId()), query);
}

