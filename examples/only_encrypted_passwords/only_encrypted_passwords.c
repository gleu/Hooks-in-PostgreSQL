/*-------------------------------------------------------------------------
 *
 * only_encrypted_passwords.c
 *      Authorize only encrypted passwords
 *
 *
 * Copyright (c) 2012, Guillaume Lelarge (Dalibo),
 * guillaume.lelarge@dalibo.com
 *
 *-------------------------------------------------------------------------*/
#include "postgres.h"

#include <ctype.h>

#include "commands/user.h"
#include "fmgr.h"
#include "libpq/md5.h"


PG_MODULE_MAGIC;

extern void _PG_init(void);

/*
 * check_password
 *
 * performs one check: is it an encrypted password or not?
 * ereport's if not acceptable
 *
 */
static void
check_password(const char *username,
			   const char *password,
			   int password_type,
			   Datum validuntil_time,
			   bool validuntil_null)
{
	if (password_type == PASSWORD_TYPE_PLAINTEXT)
	{
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			 errmsg("password is not encrypted")));
	}
}

/*
 * Module initialization function
 */
void
_PG_init(void)
{
	check_password_hook = check_password;
}
