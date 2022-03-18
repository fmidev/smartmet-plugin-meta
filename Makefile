SUBNAME = meta
SPEC = smartmet-plugin-$(SUBNAME)
INCDIR = smartmet/plugins/$(SUBNAME)

REQUIRES = gdal configpp

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

# Compiler options

DEFINES = -DUNIX -D_REENTRANT

LIBS += -L$(libdir) \
	-lsmartmet-spine \
	-lsmartmet-macgyver \
	-lctpp2

TEMPLATES = $(wildcard cnf/templates/*.template)
COMPILED_TEMPLATES = $(patsubst %.template, %.c2t, $(TEMPLATES))

# What to install

LIBFILE = $(SUBNAME).so

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
	$(CXX) $(LDFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJS) $(LIBS)

clean: clean-templates
	rm -f $(LIBFILE) *~ $(SUBNAME)/*~
	rm -rf obj
	$(MAKE) -C test $@

format:
	clang-format -i -style=file $(SUBNAME)/*.h $(SUBNAME)/*.cpp

install:
	@mkdir -p $(plugindir)
	$(INSTALL_PROG) $(LIBFILE) $(plugindir)/$(LIBFILE)

test test-oracle test-postgresql:
	$(MAKE) -C test $@

objdir:
	@mkdir -p $(objdir)

rpm: clean $(SPEC).spec
	rm -f $(SPEC).tar.gz # Clean a possible leftover from previous attempt
	tar -czvf $(SPEC).tar.gz --exclude test --exclude-vcs --transform "s,^,$(SPEC)/," *
	rpmbuild -tb $(SPEC).tar.gz
	rm -f $(SPEC).tar.gz

all-templates:
	$(MAKE) -C cnf/templates all

clean-templates:
	$(MAKE) -C cnf/templates clean

cnf/templates/%.c2t: cnf/templates/%.template
	cd cnf/templates && $(PREFIX)/bin/ctpp2c $(notdir $<) $(notdir $@)

.SUFFIXES: $(SUFFIXES) .cpp

obj/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -MD -MF $(patsubst obj/%.o, obj/%.d, $@) -MT $@ -o $@ $<

-include $(wildcard obj/*.d)

