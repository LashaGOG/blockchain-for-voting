#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "primalite.h"
#include "protocole_rsa.h"
#include "keys.h"
#include "generate_data.h"
#include "rw_data.h"

long extended_gcd (long s, long t, long *u, long *v){
	if(s==0){
		*u=0;
		*v=1;
		return t;
	}
	long uPrim, vPrim;
	long gcd=extended_gcd(t%s, s, &uPrim, &vPrim);
	*u=vPrim-(t/s)*uPrim;
	*v=uPrim;
	return gcd;
}

void generate_keys_values(long p, long q, long *n, long *s, long *u){
	*n=p*q;
	long t=(p-1)*(q-1);
	long v;
	*s=rand_long(1,t);
	while(extended_gcd(*s,t,u,&v)!=1){
		*s=rand_long(1,t);
	}
}

long *encrypt(char *chaine, long s, long n){ // Ne pas oublier de free le résultat quand plus besoin
	int taille=strlen(chaine);
	long *encrypt = (long *)(malloc((taille+1)*sizeof(long)));
	if(!encrypt){
		printf("Erreur d'allocation (protocole_rsa.c:36)\n");
		return NULL;
	}
	for(int i=0;i<taille+1;i++){
		encrypt[i]=mod_pow(chaine[i],s,n);
	}
	return encrypt;
}

char *decrypt(long *crypted, int size, long u, long n){ // Ne pas oublier de free le résultat quand plus besoin
	char *decrypt = (char *)(malloc((size+1)*sizeof(char)));
	if(!decrypt){
		printf("Erreur d'allocation (protocole_rsa.c:48)\n");
		return NULL;
	}
	for(int i=0;i<size;i++){
		decrypt[i]=mod_pow(crypted[i],u,n);
	}
	decrypt[size]='\0';
	return decrypt;
}

void print_long_vector ( long * result , int size ) {
	printf ("Vector:[");
	for ( int i =0; i < size ; i ++) {
		printf ("%ld\t", result [ i ]);
	}
	printf ("]\n");
}