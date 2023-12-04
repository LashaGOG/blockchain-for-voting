#include <stdio.h>
#include <stdlib.h>

#include "protocole_rsa.h" 
#include "primalite.h" 
#include "keys.h"
#include "generate_data.h"
#include "rw_data.h"

void init_key(Key* key, long val, long n){
	key->val=val;
	key->n=n;
}

void init_pair_keys(Key* pKey, Key* sKey, long lowsize, long upsize){
	long p=random_prime_number(lowsize ,upsize , 5000);
	long q=random_prime_number(lowsize ,upsize , 5000);
	while(p==q){
		q=random_prime_number(lowsize ,upsize , 5000);
	}
	long n, s, u;
	generate_keys_values (p, q, &n, &s, &u);
	//Pour avoir des cles positives :
	if (u<0) {
		long t=(p-1)*(q-1);
		u=u+t; //on aura toujours s*u mod t = 1
	}
	init_key(pKey,s,n);
	init_key(sKey,u,n);
}

char* key_to_str(Key* key){ // Ne pas oublier de free le résultat quand plus besoin
	char c1[20];
	char c2[20];
	sprintf(c1,"(%lx,",key->val);
	sprintf(c2,"%lx)",key->n);
	
	int const taille = strlen(c1) + strlen(c2);
	
	char *chaine = (char *)(malloc((taille+1)*sizeof(char)));
    if(!chaine){
        free(chaine);
        printf("Erreur d'allocation (keys.c:40)\n");
        return NULL;
    }
	strcpy(chaine,c1);
	strcat(chaine,c2);
	
	return chaine;
}

Key* str_to_key(char *str){ // Ne pas oublier de free le résultat quand plus besoin
	long val,n;
	sscanf(str,"(%lx,%lx)",&val,&n);
	Key *clef = (Key *)(malloc(sizeof(Key)));
    if(!clef){
        free(clef);
        printf("Erreur d'allocation (keys.c:55)\n");
        return NULL;
    }
	init_key(clef,val,n);
	return clef;
}

Signature* init_signature(long* content, int size){ // Ne pas oublier de free le résultat ainsi que tab quand plus besoin
	Signature *sign = (Signature *)(malloc(sizeof(Signature)));
    if(!sign){
        free(sign);
        printf("Erreur d'allocation (keys.c:66)\n");
        return NULL;
    }
	sign->taille=size;
	sign->tab=(long *)(malloc(size*sizeof(long)));
    if(!sign->tab){
        free(sign->tab);
        free(sign);
        printf("Erreur d'allocation (keys.c:73)\n");
        return NULL;
    }
	for(int i=0;i<size;i++){
		sign->tab[i]=content[i];
	}
	return sign;
}

Signature* sign(char* mess, Key* sKey){
	int taille = strlen(mess);
	long *tab=encrypt(mess,sKey->val,sKey->n);
	Signature *sign=init_signature(tab,taille);
    if(!sign){
        free(sign);
        free(tab);
        printf("Erreur d'allocation (keys.c:89)\n");
        return NULL;
    }
	free(tab);
	return sign;
}

char *signature_to_str(Signature *sgn){ // Ne pas oublier de free le résultat quand plus besoin
	char *result=(char *)(malloc(20*sgn->taille*sizeof(char)));
    if(!result){
        free(result);
        printf("Erreur d'allocation (keys.c:101)\n");
        return NULL;
    }
	result[0]='#';
	int pos=1;
	char buffer[256];
	for(int i=0;i<sgn->taille;i++){
		sprintf(buffer,"%lx",sgn->tab[i]) ;
		for(int j=0;(unsigned long)j<strlen(buffer);j++){
			result[pos]=buffer[j];
			pos=pos+1;
		}
		result[pos]='#';
		pos=pos+1;
	}
	result[pos]='\0' ;
	result=(char*)(realloc(result,(pos+1)*sizeof(char)));
    if(!result){
        free(result);
        printf("Erreur de réallocation (keys.c:120)\n");
        return NULL;
    }
	return result ;
}

Signature *str_to_signature(char *str){
	int len=strlen(str);
	long *content=(long *)malloc(sizeof(long)*len);
    if(!content){
        free(content);
        printf("Erreur d'allocation (keys.c:131)\n");
        return NULL;
    }
	int num=0;
	char buffer[256];
	int pos=0;
	for(int i=0;i<len;i++){
		if(str[i]!='#') {
			buffer[pos]=str[i];
			pos=pos+1;
		} 
		else {
			if (pos!=0){
				buffer[pos]='\0';
				sscanf(buffer, "%lx", &(content[num]));
				num=num+1;
				pos=0;
			}
		}
	}
	content=(long *)(realloc(content, num*sizeof(long)));
    if(!content){
        free(content);
        printf("Erreur de réallocation (keys.c:154)\n");
        return NULL;
    }
	Signature *signa=init_signature(content, num);
    free(content);
    return signa;
}

Protected *init_protected(Key *pKey, char *mess, Signature *sgn){ //Ne pas oublier de free la clef le message la signature et la structure
	Protected *protec = (Protected *)(malloc(sizeof(Protected)));
    if(!protec){
        free(protec);
        printf("Erreur d'allocation (keys.c:165)\n");
        return NULL;
    }

    protec->message=strdup(mess);
    if(!protec->message){
        free(protec->message);
        free(protec);
        printf("Erreur d'allocation (keys.c:173)\n");
        return NULL;
    }
    
    Key *clef=(Key *)(malloc(sizeof(Key)));
    if(!clef){
        free(clef);
        free(protec->message);
        free(protec);
        printf("Erreur d'allocation (keys.c:181)\n");
        return NULL;
    }
    init_key(clef,pKey->val,pKey->n);
    protec->pKey=clef;

    Signature *signa=init_signature(sgn->tab,sgn->taille);
    protec->signature=signa;

    return protec;
}

int verify(Protected* pr){
	char *messdec=decrypt(pr->signature->tab,strlen(pr->message),pr->pKey->val,pr->pKey->n);
	if(strcmp(messdec,pr->message)==0){
		free(messdec);
		return 1;
	}
	free(messdec);
	return 0;
}

char *protected_to_str(Protected* pr){ //Ne pas oublier de free la chaine
	char *c1=key_to_str(pr->pKey);
	char *c2=signature_to_str(pr->signature);
	int taille=strlen(c1);
	taille+=strlen(c2);
	taille+=strlen(pr->message);
	char *chaine = (char *)(malloc((taille+3)*sizeof(char)));
    if(!chaine){
        free(chaine);
        printf("Erreur d'allocation (keys.c:222)\n");
    }
	strcpy(chaine,c1);
	strcat(chaine," ");
	strcat(chaine,pr->message);
	strcat(chaine," ");
	strcat(chaine,c2);
	free(c1);
    free(c2);
	return chaine;
}

Protected *str_to_protected(char *chaine){ //Ne pas oublier de free le résultat et ses composants
	
	// On prend la 1ère partie de la chaine
	int i=0;
	while(chaine[i]!=' '){
		i++;
	}
	char c1[i+1];
	for(int k=0;k<i;k++){
		c1[k]=chaine[k];
	}
	c1[i]='\0';
	
	// On prend la 2ème partie de la chaine
	int j=i+1;
	while(chaine[j]!=' '){
		j++;
	}
	char c2[j-i];
	for(int k=i+1;k<j;k++){
		c2[k-(i+1)]=chaine[k];
	}
	c2[j-i-1]='\0';
	
	// On prend la 3ème partie de la chaine
	i=j+1;
	while(chaine[i]!='\0'){
		i++;
	}
	char c3[i-j];
	for(int k=j+1;k<i;k++){
		c3[k-(j+1)]=chaine[k];
	}
	c3[i-j-1]='\0';
	Protected *pr = (Protected *)(malloc(sizeof(Protected)));
    if(!pr){
        free(pr);
        printf("Erreur d'allocation (keys.c:263)\n");
        return NULL;
    }
	pr->pKey=str_to_key(c1);
	pr->message=strdup(c2);
	pr->signature=str_to_signature(c3);
	return pr;
}