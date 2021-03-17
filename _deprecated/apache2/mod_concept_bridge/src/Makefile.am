# what flags you want to pass to the C compiler & linker
prefix = @prefix@

AM_CFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -DLIBRARY_PATH=\"@prefix@/lib/libconceptcore.1.0.so\" -DINI_PATH=\"@prefix@/etc/concept.ini\" -I@apache_dir@/include
AM_CXXFLAGS = -O2 -DLINUX_PATH=\"@prefix@/bin/\" -DLIBRARY_PATH=\"@prefix@/lib/libconceptcore.1.0.so\" -DINI_PATH=\"@prefix@/etc/concept.ini\" -I@apache_dir@/include
#AM_LDFLAGS = -shared -lpthread -lhttpd -lapr-1
#AM_LDFLAGS = -lpthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libmod_concepthttpbridge.la
libmod_concepthttpbridge_la_SOURCES = AnsiString.cpp main.cpp AnsiString.h mtrand.cpp mtrand.h semhh.h AES/AES.cpp AES/AES.h bn_openssl097beta1/bn_add.c bn_openssl097beta1/bn_asm.c bn_openssl097beta1/bn_ctx.c bn_openssl097beta1/bn_div.c bn_openssl097beta1/bn_exp.c bn_openssl097beta1/bn_exp2.c bn_openssl097beta1/bn_gcd.c bn_openssl097beta1/bn_lib.c bn_openssl097beta1/bn_mod.c bn_openssl097beta1/bn_mont.c bn_openssl097beta1/bn_mpi.c bn_openssl097beta1/bn_mul.c bn_openssl097beta1/bn_prime.c bn_openssl097beta1/bn_rand_hmf.c bn_openssl097beta1/bn_recp.c bn_openssl097beta1/bn_shift.c bn_openssl097beta1/bn_sqr.c bn_openssl097beta1/bn_word.c bn_openssl097beta1/hmf_rand.c bn_openssl097beta1/bn.h bn_openssl097beta1/bn_lcl.h bn_openssl097beta1/bn_prime.h bn_openssl097beta1/hmf_prime.h bn_openssl097beta1/hmf_rand.h rsa/rsa_convert.c rsa/rsa_encode.c rsa/rsa_keys.c rsa/rsa_lib.c rsa/rsa_pem.c rsa/rsa.h md5/md5.h md5_global.h md5c.c