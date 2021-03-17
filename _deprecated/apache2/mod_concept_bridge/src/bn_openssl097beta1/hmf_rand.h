#ifndef _INCLUDED_HMF_RAND_H_
#define _INCLUDED_HMF_RAND_H_

// ==== bbs ==== NOT USED
unsigned int isPrime32(unsigned int value);
unsigned int pseudorandom_BBS32_preinit(unsigned short p, unsigned short q);
unsigned int pseudorandom_BBS32_init(unsigned int init);
unsigned int pseudorandom_BBS32_random_value();
unsigned int pseudorandom_BBS32_random_bit(unsigned int totalBit);

// ==== pseudo random number generator linear congruent .. full period
unsigned int random32_bit(unsigned int totalBit);
unsigned int random32_value();
void random32_init(unsigned int seed);
#endif
