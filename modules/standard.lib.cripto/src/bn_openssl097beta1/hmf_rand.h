#ifndef _INCLUDED_HMF_RAND_H_
#define _INCLUDED_HMF_RAND_H_

// ==== bbs ==== NOT USED
unsigned long isPrime32(unsigned long value);
unsigned long pseudorandom_BBS32_preinit(unsigned short p, unsigned short q);
unsigned long pseudorandom_BBS32_init(unsigned long init);
unsigned long pseudorandom_BBS32_random_value();
unsigned long pseudorandom_BBS32_random_bit(unsigned long totalBit);

// ==== pseudo random number generator linear congruent .. full period
unsigned long random32_bit(unsigned long totalBit);
unsigned long random32_value();
void random32_init(unsigned long seed);
#endif
