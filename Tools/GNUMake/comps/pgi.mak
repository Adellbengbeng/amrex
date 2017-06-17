#
# Generic setup for using PGI
#

# Due to how we are handling CUDA C compiling,
# we must handle all of the C/C++ options first,
# and only handle Fortran afterward.

CXX = pgc++
CC  = pgcc

########################################################################

pgi_version := $(shell $(CXX) -V 2>&1 | grep 'target')

COMP_VERSION := $(pgi_version)

########################################################################

CXXFLAGS =
CFLAGS   =

ifeq ($(DEBUG),TRUE)

  # 2016-12-02: pgi 16.10 doesn't appear to like -traceback together with c++11

  CXXFLAGS += -g -O0 -Mbounds
  CFLAGS   += -g -O0 -Mbounds

else

  CXXFLAGS += -gopt -fast
  CFLAGS   += -gopt -fast

endif

CXXFLAGS += --c++11
CFLAGS   += -c99



GENERIC_PGI_FLAGS =

ifeq ($(USE_OMP),TRUE)
  GENERIC_PGI_FLAGS += -mp=nonuma -Minfo=mp
endif

ifeq ($(USE_ACC),TRUE)
  GENERIC_PGI_FLAGS += -acc -Minfo=acc -ta=nvidia -lcudart -mcmodel=medium
else
  GENERIC_PGI_FLAGS += -noacc
endif

ifeq ($(USE_CUDA),TRUE)
  CXXFLAGS += -Mcuda=cuda8.0
  CFLAGS   += -Mcuda=cuda8.0
endif

CXXFLAGS += $(GENERIC_PGI_FLAGS)
CFLAGS   += $(GENERIC_PGI_FLAGS)



#
# If we are using CUDA, pull in the gcc compiler first
# and override it as necessary. This is done because the
# nvcc compiler driver does not work optimally with pgc++
# as a host compiler at present.
#

ifeq ($(USE_CUDA),TRUE)
  include $(AMREX_HOME)/Tools/GNUMake/comps/gnu.mak

  CXXFLAGS := -Wno-deprecated-gpu-targets -x cu --std=c++11 -ccbin=$(CXX) -Xcompiler='$(CXXFLAGS)'
  CFLAGS := -Wno-deprecated-gpu-targets -x c -ccbin=$(CC) -Xcompiler='$(CFLAGS)'

  CXX := nvcc
  CC := nvcc

  override XTRALIBS :=
endif

override XTRALIBS += -lstdc++



#
# Now set the Fortran flags. Since this is done after the GNU include
# in the CUDA version, all of the GNU specific options are overriden.
#
  
FC  = pgfortran
F90 = pgfortran

FFLAGS   =
F90FLAGS =

ifeq ($(DEBUG),TRUE)

  # 2016-12-02: pgi 16.10 doesn't appear to like -traceback together with c++11

  FFLAGS   += -g -O0 -Mbounds -Ktrap=divz,inv -Mchkptr
  F90FLAGS += -g -O0 -Mbounds -Ktrap=divz,inv -Mchkptr

else

  FFLAGS   += -gopt -fast
  F90FLAGS += -gopt -fast
endif

ifeq ($(USE_CUDA),TRUE)
  F90FLAGS += -Mcuda=cuda8.0 -Mnomain
  FFLAGS   += -Mcuda=cuda8.0 -Mnomain
endif

########################################################################

F90FLAGS += -module $(fmoddir) -I$(fmoddir) -Mdclchk
FFLAGS   += -module $(fmoddir) -I$(fmoddir) -Mextend

########################################################################

FFLAGS   += $(GENERIC_PGI_FLAGS)
F90FLAGS += $(GENERIC_PGI_FLAGS)

########################################################################

# Because we do not have a Fortran main

ifeq ($(which_computer),$(filter $(which_computer),summit))
override XTRALIBS += -pgf90libs -L /sw/summitdev/gcc/5.4.0new/lib64/ -latomic
else
override XTRALIBS += -pgf90libs -latomic
endif
