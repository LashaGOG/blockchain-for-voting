#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "primalite.h"
#include "protocole_rsa.h"
#include "keys.h"
#include "generate_data.h"
#include "rw_data.h"


int is_prime_naive(long p){
	for(int i=3;i<p;i+=2){
		if(p%i==0){
			return 0;
		}
	}
	return 1;
}

long mod_pow_naive(long a, long m, long n){
	long mod=1;
	for(int i=0;i<m;i++){
		mod*=a;
		mod=mod%n;
	}
	return mod;
}

long mod_pow(long a, long m, long n) {
	long p;
	for (p=1;m>0;m=m/2) {
		if (m%2!=0){
			p=(p*a)%n;
		}
		a=(a*a)%n;
	}
	return p;
}

int witness (long a, long b,long d, long p){
	long x=mod_pow(a, d, p);
	if(x==1){
		return 0;
	}
	for(long i=0;i<b;i++){
		if(x==p-1){
			return 0;
		}
		x=mod_pow(x, 2, p);
	}
	return 1;
}

long rand_long (long low, long up){
	return rand()%(up-low+1)+low;
}

int is_prime_miller (long p, int k){
	if(p==2){
		return 1;
	}
	if(!(p&1)||p<=1){ //on verifie que p est impair et different de 1
		return 0;
	}
	//on determine b et d :
	long b=0;
	long d=p-1;
	while(!(d&1)){ //tant que d n’est pas impair
		d=d/2;
		b=b+1;
	}
	// On genere k valeurs pour a, et on teste si c’est un temoin :
	long a ;
	int i ;
	for(i=0;i<k;i++){
		a=rand_long(2, p-1);
		if(witness(a , b, d, p)){
			return 0;
		}
	}
	return 1;
}

long random_prime_number(int low_size, int up_size, int k){
	long min=1,max=1;
	long p;
	for(int i=0;i<low_size;i++){
		min*=2;
	}
	max=min;
	for(int i=low_size-1;i<up_size;i++){
		max*=2;
	}
	max-=1;
	do{
		p=rand_long(min,max);
	}
	while(is_prime_miller(p,k)==0);
	return p;
}