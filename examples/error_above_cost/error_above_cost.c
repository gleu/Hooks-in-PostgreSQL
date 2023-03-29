/*-------------------------------------------------------------------------
 *
 * error_above_cost.c
 *
 * Copyright (c) 2023, Guillaume Lelarge (Dalibo),
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
static ExecutorRun_hook_type    prev_ExecutorRun = NULL;

static double max_cost = 0;

/*---- Function declarations ----*/

void		_PG_init(void);
void		_PG_fini(void);

static void my_ExecutorRun(QueryDesc *queryDesc, ScanDirection direction, uint64 count, bool execute_once);

/*
 * Module load callback
 */
void
_PG_init(void)
{
  /*
   * Define our custom variable.
   */
  DefineCustomRealVariable( "error_above_cost.max_cost",
    "Cancel query above a specific cost.",
    NULL,
    &max_cost,
    0,
    0,
    10000000000,
    PGC_SUSET,
    0,
    NULL,
    NULL,
    NULL);

	/*
	 * Install hooks.
	 */
	prev_ExecutorRun = ExecutorRun_hook;
	ExecutorRun_hook = my_ExecutorRun;
}

/*
 * Module unload callback
 */
void
_PG_fini(void)
{
	/* Uninstall hooks. */
	ExecutorRun_hook = prev_ExecutorRun;
}

/*
 * ExecutorRun hook: error if needed
 */
static void
my_ExecutorRun(QueryDesc *queryDesc, ScanDirection direction, uint64 count, bool execute_once)
{
  if (!IsBackgroundWorker)
  {
    if (queryDesc)
    {
      if (queryDesc->planstate)
      {
        if (queryDesc->planstate->plan)
        {
          elog(DEBUG1, "Total cost: %f -- Configured max cost: %f", queryDesc->planstate->plan->total_cost, max_cost);
          if (queryDesc->planstate->plan->total_cost > max_cost && max_cost > 0)
          {
            elog (ERROR, "coût excessif ! (%f vs %f)", queryDesc->planstate->plan->total_cost, max_cost);
          }
        }
      }
    }
  }

	if (prev_ExecutorRun)
		prev_ExecutorRun(queryDesc, direction, count, execute_once);
	else
		standard_ExecutorRun(queryDesc, direction, count, execute_once);
}

