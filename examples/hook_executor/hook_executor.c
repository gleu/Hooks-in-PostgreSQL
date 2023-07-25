/*-------------------------------------------------------------------------
 *
 * hook_executor.c
 *		TESTS
 *
 *
 * Copyright (c) 2011-2013, Guillaume Lelarge (Dalibo),
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
static ExecutorStart_hook_type  prev_ExecutorStart = NULL;
static ExecutorRun_hook_type    prev_ExecutorRun = NULL;
static ExecutorFinish_hook_type prev_ExecutorFinish = NULL;
static ExecutorEnd_hook_type    prev_ExecutorEnd = NULL;


/*---- Function declarations ----*/

void		_PG_init(void);
void		_PG_fini(void);

static void my_ExecutorStart(QueryDesc *queryDesc, int eflags);
static void my_ExecutorRun(QueryDesc *queryDesc, ScanDirection direction, uint64 count, bool execute_once);
static void my_ExecutorFinish(QueryDesc *queryDesc);
static void my_ExecutorEnd(QueryDesc *queryDesc);


/*
 * Module load callback
 */
void
_PG_init(void)
{
	/*
	 * Install hooks.
	 */
	prev_ExecutorStart = ExecutorStart_hook;
	ExecutorStart_hook = my_ExecutorStart;
	prev_ExecutorRun = ExecutorRun_hook;
	ExecutorRun_hook = my_ExecutorRun;
	prev_ExecutorFinish = ExecutorFinish_hook;
	ExecutorFinish_hook = my_ExecutorFinish;
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
	ExecutorStart_hook = prev_ExecutorStart;
	ExecutorRun_hook = prev_ExecutorRun;
	ExecutorFinish_hook = prev_ExecutorFinish;
	ExecutorEnd_hook = prev_ExecutorEnd;
}

/*
 * ExecutorStart hook: log if needed
 */
static void
my_ExecutorStart(QueryDesc *queryDesc, int eflags)
{
    elog(LOG, "**hook_executor** Start %d (%s)",
		MyProcPid,
		IsBackgroundWorker ? "worker" : "leader");

	if (prev_ExecutorStart)
		prev_ExecutorStart(queryDesc, eflags);
	else
		standard_ExecutorStart(queryDesc, eflags);
}

/*
 * ExecutorRun hook: log if needed
 */
static void
my_ExecutorRun(QueryDesc *queryDesc, ScanDirection direction, uint64 count, bool execute_once)
{
    elog(LOG, "**hook_executor** Run %d (%s)",
		MyProcPid,
		IsBackgroundWorker ? "worker" : "leader");

	if (prev_ExecutorRun)
		prev_ExecutorRun(queryDesc, direction, count, execute_once);
	else
		standard_ExecutorRun(queryDesc, direction, count, execute_once);
}

/*
 * ExecutorFinish hook: log if needed
 */
static void
my_ExecutorFinish(QueryDesc *queryDesc)
{
    elog(LOG, "**hook_executor** Finish %d (%s)",
		MyProcPid,
		IsBackgroundWorker ? "worker" : "leader");

	if (prev_ExecutorFinish)
		prev_ExecutorFinish(queryDesc);
	else
		standard_ExecutorFinish(queryDesc);
}

/*
 * ExecutorEnd hook: log if needed
 */
static void
my_ExecutorEnd(QueryDesc *queryDesc)
{
    elog(LOG, "**hook_executor** End %d (%s)",
		MyProcPid,
		IsBackgroundWorker ? "worker" : "leader");

	if (prev_ExecutorEnd)
		prev_ExecutorEnd(queryDesc);
	else
		standard_ExecutorEnd(queryDesc);
}
