#ifndef MANIP_KEYS
#define MANIP_KEYS

#include <stdlib.h>
#include <string.h>
#include "protocole_rsa.h"

typedef struct _key
{
    long val; // val represent u ou v
    long n;   // n
} Key;

typedef struct _signature
{
    // la signature sous la forme de la tableau tab de long et sa taille
    long *tab;
    int taille;
} Signature;

typedef struct _protected
{
    Key *pKey;
    Signature *signature;
    char *message;
} Protected;


void init_key(Key *key, long val, long n);
void init_pair_keys(Key *pKey, Key *sKey, long low_size, long up_size);
char *key_to_str(Key *key);
Key *str_to_key(char *str);

Signature *init_signature(long *content, int size);
Signature *sign(char *mess, Key *sKey);
char *signature_to_str(Signature *sgn);
Signature *str_to_signature(char *str);

Protected *init_protected(Key *pKey, char *mess, Signature *sgn);
int verify(Protected *pr);
char *protected_to_str(Protected *pr);
Protected *str_to_protected (char *chaine);


#endif //MANIP_KEYS