#include <stdio.h>
#include "primality.h"

int is_prime_naive (long p) {
    /* Etant donné un entier p, renvoie 1 si p est premier, 0 sinon */
    if (p <= 2) return 0; 

    for (int i = 2; i < p; i++) {
        if (p % i == 0) return 0; 
    }

    return 1; 
}
