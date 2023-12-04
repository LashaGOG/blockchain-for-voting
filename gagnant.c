#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rw_data.h"
#include "gagnant.h"

void suppr_decla_invalide(CellProtected **cp){
    CellProtected *tempcell=*cp, *tempcell2=*cp;

    if(!cp){
        return;
    }

    while(verify((*cp)->data)==0){
        *cp=(*cp)->next;
        delete_cell_protected(tempcell);
        tempcell=*cp;
        tempcell2=*cp;
    }

    while(tempcell->next){
        if(verify(tempcell->next->data)==0){
            if(tempcell->next->next){
                tempcell2=tempcell->next;
                tempcell->next=tempcell->next->next;
                delete_cell_protected(tempcell2);
            }
            else{
                delete_cell_protected(tempcell->next);
                tempcell->next=NULL;
            }
            continue;
        }
        tempcell=tempcell->next;
    }
}

HashCell *create_hashcell(Key* key){
    HashCell *ashselle=(HashCell *)(malloc(sizeof(HashCell)));
    if(!ashselle){
        free(ashselle);
        printf("Erreur d'allocation (gagnant.c:8)\n");
        return NULL;
    }

    Key *clef=(Key *)(malloc(sizeof(Key)));
    if(!clef){
        free(clef);
        printf("Erreur d'allocation (gagnant.c:15)\n");
        return NULL;
    }

    init_key(clef,key->val,key->n);

    ashselle->key=clef;
    ashselle->val=0;

    return ashselle;
}

int hash_function(Key* key, int size, int i){
    double A=(sqrt(5)-1)/2; //nombre d’or diminué de 1
    long k=key->val;
    int pos=(int)(((int)(size*(k*A-(int)(k*A)))+(int)(i/2)+(int)(i*i/2))%size); 

    return pos;
}

int find_position(HashTable* t, Key* key){
    int cpt=0;
    int i=hash_function(key,t->size,cpt);
    int j=i;
    if((t->tab[i]->key->n==key->n) && (t->tab[i]->key->val==key->val)){
        return i;
    }
    cpt++;
    i=hash_function(key,t->size,cpt);
    while(((t->tab[i]->key->n!=key->val) || (t->tab[i]->key->val!=key->val)) && i!=j){
        cpt++;
        i=hash_function(key,t->size,cpt);
    }
    return i;
}

HashTable* create_hashtable(CellKey* keys, int size){
    HashTable *ashtable=(HashTable *)(malloc(sizeof(HashTable)));
    if(!ashtable){
        free(ashtable);
        printf("Erreur d'allocation (gagnant.c:80)\n");
        return NULL;
    }
    ashtable->size=size;

    HashCell **ashselle=(HashCell **)(malloc(size*sizeof(HashCell*)));
    if(!ashselle){
        free(ashselle);
        printf("Erreur d'allocation (gagnant.c:87)\n");
        return NULL;
    }

    for(int i=0;i<size;i++){
        ashselle[i]=NULL;
    }

    CellKey *temp=keys;
    while(temp){
        int cpt=0;
        int pos=hash_function(temp->data,size,cpt);
        while(ashselle[pos]){
            cpt++;
            pos=hash_function(temp->data,size,cpt);
        }
        ashselle[pos]=create_hashcell(temp->data);
        temp=temp->next;
    }

    ashtable->tab=ashselle;
    return ashtable;
}

void delete_hascell(HashCell *h){
    free(h->key);
    free(h);
}

void delete_hashtable(HashTable *t){
    if(!t){
        return;
    }
    for(int i=0;i<t->size;i++){
        if(!t->tab[i]){
            continue;
        }
        delete_hascell(t->tab[i]);
    }
    free(t->tab);
    free(t);
}

Key* compute_winner(CellProtected* decl, CellKey* candidates, CellKey* voters, int sizeC, int sizeV){
   // suppr_decla_invalide(&decl);
    HashTable *Hc=create_hashtable(candidates,sizeC);
    HashTable *Hv=create_hashtable(voters,sizeV);
    int posV, posC;


    CellProtected *temp=decl;
    int i=0;
    while(temp){
        posV=find_position(Hv,temp->data->pKey);
        Key *signaVote=str_to_key(temp->data->message);
        posC=find_position(Hc,signaVote);
        
        if(Hv->tab[posV]->key->n==temp->data->pKey->n && Hv->tab[posV]->key->val==temp->data->pKey->val && Hv->tab[posV]->val==0){
            if(Hc->tab[posC]->key->n==signaVote->n && Hc->tab[posC]->key->val==signaVote->val){
                Hv->tab[posV]->val++;
                Hc->tab[posC]->val++;
            }
        }

        temp=temp->next;
        free(signaVote);
        i++;
    }

    Key *clef_gagnant=(Key *)(malloc(sizeof(Key)));
    init_key(clef_gagnant,0,0);
    int max=0;
    for(int i=0;i<Hc->size;i++){
        if(!Hc->tab[i]){
            continue;
        }
        if(Hc->tab[i]->val>max){
            max=Hc->tab[i]->val;
            clef_gagnant->n=Hc->tab[i]->key->n;
            clef_gagnant->val=Hc->tab[i]->key->val;
        }
    }

    delete_hashtable(Hc);
    delete_hashtable(Hv);

    return clef_gagnant;
}