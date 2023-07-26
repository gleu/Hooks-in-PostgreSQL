# contrib/loggingperdbname/Makefile

MODULE_big = loggingperdbname
OBJS = loggingperdbname.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
override CFLAGS += -DINCLUDE_PACKAGE_SUPPORT=0  -I$(top_builddir)/src/pl/plpgsql/src

