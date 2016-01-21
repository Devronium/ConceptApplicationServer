CC = g++
CCOMP = gcc
RM = del
CFLAGS = -O2 -s
LIBS =  -lwsock32 -lapr-1 -laprutil-1 -lhttpd
# -lCONCEPT_DLL -L./
#-laprutil-1 


RSA_OBJS = 	rsa_convert.o rsa_encode.o rsa_keys.o rsa_lib.o rsa_pem.o\
		bn_add.o bn_asm.o bn_ctx.o bn_div.o bn_exp.o bn_exp2.o bn_gcd.o\
		bn_lib.o bn_mod.o bn_mont.o bn_mpi.o bn_prime.o bn_rand_hmf.o\
		bn_recp.o bn_shift.o bn_sqr.o bn_word.o hmf_rand.o bn_mul.o

OBJS = ${RSA_OBJS} AES.o md5c.o mtrand.o AnsiString.o main.o

concept: ${OBJS}
	${CC} -o mod_concepthttpbridge.so -shared ${CFLAGS} ${OBJS} ${LIBS}

AES.o : AES/AES.cpp AES/AES.h
	${CC} ${CFLAGS} -c AES/AES.cpp

AnsiString.o: AnsiString.cpp AnsiString.h
	${CC} ${CFLAGS} -c AnsiString.cpp

main.o: main.cpp
	${CC} ${CFLAGS} -c main.cpp

md5c.o: md5/md5c.c md5/md5.h
	${CCOMP} ${CFLAGS} -c md5/md5c.c

mtrand.o: mtrand.cpp mtrand.h
	${CC} ${CFLAGS} -c mtrand.cpp

rsa_convert.o: rsa/rsa_convert.c
	${CCOMP} ${CFLAGS} -c rsa/rsa_convert.c

rsa_encode.o: rsa/rsa_encode.c
	${CCOMP} ${CFLAGS} -c rsa/rsa_encode.c

rsa_keys.o: rsa/rsa_keys.c
	${CCOMP} ${CFLAGS} -c rsa/rsa_keys.c

rsa_lib.o: rsa/rsa_lib.c
	${CCOMP} ${CFLAGS} -c rsa/rsa_lib.c

rsa_pem.o: rsa/rsa_pem.c
	${CCOMP} ${CFLAGS} -c rsa/rsa_pem.c

bn_add.o: bn_openssl097beta1/bn_add.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_add.c

bn_asm.o: bn_openssl097beta1/bn_asm.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_asm.c

bn_ctx.o: bn_openssl097beta1/bn_ctx.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_ctx.c

bn_div.o: bn_openssl097beta1/bn_div.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_div.c

bn_exp.o: bn_openssl097beta1/bn_exp.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_exp.c

bn_exp2.o: bn_openssl097beta1/bn_exp2.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_exp2.c

bn_gcd.o: bn_openssl097beta1/bn_gcd.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_gcd.c

bn_lib.o: bn_openssl097beta1/bn_lib.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_lib.c

bn_mod.o: bn_openssl097beta1/bn_mod.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_mod.c

bn_mont.o: bn_openssl097beta1/bn_mont.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_mont.c

bn_mpi.o: bn_openssl097beta1/bn_mpi.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_mpi.c

bn_mul.o: bn_openssl097beta1/bn_mul.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_mul.c

bn_prime.o: bn_openssl097beta1/bn_prime.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_prime.c

bn_rand_hmf.o: bn_openssl097beta1/bn_rand_hmf.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_rand_hmf.c

bn_recp.o: bn_openssl097beta1/bn_recp.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_recp.c

bn_shift.o: bn_openssl097beta1/bn_shift.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_shift.c

bn_sqr.o: bn_openssl097beta1/bn_sqr.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_sqr.c

bn_word.o: bn_openssl097beta1/bn_word.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/bn_word.c

hmf_rand.o: bn_openssl097beta1/hmf_rand.c
	${CCOMP} ${CFLAGS} -c bn_openssl097beta1/hmf_rand.c

clean:
	$(RM) mod_concept.so ${OBJS}
	@echo all cleaned up!