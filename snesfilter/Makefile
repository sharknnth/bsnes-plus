include nall/Makefile

qtlibs := QtCore QtGui
include nall/qt/Makefile

c     := $(compiler) -xc -std=gnu99
cpp   := $(compiler) -std=gnu++0x
flags := -I. -Iobj $(qtinc)
link  :=

ifeq ($(DEBUG), 1)
  flags += -O0 -g -DDEBUG
else
  flags += -O3 -fomit-frame-pointer -DNDEBUG 
endif

# silence warnings
flags += -Wno-switch -Wno-absolute-value -Wno-parentheses

ifeq ($(platform),x)
  flags := -fPIC -fopenmp $(flags)
  link += -s -fopenmp -lpthread -lgomp
else ifeq ($(platform),osx)
# TODO: openmp not currently supported in Apple clang
#  flags := -fPIC -fopenmp $(flags)
#  link += -fopenmp -lpthread -lgomp 
  flags := -fPIC $(flags)
  link += -lpthread
  link += -F/usr/local/lib
else ifeq ($(platform),win)
  flags := -fopenmp $(flags)
  link += -fopenmp -lpthread
endif

objects := snesfilter

compile = \
  $(strip \
    $(if $(filter %.c,$<), \
      $(c) $(flags) $1 -c $< -o $@, \
      $(if $(filter %.cpp,$<), \
        $(cpp) $(flags) $1 -c $< -o $@ \
      ) \
    ) \
  )

%.o: $<; $(call compile)

all: build;

objects := $(patsubst %,obj/%.o,$(objects))
moc_headers := $(call rwildcard,./,%.moc.hpp)
moc_objects := $(foreach f,$(moc_headers),obj/$(notdir $(patsubst %.moc.hpp,%.moc,$f)))

# automatically run moc on all .moc.hpp (MOC header) files
%.moc: $<; $(moc) -i $< -o $@

# automatically generate %.moc build rules
__list = $(moc_headers)
$(foreach f,$(moc_objects), \
  $(eval __file = $(word 1,$(__list))) \
  $(eval __list = $(wordlist 2,$(words $(__list)),$(__list))) \
  $(eval $f: $(__file)) \
)

##################
### snesfilter ###
##################

obj/snesfilter.o: snesfilter.cpp *

###############
### targets ###
###############

build: $(moc_objects) $(objects)
ifeq ($(platform),x)
	ar rcs libsnesfilter.a $(objects)
	$(cpp) $(link) -o libsnesfilter.so -shared -Wl,-soname,libsnesfilter.so.1 $(objects) $(qtlib)
else ifeq ($(platform),osx)
	ar rcs libsnesfilter.a $(objects)
	$(cpp) $(link) -o libsnesfilter.dylib -shared -dynamiclib $(objects) $(qtlib)
else ifeq ($(platform),win)
	$(cpp) $(link) -o snesfilter.dll -shared -Wl,--out-implib,libsnesfilter.a $(objects) $(qtlib)
endif

install:
ifeq ($(platform),x)
	install -D -m 755 libsnesfilter.a $(DESTDIR)$(prefix)/lib
	install -D -m 755 libsnesfilter.so $(DESTDIR)$(prefix)/lib
	ldconfig -n $(DESTDIR)$(prefix)/lib
else ifeq ($(platform),osx)
	cp libsnesfilter.dylib /usr/local/lib/libsnesfilter.dylib
endif

clean:
	-@$(call delete,obj/*.o)
	-@$(call delete,obj/*.moc)
	-@$(call delete,libsnesfilter.a)
	-@$(call delete,libsnesfilter.so)
	-@$(call delete,libsnesfilter.dylib)
	-@$(call delete,snesfilter.dll)
