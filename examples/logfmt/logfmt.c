/*-------------------------------------------------------------------------
 *
 * logfmt.c
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
static void emit_logfmt(ErrorData *edata);

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
    emit_logfmt(edata);
    in_hook = false;
  }
}

static void emit_logfmt(ErrorData *edata)
{
  FILE           *file;
  MemoryContext  oldcontext;
  StringInfoData buf; 

  if (!edata->output_to_server)
    return;

  oldcontext = MemoryContextSwitchTo(ErrorContext);

  initStringInfo(&buf);

  appendStringInfoString(&buf, "level=");
  appendStringInfoString(&buf, error_severity(edata->elevel));

  if (MyProcPort)
  {
    if (MyProcPort->database_name)
    {
      appendStringInfoString(&buf, " database=");
      appendStringInfoString(&buf, MyProcPort->database_name);
    }
  }

  if (edata->message)
  {
    appendStringInfoString(&buf, " message=");
    appendStringInfoString(&buf, edata->message);
  }
  appendStringInfoString(&buf, "\n");

  file = AllocateFile("postgresql.logfmt", "a");
  fwrite(buf.data, 1, strlen(buf.data), file);
  FreeFile(file);

  MemoryContextSwitchTo(oldcontext);
}

