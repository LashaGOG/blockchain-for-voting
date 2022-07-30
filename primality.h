#ifndef PRIMALITY
#define PIRMALITY 

#include <stdlib.h>

int is_prime_naive (long p); 
long mod_pow_naive (long a, long m, long n); 
int modpow (long a, long m, long n); 
int witness (long a, long b, long d, long p); 
long rand_long (long low, long up); 
int is_prime_miller (long p, int k);

#endif //PRIMALITY