### List of applications to be build
EXECUTABLE=gain

#Source files
SRCS=gain.c gain_settings.c
INCLUDES=#There are in ~/.xenomailab/include

#Custom flags
MY_CFLAGS=#compiler e.g. -Werror
MY_LDFLAGS=#libs e.g. -lparapin

#Everything bellow this point is common code

INCLUDE_PATH=$(HOME)/.xenomailab/include
INCLUDES+=rt_block_io.c settings.c strmap.c mtrx.c
SETTINGS_PROJECT_FOLDER=settings_gui/

#preprend includes with include path
PREPENDED_INCLUDES=$(INCLUDES:%.c=$(INCLUDE_PATH)/%.c)
DEP=$(SRCS) $(PREPENDED_INCLUDES)
OBJS=$(DEP:.c=.o)

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
CFLAGS+=-I$(INCLUDE_PATH) -O3
CFLAGS+=$(shell $(XENOCONFIG) --skin=native --cflags) $(MY_CFLAGS)

# This includes the library path of given Xenomai into the binary to make life
# easier for beginners if Xenomai's libs are not in any default search path.
LDFLAGS=-Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)
# -lnative -L/usr/lib -lxenomai -lpthread
LDFLAGS+=$(shell $(XENOCONFIG) --skin=native --ldflags)
LDFLAGS+=-lm $(MY_LDFLAGS)

all: $(DEP) $(EXECUTABLE)
	cd $(SETTINGS_PROJECT_FOLDER); qmake && make;

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(DEP):
	$(CC) $(CFLAGS) -c -o $@
clean::
	$(RM) $(BLOCK) $(OBJS)
	cd $(SETTINGS_PROJECT_PATH); qmake && make clean;
cleanall::
	$(RM) $(BLOCK) $(OBJS) $(EXECUTABLE)
	cd $(SETTINGS_PROJECT_FOLDER); qmake && make clean;

