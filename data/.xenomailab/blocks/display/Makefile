###### CONFIGURATION ######
INCLUDE=$(HOME)/.xenomailab/include/
### List of applications to be build
EXECUTABLE=display
DEP=display.c display_settings.c $(INCLUDE)nrt_block_io.c $(INCLUDE)settings.c $(INCLUDE)strmap.c $(INCLUDE)mtrx.c
OBJS=$(DEP:.c=.o)
#APPLICATIONS =  virtual_ad virtual_da gain

### Note: to override the search path for the xeno-config script, use "make XENO=..."

###### USER SPACE BUILD (no change required normally) ######
#ifeq ($(KERNELRELEASE),)
#ifneq ($(APPLICATIONS),)

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

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(DEP):
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
clean::
	$(RM) $(BLOCK) $(OBJS)
cleanall::
	$(RM) $(BLOCK) $(OBJS) $(EXECUTABLE)
#.c.o?
#
#endif
#endif

#	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

###### SPECIAL TARGET RULES ######
#rtprint: blockioz.c
#	$(CC) $(CFLAGS) $? $(LDFLAGS) -c -o $@
