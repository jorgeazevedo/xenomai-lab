###### CONFIGURATION ######
INCLUDE=$(HOME)/.xenomailab/include/
### List of applications to be build
EXECUTABLE=gain

DEP=gain.c gain_settings.c $(INCLUDE)rt_block_io.c $(INCLUDE)settings.c $(INCLUDE)strmap.c $(INCLUDE)mtrx.c
OBJS=$(DEP:.c=.o)

SETTINGS_EXECUTABLE=$(EXECUTABLE)_settings
SETTINGS_PROJECT_FOLDER=$(SETTINGS_EXECUTABLE)_proj/$(SETTINGS_EXECUTABLE)_proj/

### Default Xenomai installation path
XENO ?= /usr/xenomai

XENOCONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)

### Sanity check
ifeq ($(XENOCONFIG),)
all::
	@echo ">>> Invoke make like this: \"make XENO=/path/to/xeno-config\" <<<"
	@echo
endif

MY_CFLAGS=-I$(INCLUDE)

CC=$(shell $(XENOCONFIG) --cc)

CFLAGS=$(shell $(XENOCONFIG) --skin=native --cflags) $(MY_CFLAGS)

LDFLAGS=$(shell $(XENOCONFIG) --skin=native --ldflags) $(MY_LDFLAGS) -lnative -lm

# This includes the library path of given Xenomai into the binary to make live
# easier for beginners if Xenomai's libs are not in any default search path.
LDFLAGS+=-Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)

#CFLAGS+=-lparapin

all: $(DEP) $(EXECUTABLE)
	cd $(SETTINGS_PROJECT_FOLDER); qmake && make;

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(DEP):
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
clean::
	$(RM) $(BLOCK) $(OBJS)
	cd $(SETTINGS_PROJECT_FOLDER); qmake && make clean;
cleanall::
	$(RM) $(BLOCK) $(OBJS) $(EXECUTABLE) $(SETTINGS_EXECUTABLE)
	cd $(SETTINGS_PROJECT_FOLDER); qmake && make clean;

