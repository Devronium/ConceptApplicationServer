# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2 -fPIC -DTLS_AMALGAMATION -DTLS_WITH_CHACHA20_POLY1305 -DTLS_CLIENT_ECDSA -DTLS_PREFER_CHACHA20
AM_CXXFLAGS = 
AM_LDFLAGS = 

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libconceptclient.la
libconceptclient_la_SOURCES = ConceptClient.cpp ConceptClient.h AnsiString.cpp AnsiString.h messages.cpp messages.h AnsiList.cpp AnsiList.h Codes.cpp Codes.h tlse.c tlse.h tls_root_ca.h libconceptclient.cpp libconceptclient.h
