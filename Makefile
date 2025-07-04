# vim:ts=8:sw=8:noexpandtab:
# Makefile copied from libcmini - THANKS!
#
# disable verbose builds by default
# If you want to see all the gory details of the build process,
# run "VERBOSE=yes make <target>"

ifndef VERBOSE
  	VERBOSE=no
endif
ifeq (,$(filter $(VERBOSE),Y yes))
  	Q=@
	v=
else
  	Q=
	v=v
endif

all:$(patsubst %,%/$(APP),$(TRGTDIRS))

#
# ONLY_68K: for faster developing; set to Y to build only the 68000 library
# BUILD_CF: Build ColdFire binaries.
#
ONLY_68K=N
BUILD_CF=Y
BUILD_FAST=$(shell if $(CC) -mfastcall -E - < /dev/null >/dev/null 2>&1; then echo Y; else echo N; fi)
BUILD_SOFT_FLOAT=Y
BUILD_SHORT=Y
COMPILE_ELF=Y

-include Make.config
-include Make.config.local

ifneq (,$(filter $(COMPILE_ELF),Y yes))
	CROSSPREFIX=m68k-atari-mintelf-
else 
  	CROSSPREFIX=m68k-atari-mint-
endif

CFLAGS+=\
	-Os \
	-fomit-frame-pointer
	
CC=$(CROSSPREFIX)gcc
LD=$(CROSSPREFIX)ld
CPP=$(CROSSPREFIX)cpp
OBJCOPY=$(CROSSPREFIX)objcopy
AR=$(CROSSPREFIX)ar
RANLIB=$(CROSSPREFIX)ranlib

INCLUDE=-Iinclude -I.
CSRCS= $(wildcard $(SRCDIR)/*.c)
SRCDIR=src
BUILDDIR=build

ifneq (,$(filter $(ONLY_68K),Y yes))
	# asume a multi-lib without flags ar m68000
	# NOTE \s?$ is important - gcc on Windows outputs \r\n-lineendings but MSYS's grep only accept \n -> \s eats \r
	MULTILIBDIRS := $(shell $(CC) -print-multi-lib | grep -E ';\s?$$' | sed -e "s/;.*//")
else
	MULTILIBDIRS := $(shell $(CC) -print-multi-lib | sed -e "s/;.*//")
	ifeq (,$(filter $(BUILD_FAST),Y yes))
		MULTILIBDIRS := $(shell echo $(MULTILIBDIRS) | sed -e 's/\S*fastcall\S*/ /g')
	endif
	ifeq (,$(filter $(BUILD_CF),Y yes))
		MULTILIBDIRS := $(shell echo $(MULTILIBDIRS) | sed -e 's/\S*m5475\S*/ /g')
	endif
	ifeq (,$(filter $(BUILD_SOFT_FLOAT),Y yes))
		MULTILIBDIRS := $(shell echo $(MULTILIBDIRS) | sed -e 's/\S*soft-float\S*/ /g')
	endif
	ifeq (,$(filter $(BUILD_SHORT),Y yes))
		MULTILIBDIRS := $(shell echo $(MULTILIBDIRS) | sed -e 's/\S*short\S*/ /g')
	endif
endif
MULTILIBFLAGS = $(shell $(CC) -print-multi-lib | grep '^$(1);' | sed -e 's/^.*;//' -e 's/@/ -/g')

LIBDIRS=$(patsubst %,$(BUILDDIR)/%,$(MULTILIBDIRS))
OBJDIRS=$(patsubst %,%/objs,$(LIBDIRS))

COBJS=$(patsubst $(SRCDIR)/%.o,%.o,$(patsubst %.c,%.o,$(CSRCS)))
OBJS=$(COBJS)

LIBC=libxyzst.a

LIBS=$(patsubst %,%/$(LIBC),$(LIBDIRS))

all: dirs libs
libs: $(LIBS)
	
dirs::
	$(Q)mkdir -p $(LIBDIRS) $(OBJDIRS)

clean:
	@rm -r$(v) $(patsubst %,$(BUILDDIR)/%,$(shell ls $(BUILDDIR)))

all:$(patsubst %,%/$(APP),$(TRGTDIRS))

#
# multilib flags
#
define MULTILIBFLAGS_TEMPLATE
$(BUILDDIR)/$(1)/%: CFLAGS += $(call MULTILIBFLAGS,$(1))
endef
$(foreach DIR,$(MULTILIBDIRS),$(eval $(call MULTILIBFLAGS_TEMPLATE,$(DIR))))

#
# generate pattern rules for multilib object files
#
define CC_TEMPLATE
$(1)/objs/%.o:$(SRCDIR)/%.c
	$(Q)echo "CC $$(@)"
	$(Q)$(CC) -MMD -MP -MF $$(@:.o=.d) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)/objs/%.o:$(SRCDIR)/%.S
	$(Q)echo "CC $$(@)"
	$(Q)$(CC) -MMD -MP -MF $$(@:.o=.d) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@

$(1)/%.o:$(SRCDIR)/%.S
	$(Q)echo "CC $$(@)"
	$(Q)$(CC) -MMD -MP -MF $$(@:.o=.d) $$(CFLAGS) $(INCLUDE) -c $$< -o $$@
endef
$(foreach DIR,$(LIBDIRS),$(eval $(call CC_TEMPLATE,$(DIR))))

#
# generate pattern rules for multilib archive
#
define ARC_TEMPLATE
$(1)_OBJS=$(patsubst %,$(1)/objs/%,$(OBJS))
$(1)/$(LIBC): $$($(1)_OBJS)
	$(Q)echo "AR $$@"
	$(Q)$(AR) cr $$@ $$?
	$(Q)$(RANLIB) $$@
LIBDEPEND+=$$($1_OBJS)
LIBSE+=$(1)/$(LIBC)
endef
$(foreach DIR,$(LIBDIRS),$(eval $(call ARC_TEMPLATE,$(DIR))))

.PHONY: release
release: all
	RELEASETAG=$$(git tag --contains | sed -e 's/v//' | sed -e 's/ //g') ;\
    RELEASEDIR=libcmini-$$RELEASETAG ;\
	if [ "x$$RELEASETAG" != "x" ] ; then\
	    mkdir -p $$RELEASEDIR/lib ;\
	    cp -r include $$RELEASEDIR ;\
	    for i in $(MULTILIBDIRS); do \
		    mkdir -p $$RELEASEDIR/lib/$$i ;\
	        cp $(BUILDDIR)/$$i/libcmini.a $$RELEASEDIR/lib/$$i ;\
	        cp $(BUILDDIR)/$$i/crt0.o $$RELEASEDIR/lib/$$i ;\
	    done ;\
		chown -R 0:0 $$RELEASEDIR/* ;\
		tar -C $$RELEASEDIR -cvzf $$RELEASEDIR.tar.gz . ;\
	    chmod 644 $$RELEASEDIR.tar.gz ;\
	fi ;\
	ls -l


.PHONY: printvars
printvars:
	@$(foreach V,$(.VARIABLES),	$(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V))))

install: all

ifeq (,$(PREFIX))
PREFIX = /opt/libxyzst
endif
ifeq (,$(PREFIX_FOR_INCLUDE))
PREFIX_FOR_INCLUDE =
endif
ifeq (,$(PREFIX_FOR_LIB))
PREFIX_FOR_LIB =
endif

ifneq (,$(PREFIX)$(PREFIX_FOR_INCLUDE)$(PREFIX_FOR_LIB))
	ifneq (,$(DESTDIR))
		ERR := $(error Options DESTDIR and PREFIX[_FOR_(INCLUDE|LIB)] are mutually incompatible. Use either DESTDIR or PREFIX...)
	endif
	ifneq (,$(PREFIX))
		ifeq (,$(PREFIX_FOR_INCLUDE))
			PREFIX_FOR_INCLUDE = $(PREFIX)/include
		endif
		ifeq (,$(PREFIX_FOR_LIB))
			PREFIX_FOR_LIB = $(PREFIX)/lib
		endif
	endif
else
	PREFIX_FOR_INCLUDE = $(DESTDIR)/usr/include
	PREFIX_FOR_LIB = $(DESTDIR)/usr/lib
endif

ifneq (,$(PREFIX_FOR_INCLUDE))
install : install-include
install-include:
	@mkdir -pv $(PREFIX_FOR_INCLUDE)
	@cp -arv include/* $(PREFIX_FOR_INCLUDE)
endif

ifneq (,$(PREFIX_FOR_LIB))
install : install-libs
install-libs:
	@for i in $(MULTILIBDIRS); do \
		mkdir -pv $(PREFIX_FOR_LIB)/$$i; \
		cp -av $(BUILDDIR)/$$i/$(LIBC) $(PREFIX_FOR_LIB)/$$i; \
	done
endif

lint:
	cppcheck -q --std=c99 --enable=performance --language=c --template=gcc -DDGK -DGEMDOS src/*.c
