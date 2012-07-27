INCLUDE_SRC_DIR=$(HOME)/.xenomailab/include
INCLUDE_BUILD_DIR=$(INCLUDE_SRC_DIR)/build
INCLUDE_OBJ_DIR=$(INCLUDE_BUILD_DIR)/.obj
INCLUDE_SRCS=rt_block_io.c settings.c strmap.c mtrx.c $(INCLUDES)
SETTINGS_PROJECT_PATH=settings_gui/

GUI=$(addsuffix _settings,$(EXECUTABLE))
INCLUDE_OBJS=$(addprefix $(INCLUDE_OBJ_DIR)/,$(INCLUDE_SRCS:.c=.o))
OBJS=$(SRCS:.c=.o)

### Default Xenomai installation path
XENO ?= /usr/xenomai

XENOCONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)

### Sanity check
ifeq ($(XENOCONFIG),)
all::
	@echo ">> Invoke make like this: \"make XENO=/path/to/xeno-config\" <"
	@echo
endif

#compiler
CC=$(shell $(XENOCONFIG) --cc)

#Warnings
CFLAGS=-fdiagnostics-show-option# -Wextra -Wundef -Wpointer-arith\
#-Wcast-align -Wwrite-strings -Wswitch-enum -Wformat=2 -Wdisabled-optimization\
#-Wmissing-include-dirs -Wfloat-equal -Wstrict-prototypes -Wstrict-overflow=5\
#-Wswitch-default -Wconversion -Wsign-conversion
CFLAGS+=-I$(INCLUDE_SRC_DIR) -O3
CFLAGS+=$(shell $(XENOCONFIG) --skin=native --cflags) $(MY_CFLAGS)

# This includes the library path of given Xenomai into the binary to make life
# easier for beginners if Xenomai's libs are not in any default search path.
LDFLAGS=-Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)
LDFLAGS+=$(shell $(XENOCONFIG) --skin=native --ldflags)
LDFLAGS+=-lm $(MY_LDFLAGS)

all: $(EXECUTABLE) $(GUI)

$(GUI):
	cd $(SETTINGS_PROJECT_PATH); qmake && make

$(EXECUTABLE): $(OBJS) $(INCLUDE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(INCLUDE_OBJ_DIR)/%.o: $(INCLUDE_SRC_DIR)/%.c $(INCLUDE_SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c %.h

$(INCLUDE_OBJS): | $(INCLUDE_OBJ_DIR)

$(INCLUDE_OBJ_DIR): | $(INCLUDE_BUILD_DIR)

$(INCLUDE_BUILD_DIR):
	mkdir $(INCLUDE_BUILD_DIR)

$(INCLUDE_OBJ_DIR):
	mkdir $(INCLUDE_OBJ_DIR)

.PHONY : clean cleanall
clean::
	$(RM) $(OBJS) $(INCLUDE_OBJS)
	cd $(SETTINGS_PROJECT_PATH); qmake && make clean

cleanall: clean
	$(RM) -r $(INCLUDE_BUILD_DIR)
	$(RM) $(EXECUTABLE)

