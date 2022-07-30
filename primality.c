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

int witness (long a, long b, long d, long p) 
{
    /* teste si a est est un témoin de Miller pour p, pour un entier a donné */
    long x = modpow (a, d, p) ;
    if (x == 1) 
    {
        return 0;
    } 
    for (long i = 0; i < b; i++) 
    {
        if (x == p-1) 
        {
            return 0; 
        }
        x = modpow(x,2,p);
    }
    return 1; 
}

long rand_long (long low, long up) 
{
    /* retourne un entier long généré aléatoirement entre low et up inclus */ 
    return rand() % (up - low + 1) + low ; 
}

int is_prime_miller (long p, int k) 
{
    /* 
    * réalise le test de Miller-Rabin en générant k valeurs
    * de a au hasard, et en testant si chaque valeur de a est un témoin de Miller pour p. La fonction
    * retourne 0 dès qu’un témoin de Miller est trouvé (p n’est pas premier), et retourne 1 si aucun
    * témoin de Miller n’a été trouvé (p est très probablement premier)
    */

   if (p == 2) 
   {
        return 1; 
   }
   if (!(p & 1) || p <= 1) // on vérifie que p est impair et different de 1
   {
        return 0; 
   }
   // on determine b et d :
   long b = 0 ; 
   long d = p - 1 ;
   while (!(d & 1)) // tant que d n'est pas impair
   {
        d = d/2 ; 
        b = b + 1 ; 
   }
   // on genere k valeurs pour a, et on teste si c'est un temoin : 
   long a; 
   int i; 
   for (i = 0; i < k; i++) {
        a = rand_long(2, p-1); 
        if (witness(a,b,d,p)) 
        {
            return 0; 
        }
    }
    return 1; 
}

int main (void) 
{
    return 1; 
}