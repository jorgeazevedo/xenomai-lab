SETTINGS_PROJECT_PATH = settings_gui
BUILD_DIR = $(SETTINGS_PROJECT_PATH)/build
OBJ_DIR = $(BUILD_DIR)/.obj

INCLUDE_SRC_DIR=$(HOME)/.xenomailab/include
INCLUDE_BUILD_DIR=$(INCLUDE_SRC_DIR)/build
INCLUDE_OBJ_DIR=$(INCLUDE_BUILD_DIR)/.obj
INCLUDE_SRCS=rt_block_io.c settings.c strmap.c mtrx.c $(INCLUDES)

GUI=$(addsuffix _settings,$(EXECUTABLE))
INCLUDE_OBJS=$(addprefix $(INCLUDE_OBJ_DIR)/,$(INCLUDE_SRCS:.c=.o))
OBJS=$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

### Default Xenomai installation path
XENO ?= /usr/xenomai

XENOCONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)

### Sanity check
ifeq ($(XENOCONFIG),)
all::
	@echo ">> Invoke make like this: \"make XENO=/path/to/xeno-config\" <"
	@echo
endif

MKDIR = mkdir -p

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

.PHONY : all

all: $(EXECUTABLE) $(GUI)

$(GUI):
	cd $(SETTINGS_PROJECT_PATH); qmake master.pro && make

$(EXECUTABLE): $(OBJS) $(INCLUDE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#Notice that gain.o depends only on gain.c, as there is no gain.h. The more
#specific rules wins
$(OBJ_DIR)/$(EXECUTABLE).o: $(EXECUTABLE).c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR): | $(BUILD_DIR)

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(INCLUDE_OBJ_DIR)/%.o: $(INCLUDE_SRC_DIR)/%.c $(INCLUDE_SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(INCLUDE_OBJS): | $(INCLUDE_OBJ_DIR)

$(INCLUDE_OBJ_DIR): | $(INCLUDE_BUILD_DIR)

$(INCLUDE_BUILD_DIR):
	$(MKDIR) $(INCLUDE_BUILD_DIR)

$(INCLUDE_OBJ_DIR):
	$(MKDIR) $(INCLUDE_OBJ_DIR)

.PHONY : clean cleanall
clean::
	$(RM) $(OBJS) $(INCLUDE_OBJS)
	cd $(SETTINGS_PROJECT_PATH); qmake master.pro && make clean

cleanall: clean
	$(RM) $(EXECUTABLE)
	$(RM) $(GUI)
	$(RM) -r $(SETTINGS_PROJECT_PATH)/build
	$(RM) $(SETTINGS_PROJECT_PATH)/Makefile*
	$(RM) -r $(INCLUDE_SRC_DIR)/build
	$(RM) $(INCLUDE_SRC_DIR)/Makefile*
