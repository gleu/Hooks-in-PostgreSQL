/*-------------------------------------------------------------------------
 *
 * deny_drop.c
 *      Deny dropping a database
 *
 *
 * Copyright (c) 2012-2023, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/

#include "postgres.h"
#include "fmgr.h"
#include "utils/guc.h"
#include "catalog/objectaccess.h"
#include "catalog/pg_database.h"

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
