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


PG_MODULE_MAGIC;

extern void _PG_init(void);

/*
 * deny_drop
 *
 * performs one check: is it an encrypted password or not?
 * ereport's if not acceptable
 *
 */
static void
deny_drop(ObjectAccessType access,
                       Oid classId,
                       Oid objectId,
                       int subId,
                       void *arg)
{
	if (access = OAT_DROP)
	{
        if (classId == DatabaseRelationId)
		    ereport(ERROR,
		    	(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
		    	 errmsg("cannot drop a database!")));
	}
}

/*
 * Module initialization function
 */
void
_PG_init(void)
{
	object_access_hook = deny_drop;
}
