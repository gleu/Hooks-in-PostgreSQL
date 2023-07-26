# contrib/logplpgsql/Makefile

MODULE_big = logplpgsql
OBJS = logplpgsql.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
override CFLAGS += -DINCLUDE_PACKAGE_SUPPORT=0  -I$(top_builddir)/src/pl/plpgsql/src

