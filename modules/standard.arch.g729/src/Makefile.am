# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -std=c99 -O2 -fPIC -Ibcg729
AM_CXXFLAGS = -Wall -O2 -fPIC -Ibcg729
AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
libexecdir=@libexecdir@/concept
libexec_PROGRAMS = standard.arch.g729.so
standard_arch_g729_so_SOURCES = stdlibrary.cpp stdlibrary.h main.cpp AnsiTypes.h pointer_list.h bcg729/LP2LSPConversion.c bcg729/cng.c bcg729/computeWeightedSpeech.c bcg729/decodeLSP.c bcg729/findOpenLoopPitchDelay.c bcg729/postFilter.c bcg729/utils.c bcg729/LPSynthesisFilter.c bcg729/codebooks.c bcg729/decodeAdaptativeCodeVector.c bcg729/decoder.c bcg729/fixedCodebookSearch.c bcg729/postProcessing.c bcg729/vad.c bcg729/LSPQuantization.c bcg729/computeAdaptativeCodebookGain.c bcg729/decodeFixedCodeVector.c bcg729/dtx.c bcg729/gainQuantization.c bcg729/preProcessing.c bcg729/adaptativeCodebookSearch.c bcg729/computeLP.c bcg729/decodeGains.c bcg729/encoder.c bcg729/interpolateqLSP.c bcg729/qLSP2LP.c