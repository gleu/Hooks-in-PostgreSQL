/*-------------------------------------------------------------------------
 *
 * betterexplain.c
 *      Change explain options on the fly
 *
 *
 * Copyright (c) 2025-2026, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/

#include "postgres.h"
#include "fmgr.h"
#include "utils/guc.h"
#include "commands/explain.h"

PG_MODULE_MAGIC;

static bool explain_costs;
static bool explain_verbose;
static bool explain_buffers;
static bool explain_summary;
static bool explain_memory;
static bool explain_settings;
static bool explain_generic;

static bool explainanalyze_costs;
static bool explainanalyze_verbose;
static bool explainanalyze_buffers;
static bool explainanalyze_summary;
static bool explainanalyze_memory;
static bool explainanalyze_settings;
static bool explainanalyze_wal;
static bool explainanalyze_timing;

extern void _PG_init(void);

/*
 * betterexplain_hook
 *
 * change EXPLAIN behavior
 *
 */
static void
betterexplain_hook(Query *query, int cursorOptions,
       IntoClause *into, ExplainState *es,
       const char *queryString, ParamListInfo params,
       QueryEnvironment *queryEnv)
{
  // Change options
  es->costs    = es->analyze ? explainanalyze_costs    : explain_costs;
  es->verbose  = es->analyze ? explainanalyze_verbose  : explain_verbose;
  es->buffers  = es->analyze ? explainanalyze_buffers  : explain_buffers;
  es->summary  = es->analyze ? explainanalyze_summary  : explain_summary;
  es->memory   = es->analyze ? explainanalyze_memory   : explain_memory;
  es->settings = es->analyze ? explainanalyze_settings : explain_settings;
  es->generic  = es->analyze ? false                   : explain_generic;
  es->wal      = es->analyze ? explainanalyze_wal      : false;
  es->timing   = es->analyze ? explainanalyze_timing   : false;

  // TODO serialize and format

  // Get the plan
  standard_ExplainOneQuery(query, cursorOptions, into, es,
                   queryString, params, queryEnv);
}

/*
 * Module initialization function
 */
void
_PG_init(void)
{
    // add the hook function
    ExplainOneQuery_hook = betterexplain_hook;

    // define the custom parameters for EXPLAIN
    DefineCustomBoolVariable("betterexplain.costs",
                             gettext_noop("Change COSTS option for EXPLAIN."),
                             NULL,
                             &explain_costs,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.verbose",
                             gettext_noop("Change VERBOSE option for EXPLAIN."),
                             NULL,
                             &explain_verbose,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.buffers",
                             gettext_noop("Change BUFFERS option for EXPLAIN."),
                             NULL,
                             &explain_buffers,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.summary",
                             gettext_noop("Change SUMMARY option for EXPLAIN."),
                             NULL,
                             &explain_summary,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.memory",
                             gettext_noop("Change MEMORY option for EXPLAIN."),
                             NULL,
                             &explain_memory,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.settings",
                             gettext_noop("Change SETTINGS option for EXPLAIN."),
                             NULL,
                             &explain_settings,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplain.generic",
                             gettext_noop("Change GENERIC option for EXPLAIN."),
                             NULL,
                             &explain_generic,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    // define the custom parameters for EXPLAIN ANALYZE
    DefineCustomBoolVariable("betterexplainanalyze.costs",
                             gettext_noop("Change COSTS option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_costs,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.verbose",
                             gettext_noop("Change VERBOSE option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_verbose,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.buffers",
                             gettext_noop("Change BUFFERS option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_buffers,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.summary",
                             gettext_noop("Change SUMMARY option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_summary,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.memory",
                             gettext_noop("Change MEMORY option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_memory,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.settings",
                             gettext_noop("Change SETTINGS option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_settings,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.wal",
                             gettext_noop("Change WAL option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_wal,
                             false,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

    DefineCustomBoolVariable("betterexplainanalyze.timing",
                             gettext_noop("Change TIMING option for EXPLAIN ANALYZE."),
                             NULL,
                             &explainanalyze_timing,
                             true,
                             PGC_USERSET,
                             0,
                             NULL,
                             NULL,
                             NULL);

}
