# contrib/only_encrypted_passwords/Makefile

MODULE_big = only_encrypted_passwords
OBJS = only_encrypted_passwords.o

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/only_encrypted_passwords
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
