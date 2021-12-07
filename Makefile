MODULES = pg_mqtt
EXTENSION = pg_mqtt     # the extersion's name
DATA = pg_mqtt--0.0.1.sql    # script file to install
#REGRESS = get_sum_test      # the test script file

# for posgres build
PG_CONFIG = pg_config

#LIBS += -lpaho-mqtt3c
#SHLIB_LINK := $(LIBS)
PG_LDFLAGS += -lpaho-mqtt3c
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)