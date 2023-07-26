/*-------------------------------------------------------------------------
 *
 * my_client_auth.c
 *      Authorize connections unless a file exists
 *
 *
 * Copyright (c) 2012, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/
#include "postgres.h"
#include "fmgr.h"

#include "commands/seclabel.h"
#include "libpq/auth.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


PG_MODULE_MAGIC;

static ClientAuthentication_hook_type prev_client_auth_hook = NULL;

void _PG_init(void);
static void my_client_auth(Port *port, int status);

/*
 * my_client_auth
 * Authentication hook entry point
 */
static void
my_client_auth(Port *port, int status)
{
    struct stat buf;

    if (prev_client_auth_hook)
        (*prev_client_auth_hook) (port, status);

    /*
     * If the authentication fails, no need to go further
     */
    if (status != STATUS_OK)
        return;

    /*
     * Here is all the work of our hook
     */
    if(!stat("/tmp/connection.stopped", &buf))
        ereport(FATAL, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("Connection not authorized!!")));
}

/* Module entry point */
void
_PG_init(void)
{
    prev_client_auth_hook = ClientAuthentication_hook;
    ClientAuthentication_hook = my_client_auth;
}
