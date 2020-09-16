SUBNAME = meta
SPEC = smartmet-plugin-$(SUBNAME)
INCDIR = smartmet/plugins/$(SUBNAME)

# Installation directories

processor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
datadir = $(PREFIX)/share
plugindir = $(datadir)/smartmet/plugins
objdir = obj

# Compiler options

DEFINES = -DUNIX -D_REENTRANT

-include $(HOME)/.smartmet.mk
GCC_DIAG_COLOR ?= always

# Boost 1.69

ifneq "$(wildcard /usr/include/boost169)" ""
  INCLUDES += -isystem /usr/include/boost169
  LIBS += -L/usr/lib64/boost169
endif

ifneq "$(wildcard /usr/gdal30/include)" ""
  INCLUDES += -isystem /usr/gdal30/include
  LIBS += -L/usr/gdal30/lib
else
  INCLUDES += -isystem /usr/include/gdal
endif

ifeq ($(CXX), clang++)

 # TODO: sign-conversion should be enabled
 # TODO: could not find a way to avoid the weak vtable warning

 FLAGS = \
	-std=c++11 -fPIC -MD \
	-Wno-c++98-compat \
	-Wno-float-equal \
	-Wno-padded \
	-Wno-missing-prototypes \
	-Wno-sign-conversion \
	-Wno-weak-vtables

 INCLUDES += \
	-I$(includedir)/smartmet

else

 FLAGS = -std=c++11 -fPIC -MD -Wall -W -Wno-unused-parameter -fno-omit-frame-pointer -Wno-unknown-pragmas -fdiagnostics-color=$(GCC_DIAG_COLOR)

 FLAGS_DEBUG = \
	-Wcast-align \
	-Wcast-qual \
	-Winline \
	-Wno-multichar \
	-Wno-pmf-conversions \
	-Wpointer-arith \
	-Wwrite-strings

 FLAGS_RELEASE = -Wno-uninitialized

 INCLUDES += \
	-I$(includedir)/smartmet

endif

ifeq ($(TSAN), yes)
  FLAGS += -fsanitize=thread
endif
ifeq ($(ASAN), yes)
  FLAGS += -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fsanitize-address-use-after-scope
endif

# Compile options in detault, debug and profile modes

CFLAGS_RELEASE = $(DEFINES) $(FLAGS) $(FLAGS_RELEASE) -DNDEBUG -O2 -g
CFLAGS_DEBUG   = $(DEFINES) $(FLAGS) $(FLAGS_DEBUG)   -Werror  -O0 -g

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  override CFLAGS += $(CFLAGS_DEBUG)
else
  override CFLAGS += $(CFLAGS_RELEASE)
endif

LIBS += -L$(libdir) \
	-lsmartmet-spine \
	-lsmartmet-macgyver \
	-lctpp2 

TEMPLATES = $(wildcard cnf/templates/*.template)
COMPILED_TEMPLATES = $(patsubst %.template, %.c2t, $(TEMPLATES))

# What to install

LIBFILE = $(SUBNAME).so

# How to install

INSTALL_PROG = install -p -m 775
INSTALL_DATA = install -p -m 664

# Compilation directories

vpath %.cpp $(SUBNAME)
vpath %.h $(SUBNAME)

# The files to be compiled

SRCS = $(wildcard $(SUBNAME)/*.cpp)
HDRS = $(wildcard $(SUBNAME)/*.h)
OBJS = $(patsubst %.cpp, obj/%.o, $(notdir $(SRCS)))

INCLUDES := -I$(SUBNAME) $(INCLUDES)

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE) all-templates
debug: all
release: all
profile: all

$(LIBFILE): $(OBJS)
	$(CXX) $(CFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJS) $(LIBS)

clean: clean-templates
	rm -f $(LIBFILE) *~ $(SUBNAME)/*~
	rm -rf obj
	cd test && make clean

format:
	clang-format -i -style=file $(SUBNAME)/*.h $(SUBNAME)/*.cpp

install:
	@mkdir -p $(plugindir)
	$(INSTALL_PROG) $(LIBFILE) $(plugindir)/$(LIBFILE)

test:
	cd test && make test

objdir:
	@mkdir -p $(objdir)

rpm: clean $(SPEC).spec
	rm -f $(SPEC).tar.gz # Clean a possible leftover from previous attempt
	tar -czvf $(SPEC).tar.gz --exclude test --exclude-vcs --transform "s,^,$(SPEC)/," *
	rpmbuild -ta $(SPEC).tar.gz
	rm -f $(SPEC).tar.gz

all-templates:
	$(MAKE) -C cnf/templates all

clean-templates:
	$(MAKE) -C cnf/templates clean

cnf/templates/%.c2t: cnf/templates/%.template
	cd cnf/templates && $(PREFIX)/bin/ctpp2c $(notdir $<) $(notdir $@)

.SUFFIXES: $(SUFFIXES) .cpp

obj/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

-include $(wildcard obj/*.d)

