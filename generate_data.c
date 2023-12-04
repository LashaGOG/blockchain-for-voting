#include <stdio.h>
#include <stdlib.h>

#include "protocole_rsa.h" 
#include "primalite.h" 
#include "keys.h" 
#include "generate_data.h"
#include "rw_data.h"

void generate_random_data(int nv, int nc){
	Key tab_couple[nv][2];
	for(int i=0;i<nv;i++){
		init_pair_keys(&tab_couple[i][0],&tab_couple[i][1],5,15);
		for(int j=0;j<i;j++){
			if((tab_couple[i][0].val==tab_couple[j][0].val && tab_couple[i][0].n==tab_couple[i][0].n) || (tab_couple[i][1].val==tab_couple[j][1].val && tab_couple[i][1].n==tab_couple[j][1].n)){
				i--;
				continue;
			}
		}
	}
	
	FILE *fichier = fopen("keys.txt","w");
    if(!fichier){
        printf("Erreur d'ouverture de fichier (generate_data.c:24)\n");
        return;
    }
	for(int i=0;i<nv;i++){
		char *temp1=key_to_str(&tab_couple[i][0]);
		char *temp2=key_to_str(&tab_couple[i][1]);
		fprintf(fichier,"%s %s\n",temp1,temp2);
		free(temp1);
		free(temp2);
	}
	if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (generate_data.c:34)\n");
        exit(-1);
    }
	
	Key tab_candidat[nc][2];
	int alea[nc];
	for(int i=0;i<nc;i++){
		alea[i] = rand()%nv;
		for(int j=0;j<i;j++){
			if(alea[j]==alea[i]){
				i--;
				continue;
			}
		}
		tab_candidat[i][0].val=tab_couple[alea[i]][0].val;
		tab_candidat[i][1].val=tab_couple[alea[i]][1].val;
		tab_candidat[i][0].n=tab_couple[alea[i]][0].n;
		tab_candidat[i][1].n=tab_couple[alea[i]][1].n;
	}
	
	FILE *fichier2 = fopen("candidates.txt","w");
    if(!fichier2){
        printf("Erreur d'ouverture de fichier (generate_data.c:55)\n");
        return;
    }
	for(int i=0;i<nc;i++){
		char *temp1=key_to_str(&tab_candidat[i][0]);
		char *temp2=key_to_str(&tab_candidat[i][1]);
		fprintf(fichier2,"%s %s\n",temp1,temp2);
		free(temp1);
		free(temp2);
	}
	if(fclose(fichier2)==EOF){
        printf("Erreur de fermeture de fichier (generate_data.c:67)\n");
        exit(-1);
    }
	
	Protected *tab_vote[nv];
	int candidalea;
	for(int i=0;i<nv;i++){
		candidalea=rand()%nc;
		char *temp1=key_to_str(&tab_candidat[candidalea][0]);
		char *temp2=key_to_str(&tab_candidat[candidalea][0]);
        Signature *signtemp=sign(temp2, &tab_couple[i][1]);
		tab_vote[i]=init_protected(&tab_couple[i][0], temp1, signtemp);
		free(temp1);
		free(temp2);
        free(signtemp->tab);
        free(signtemp);
	}
	
	FILE *fichier3 = fopen("declarations.txt","w");
    if(!fichier3){
        printf("Erreur d'ouverture de fichier (generate_data.c:86)\n");
        return;
    }
	for(int i=0;i<nv;i++){
		char *temp=protected_to_str(tab_vote[i]);
		fprintf(fichier3,"%s\n",temp);
		free(temp);
	}
	if(fclose(fichier3)==EOF){
        printf("Erreur de fermeture de fichier (generate_data.c:96)\n");
        exit(-1);
    }
	
	for(int i=0;i<nv;i++){
		free(tab_vote[i]->message);
        free(tab_vote[i]->pKey);
        free(tab_vote[i]->signature->tab);
        free(tab_vote[i]->signature);
        free(tab_vote[i]);
	}
}
