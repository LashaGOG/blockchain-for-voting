#include <stdio.h>
#include "primality.h"

int is_prime_naive (long p) 
{
    /* etant donné un entier p, renvoie 1 si p est premier, 0 sinon */
    if (p <= 2) return 0; 

    for (int i = 2; i < p; i++) 
    {
        if (p % i == 0) return 0; 
    }

    return 1; 
}

long mod_pow_naive (long a, long m, long n) 
{
    /* retourne a^m mod n par la methode naive en supposant m >= 0 */

    long res = 1 ;
    int i ; 

    for (i = 0; i < m; i++) 
    {
        res *= a;
        res %= n;  
    }
    return res; 
}

int modpow (long a, long m, long n) 
{
    /* la version rapide (de complexité log) de la fonction mod_pow_naive */

    // Les cas de base 
    if (a == 0) return 0; 
    if (m == 0) return 1;
    
    // si m est pair
    long b; 
    if (m % 2 == 0) 
    {
        b = modpow (a, m/2, n);
        return (b * b) % n ; 
    }

    // si m est impair

    else
    {
        b = modpow(a, (m-1)/2, n); 
        return (a * b * b) % n ; 
    }
}

