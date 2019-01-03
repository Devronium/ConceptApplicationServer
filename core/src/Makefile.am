# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC -fno-strict-aliasing -fomit-frame-pointer -DCOMPUTED_GOTO
AM_CXXFLAGS = -O2 -fPIC -fno-strict-aliasing -fomit-frame-pointer -DCOMPUTED_GOTO
#AM_LDFLAGS = -shared

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libconceptcore.2.0.la
libconceptcore_2_0_la_SOURCES = AnsiException.cpp AnsiList.cpp AnsiParser.cpp AnsiString.cpp Array.cpp ClassCode.cpp ClassMember.cpp Codes.cpp CompiledClass.cpp ConceptInterpreter.cpp Debugger.cpp GarbageCollector.cpp main.cpp ModuleLink.cpp Optimizer.cpp PIFAlizator.cpp TinyString.cpp SHManager.cpp StaticString.cpp StaticList.cpp DoubleList.cpp BuiltIns.cpp HashTable.c MemoryTable.c JIT/sljitLir.c builtin/regexp.c dlmalloc.c simple/plainstring.c
