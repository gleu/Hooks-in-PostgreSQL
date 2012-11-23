/*-------------------------------------------------------------------------
 *
 * deny_drop.c
 *      Deny dropping a database
 *
 *
 * Copyright (c) 2012, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/
#include "postgres.h"

#include <ctype.h>

#include "catalog/dependency.h"
#include "catalog/objectaccess.h"
#include "catalog/pg_class.h"
#include "catalog/pg_database.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_proc.h"
#include "commands/seclabel.h"
#include "executor/executor.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "tcop/utility.h"
#include "utils/guc.h"

#include "commands/user.h"
#include "fmgr.h"
#include "libpq/md5.h"

static bool    iknowwhatiamdoing = false;


PG_MODULE_MAGIC;

extern void _PG_init(void);

/*
 * deny_drop
 *
 * deny dropping a database
 *
 */
static void
deny_drop(ObjectAccessType access,
                       Oid classId,
                       Oid objectId,
                       int subId,
                       void *arg)
{
	if (access == OAT_DROP && !iknowwhatiamdoing)
	{
        if (classId == DatabaseRelationId)
		    ereport(ERROR,
		    	(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
		    	 errmsg("cannot drop a database!")));
	}
}

/*
 * Module initialization function
 */
void
_PG_init(void)
{
    // add the hook function
	object_access_hook = deny_drop;

    // define the custom parameter
    DefineCustomBoolVariable("deny_drop.iknowwhatiamdoing",
                             gettext_noop("If true, allow drop database."),
                             NULL,
                             &iknowwhatiamdoing,
                             false,
                             PGC_USERSET,
                             0,
#if PG_VERSION_NUM >= 90100
                             NULL,
#endif
                             NULL,
                             NULL);
}
