#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "blockchain.h"
#include "keys.h"
#include "rw_data.h"

void ecrire_bloc_fichier(Block *bloc, char *nom_fichier){
    FILE *fichier=fopen(nom_fichier,"w");
    if(!fichier){
        printf("Erreur d'ouverture de fichier (blockchain.c:10)\n");
        return;
    }

    char *strkey=key_to_str(bloc->author);
    fprintf(fichier,"%s\n",strkey);
    free(strkey);

    fprintf(fichier,"%s\n",bloc->hash);

    if(bloc->previous_hash){
        fprintf(fichier,"%s",bloc->previous_hash);
    }
    fprintf(fichier,"\n");

    char preuve[30];
    sprintf(preuve,"%d",bloc->nonce);
    fprintf(fichier,"%s\n",preuve);

    CellProtected *protemp=bloc->votes;
    while(protemp){
        char *strpro=protected_to_str(protemp->data);
        fprintf(fichier,"%s\n",strpro);
        free(strpro);
        protemp=protemp->next;
    }


    if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (blockchain.c:39)\n");
        exit(-1);
    }
}

Block *lire_bloc_fichier(char *nom_fichier){
    FILE *fichier=fopen(nom_fichier,"r");
    if(!fichier){
        printf("Erreur d'ouverture de fichier (blockchain.c:46)\n");
        return NULL;
    }

    Block *bloc=(Block *)(malloc(sizeof(Block)));
    if(!bloc){
        printf("Erreur d'allocation (blockchain.c:51)\n");
        if(fclose(fichier)==EOF){
            printf("Erreur de fermeture de fichier (blockchain.c:54)\n");
            exit(-1);
        }
        return NULL;
    }

    char *ligne;
    char buffer[512];

    ligne=fgets(buffer,512,fichier);
    bloc->author=str_to_key(buffer);

    ligne=fgets(buffer,512,fichier);
    bloc->hash=(unsigned char *)strdup(buffer);

    ligne=fgets(buffer,512,fichier);
    if(strcmp(buffer,"\n")!=0){
        bloc->previous_hash=(unsigned char *)strdup(buffer);
    }
    else{
        bloc->previous_hash=NULL;
    }

    ligne=fgets(buffer,512,fichier);
    sscanf(buffer,"%d",&(bloc->nonce));

    ligne=fgets(buffer,512,fichier);
    Protected *protemp=str_to_protected(buffer);
    bloc->votes=create_cell_protected(protemp);
    free(protemp->message);
    free(protemp->pKey);
    free(protemp->signature->tab);
    free(protemp->signature);
    free(protemp);

    while(ligne){
        ligne=fgets(buffer,512,fichier);
        protemp=str_to_protected(buffer);
        ajout_cell_protec_tete(&(bloc->votes),protemp);
        free(protemp->message);
        free(protemp->pKey);
        free(protemp->signature->tab);
        free(protemp->signature);
        free(protemp);
    }

    if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (blockchain.c:101)\n");
        exit(-1);
    }

    return bloc;
}

char *block_to_str(Block *bloc){
    char *chaine=(char *)(malloc(16384*sizeof(char)));
    if(!chaine){
        printf("Erreur d'allocation (blockchain.c:105)\n");
        return NULL;
    }
    char *strclef=key_to_str(bloc->author);
    strcpy(chaine,strclef);
    strcat(chaine," ");
    free(strclef);

    if(bloc->previous_hash){
        strcat(chaine,(char *)(bloc->previous_hash));
        strcat(chaine," ");
    }

    CellProtected *cellprotemp=bloc->votes;
    while(cellprotemp){
        char *strpro=protected_to_str(cellprotemp->data);
        strcat(chaine,strpro);
        strcat(chaine," ");
        free(strpro);
        cellprotemp=cellprotemp->next;
    }

    char nb[30];
    sprintf(nb,"%d",bloc->nonce);
    strcat(chaine,nb);

    int taille=strlen(chaine);

    chaine=(char *)(realloc(chaine,(taille+2)*sizeof(char)));

    return chaine;
}

unsigned char *hach_SHA256(const char *chaine){
    return SHA256((unsigned char *)chaine,strlen(chaine),0);
}

int count_lead_zeros(unsigned char *chaine){
    int nb_z=0;
    for(int i=0;i<strlen((char *)chaine);i++){
        if(chaine[i]=='0'){
            nb_z++;
        }
        else{
            break;
        }
    }
    return nb_z;
}

void compute_proof_of_work(Block *B, int d){
    B->nonce=0;
    char *blocstr=block_to_str(B);
    unsigned char *blochach=hach_SHA256(blocstr);
    int nb_z=count_lead_zeros(blochach);
    while(nb_z<d){
        free(blocstr);
        B->nonce++;
        blocstr=block_to_str(B);
        blochach=hach_SHA256(blocstr);
        //printf("%d: %s\n",B->nonce,blochach);
        nb_z=count_lead_zeros(blochach);
    }
    free(blocstr);
    if(B->hash){
        free(B->hash);
    }

    int taille=strlen((char *)blochach);
    for(int i=0;i<taille;i++){
        if(blochach[i]<32 && blochach[i]!=3){
            blochach[i]='e';
        }
    }

    B->hash=(unsigned char*)strdup((char *)blochach);
    printf("Valeur finale: %d: %s\n",B->nonce,B->hash);
}

int verify_block(Block* B, int d){
    char *hach=strdup((char *)B->hash);
    for(int i=0;i<d;i++){
        if(hach[i]!='0'){
            free(hach);
            return 0;
        }
    }
    free(hach);
    return 1;
}

void delete_block(Block* B){
    free(B->hash);
    if(B->previous_hash){
        free(B->previous_hash);
    }
    CellProtected *cpt=B->votes;
    while(cpt){
        B->votes=B->votes->next;
        free(cpt);
        cpt=B->votes;
    }
    free(B);
}

CellTree* create_node(Block* b){
    CellTree *arbre=(CellTree *)(malloc(sizeof(CellTree)));
    if(!arbre){
        printf("Erreur d'allocation (blockchain.c:205)");
        return NULL;
    }

    arbre->block=b; //On ne procède pas par copie ici pour des raisons de praticité de la désallocation de mémoire.

    arbre->father=NULL;
    arbre->firstChild=NULL;
    arbre->nextBro=NULL;
    arbre->height=0;
    return arbre;
}

int update_height(CellTree* father, CellTree* child){
    if(father->height<=(child->height+1)){
        father->height=child->height+1;
        return 1;
    }
    return 0;
}

void add_child(CellTree* father, CellTree* child){
    if(!father->firstChild){
        father->firstChild=child;
    }
    else{
        CellTree *nenfant=father->firstChild;
        while(nenfant->nextBro){
            nenfant=nenfant->nextBro;
        }
        nenfant->nextBro=child;
    }

    CellTree *popa=father;
    while(popa){
        update_height(popa,child);
        popa=popa->father;
    }
}

void print_tree(CellTree* arbre){
    CellTree *arbrebis=arbre;
    while(arbrebis){
        char *strbloc=block_to_str(arbrebis->block);
        printf("%s\n",strbloc);
        free(strbloc);
        print_tree(arbrebis->nextBro);
        arbrebis=arbrebis->firstChild;
    }
}

void delete_node(CellTree* node){
    delete_block(node->block);
    if(node->firstChild){
        node->firstChild->father=node->father;
        CellTree *arbre=node->firstChild->nextBro;
        while(arbre){
            arbre=arbre->nextBro;
        }
        if(node->nextBro){
            arbre->nextBro=node->nextBro;
        }
    }
    free(node);
}

void delete_tree(CellTree* arbre){
    if(!arbre){
        return;
    }
    CellTree *frere1=arbre, *frere2=arbre, *frere3=arbre;
    while(frere1){
        frere1=frere1->nextBro;
        frere3=frere2->firstChild;
        delete_node(frere2);
        delete_tree(frere3);
        frere2=frere1;
    }
}

CellTree* highest_child(CellTree* cell){
    if(!cell){
        return NULL;
    }
    CellTree *gandalf=cell->firstChild, *elu=cell->firstChild;
    if(!gandalf){
        return cell;
    }
    while(gandalf->nextBro){
        gandalf=gandalf->nextBro;
        if(gandalf->height>elu->height){
            elu=gandalf;
        }
    }
    return elu;
}

CellTree *last_node(CellTree *tree){
    if(!tree){
        return NULL;
    }
    if(!tree->firstChild){
        return tree;
    }
    CellTree *arbre=tree;
    while(arbre->firstChild){
        arbre=highest_child(arbre);
    }
    return arbre;
}

void fusion_liste_decla(CellProtected *dest, CellProtected *cible){
    CellProtected *arbre=dest;
    while(arbre->next){
        arbre=arbre->next;
    }
    arbre->next=cible;
}

CellProtected *liste_decla_block(CellTree *arbre){
    CellProtected *liste=arbre->block->votes;
    CellTree *arbret=highest_child(arbre);
    CellProtected *listetemp;
    while(arbret->firstChild){
        listetemp=arbret->block->votes;
        fusion_liste_decla(liste,listetemp);
        arbret=highest_child(arbret);
    }
    return liste;
}