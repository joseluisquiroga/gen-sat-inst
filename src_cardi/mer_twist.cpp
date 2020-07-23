

/*************************************************************

mer_twist.cpp  
A C-program for MT19937, with initialization improved 2002/1/26.
Coded by Takuji Nishimura and Makoto Matsumoto.

Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
All rights reserved.                          

Adapted to c++ by 
(C 2010) QUIROGA BELTRAN, Jose Luis. Bogotá - Colombia.

Date of birth: December 28 of 1970.
Place of birth: Bogota - Colombia - Southamerica.
Id (cedula): 79523732 de Bogota.
email: JoseLuisQuiroga@yahoo.com

Classes and that implement the random number generator of 
Makoto and Takuji.

--------------------------------------------------------------*/

/* 
	mer_twist.cpp

   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include "mer_twist.h"

/* initializes mt[MER_TWIST_N] with a seed */
void 
mer_twist::init_with_long(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MER_TWIST_N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void 
mer_twist::init_with_array(const unsigned long* init_key, int key_length)
{
    int i, j, k;
	
    init_with_long(MER_TWIST_INIT_LONG_1);
    i=1; j=0;
    k = (MER_TWIST_N>key_length ? MER_TWIST_N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=MER_TWIST_N) { mt[0] = mt[MER_TWIST_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=MER_TWIST_N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=MER_TWIST_N) { mt[0] = mt[MER_TWIST_N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long 
mer_twist::gen_rand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MER_TWIST_MATRIX_A};
    /* mag01[x] = x * MER_TWIST_MATRIX_A  for x=0,1 */

    if (mti >= MER_TWIST_N) { /* generate MER_TWIST_N words at one time */
        int kk;

        if (mti == MER_TWIST_N+1)   /* if init_with_long() has not been called, */
            init_with_long(MER_TWIST_INIT_LONG_2); /* a default initial seed is used */

        for (kk=0;kk<MER_TWIST_N-MER_TWIST_M;kk++) {
            y = (mt[kk]&MER_TWIST_UPPER_MASK)|(mt[kk+1]&MER_TWIST_LOWER_MASK);
            mt[kk] = mt[kk+MER_TWIST_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MER_TWIST_N-1;kk++) {
            y = (mt[kk]&MER_TWIST_UPPER_MASK)|(mt[kk+1]&MER_TWIST_LOWER_MASK);
            mt[kk] = mt[kk+(MER_TWIST_M-MER_TWIST_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MER_TWIST_N-1]&MER_TWIST_UPPER_MASK)|(mt[0]&MER_TWIST_LOWER_MASK);
        mt[MER_TWIST_N-1] = mt[MER_TWIST_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,1]-real-interval */
double 
mer_twist::gen_rand_i0_i1(void)
{
    return gen_rand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double 
mer_twist::gen_rand_i0_e1(void)
{
    return gen_rand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double 
mer_twist::gen_rand_e0_e1(void)
{
    return (((double)gen_rand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* These real versions are due to Isaku Wada, 2002/01/09 added */
/*
int main(void)
{
    int i;
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_with_array(init, length);
    printf("1000 outputs of gen_rand_int32()\n");
    for (i=0; i<1000; i++) {
      printf("%10lu ", gen_rand_int32());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of gen_rand_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", gen_rand_real2());
      if (i%5==4) printf("\n");
    }
    return 0;
}
*/

