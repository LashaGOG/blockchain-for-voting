#ifndef PROTOCOLE_RSA
#define PROTOCOLE_RSA

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "primalite.h"

long extended_gcd(long s, long t, long *u, long *v);
void generate_keys_values(long p, long q, long* n, long* s, long* u);
long *encrypt(char *chaine, long s, long n);
char *decrypt(long* crypted, int size, long u, long n);
void print_long_vector(long *result, int size);

#endif //PROTOCOLE_RSA