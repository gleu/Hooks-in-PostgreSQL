/*-------------------------------------------------------------------------
 *
 * loggingperdbname.c
 *		TESTS
 *
 *
 * Copyright (c) 2023, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/

#include "postgres.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "libpq/libpq-be.h"
#include "tcop/tcopprot.h"
#include "utils/elog.h"
#include "utils/memutils.h"
#include "storage/fd.h"

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

static void do_emit_log(ErrorData *edata);
static void emit_log_per_database(ErrorData *edata);

static emit_log_hook_type prev_emit_log_hook = NULL;

void _PG_init(void)
{
	prev_emit_log_hook = emit_log_hook;
	emit_log_hook = do_emit_log;
}

void _PG_fini(void)
{
    if (emit_log_hook == do_emit_log)
        emit_log_hook = prev_emit_log_hook;
}

static void do_emit_log(ErrorData *edata)
{
    static bool in_hook = false;
    
    /* Call any previous hooks */
    if (prev_emit_log_hook)
        prev_emit_log_hook(edata);
    
    /* Protect from recursive calls */
    if (! in_hook)
    {
        in_hook = true;
        emit_log_per_database(edata);
        in_hook = false;
    }
}

static void emit_log_per_database(ErrorData *edata)
{
    FILE           *file;
    char           filename[1000];
    MemoryContext  oldcontext;
    StringInfoData buf; 

    if (!edata->output_to_server)
        return;

    oldcontext = MemoryContextSwitchTo(ErrorContext);

    initStringInfo(&buf);
    appendStringInfoString(&buf, "module ");
    if (edata->message)
    {
        appendStringInfoString(&buf, "MESSAGE=");
        appendStringInfoString(&buf, edata->message);
    }
    appendStringInfoString(&buf, "\n");

    if (MyProcPort)
    {
        if (MyProcPort->database_name)
            sprintf(filename, "pg_log/%s.log", MyProcPort->database_name);
        else
            sprintf(filename, "pg_log/unknown.log");
        file = AllocateFile(filename, "a");
        fwrite(buf.data, 1, strlen(buf.data), file);
        FreeFile(file);
    }

	MemoryContextSwitchTo(oldcontext);
}

