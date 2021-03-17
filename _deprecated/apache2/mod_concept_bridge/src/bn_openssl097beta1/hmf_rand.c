#include <math.h> // sqrt()
#include "hmf_prime.h"
#include "hmf_rand.h"

static unsigned int N = 0;
static unsigned /*__int64*/ long long X = 0;

/*	check whether the value is prime or not
 *      return zero if prime else return the divider
 *      This function uses normal divider by small prime because
 *      the value to check is small (32-bits), another special
 *      algorithm is required to check a greater value	*/

/*
 * unsigned long isPrime32(unsigned long value)
 * {
 *      unsigned long limit;
 *      unsigned long ul;
 *      unsigned long result = 0;
 *
 *      // check for even value
 *      if(!(value & 1))
 *      {
 *              if(value == 2)
 *                      return(0); // 2 = prime .. the ONLY even number that is prime !!!
 *              else
 *                      return(2); // it is an even number .. NOT a prime
 *      }
 *
 *      // get limit
 *      limit = (unsigned long) sqrt(value);
 *      // check it by its limit
 *      if(limit * limit == value)
 *              return(limit);
 *
 *      for(ul = 1; ul <= 6542; ul++)
 *      {
 *              result = value % first6542prime[ul];
 *              if(!result)
 *              {
 *                      if(value == first6542prime[ul])
 *                              return(0);
 *                      else
 *                              return(first6542prime[ul]);
 *              }
 *
 *              // check for limit
 *              if(first6542prime[ul] >= limit)
 *                      return(0);
 *
 *      }
 *
 *      // this line should never be reached
 *      // if reached, it means (one of below)
 *      // 1. list of prime is NOT enough
 *      // 2. this algorithm is wrong (bug)
 *      return(0);
 * }
 *
 * unsigned long pseudorandom_BBS32_preinit(unsigned short p, unsigned short q)
 * {
 *      int remainder;
 *
 *      //check prime
 *      if(isPrime32(p))
 *              return(0);
 *
 *      if(isPrime32(q))
 *              return(0);
 *
 *      remainder = (p % 4);
 *      if(remainder != 3)
 *              return(0);
 *
 *      if(remainder != (q % 4))
 *              return(0); // failed
 *
 *      N = p * q;
 *
 *      return(N);
 * }
 *
 * unsigned long pseudorandom_BBS32_init(unsigned long init)
 * {
 *      unsigned long prevX;
 *
 *      prevX = (unsigned long) X;
 *
 *      // rule no 1
 *      if(init < sqrt(N))
 *              return(0);
 *
 *      // rule no 2
 *      if(init > N)
 *              return(0);
 *
 *      X = init;
 *
 *      // return previous value of X
 *      // plus 1 to avoid zero because 0 = error
 *      return(prevX + 1);
 * }
 *
 * unsigned long pseudorandom_BBS32_random_value()
 * {
 *      X = (X * X) % N;
 *
 *      return((unsigned long) X);
 * }
 *
 * unsigned long pseudorandom_BBS32_random_bit(unsigned long totalBit)
 * {
 *      unsigned long value;
 *      unsigned long i;
 *
 *      if(totalBit < 1)
 *              return(0);
 *
 *      if(totalBit > 32)
 *              return(0);
 *
 *      //get first bit
 *      value = (pseudorandom_BBS32_random_value() & 1);
 *
 *      //get the rest of the bit
 *      for(i = 1; i < totalBit; i++)
 *      {
 *              value <<= 1;
 *              value |= (pseudorandom_BBS32_random_value() & 1);
 *      }
 *
 *      return(value);
 * }
 */
//===========================================end of bbs

//===========================================start of linear congruent
// X = (16807 * X) % 2147483647
// a full period value ...
const unsigned int  modulus    = 2147483647;
const unsigned int  multiplier = 16807;
static unsigned int lastValue;

unsigned int random32_bit(unsigned int totalBit) {
    unsigned int value;
    unsigned int i;

    value = 0;
    for (i = 0; i < totalBit; i++) {
        value <<= 1;
        value  |= (random32_value() & 1);
    }

    return value;
}

unsigned int random32_value() {
    lastValue = (multiplier * lastValue) % modulus;

    return lastValue;
}

void random32_init(unsigned int seed) {
    lastValue = seed;
}
