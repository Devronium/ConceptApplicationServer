# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -O3 -fPIC
AM_CXXFLAGS = -O3 -fPIC
AM_LDFLAGS = -shared -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.lib.vad.so
standard_lib_vad_so_SOURCES = libfvad/src/fvad.c libfvad/include/fvad.h libfvad/src/signal_processing/division_operations.c libfvad/src/signal_processing/energy.c libfvad/src/signal_processing/get_scaling_square.c libfvad/src/signal_processing/resample_48khz.c libfvad/src/signal_processing/resample_by_2_internal.c libfvad/src/signal_processing/resample_fractional.c libfvad/src/signal_processing/spl_inl.c libfvad/src/vad/vad_core.c libfvad/src/vad/vad_filterbank.c libfvad/src/vad/vad_gmm.c libfvad/src/vad/vad_sp.c stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h
