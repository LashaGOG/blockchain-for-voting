#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "processus_vote.h"
#include "gagnant.h"

void submit_vote(Protected* p){
    FILE *fichier=fopen("Pending_votes.txt","a");
    if(!fichier){
        fichier=fopen("Pending_votes.txt","w");
        if(!fichier){
            printf("Erreur d'ouverture de fichier (processus_vote.c:12)\n");
            return;
        }
    }

    char *chaine=protected_to_str(p);
    fprintf(fichier,"%s\n",chaine);
    free(chaine);
    if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (processus_vote.c:22)\n");
        exit(-1);
    }
}

void create_block(CellTree** tree, Key* author, int d){
    CellProtected *cp=read_protected("Pending_votes.txt");

    Block *bloque=(Block *)(malloc(sizeof(Block)));
    if(!bloque){
        printf("Erreur d'allocation (processus_vote.c:31)\n");
        return;
    }
    
    bloque->author=author;
    bloque->hash=NULL;
    if(*tree){
        CellTree *ptitdernier=last_node(*tree);
        bloque->previous_hash=(unsigned char *)(strdup((char *)(ptitdernier->block->hash)));
    }
    else{
        bloque->previous_hash=NULL;
    }
    bloque->nonce=0;
    bloque->votes=cp;
    compute_proof_of_work(bloque,d);
    if(!*tree){
        *tree=create_node(bloque);
    }
    else{
        CellTree *node=create_node(bloque), *enfant=last_node(*tree);
        add_child(enfant,node);
    }
    ecrire_bloc_fichier(bloque,"Pending_block.txt");
    remove("Pending_votes.txt");
}

void add_block(int d, char* name){
    Block *bloque=lire_bloc_fichier("Pending_block.txt");
    int taille=strlen((char *)(bloque->hash));
    for(int i=0;i<taille;i++){
        if(bloque->hash[i]=='\n'){
            bloque->hash[i]='\0';
        }
    }
    if(bloque->previous_hash){
        taille=strlen((char *)(bloque->previous_hash));
        for(int i=0;i<taille;i++){
            if(bloque->previous_hash[i]=='\n'){
                bloque->previous_hash[i]='\0';
            }
        }
    }

    if(verify_block(bloque,d)!=1){
        printf("bloque non valide !\n");
        remove("Pending_block.txt");
        CellProtected *cp=bloque->votes;
        free(bloque->author);
        while(cp){
            free(cp->data->message);
            free(cp->data->pKey);
            free(cp->data->signature->tab);
            free(cp->data->signature);
            free(cp->data);
            cp=cp->next;
        }
        delete_block(bloque);
        return;
    }

    char repertoire[50]="Blockchain/";
    strcat(repertoire,name);

    ecrire_bloc_fichier(bloque,repertoire);

    remove("Pending_block.txt");
    CellProtected *cp=bloque->votes;
    free(bloque->author);
    while(cp){
        free(cp->data->message);
        free(cp->data->pKey);
        free(cp->data->signature->tab);
        free(cp->data->signature);
        free(cp->data);
        cp=cp->next;
    }
    delete_block(bloque);
    
}

CellTree* read_tree(void){

    CellTree **tarbre=(CellTree **)(malloc(10000*sizeof(CellTree *)));
    Block *bloque=NULL;
    int i=0;

    DIR *rep=opendir("Blockchain");
    if(!rep){
        printf("Erreur d'ouverture de dossier (processus_vote.c:104)\n");
        return NULL;
    }

    struct dirent* fichier_lu=readdir(rep);

    while(fichier_lu){
        if(strcmp(fichier_lu->d_name,".")==0 || strcmp(fichier_lu->d_name,"..")==0){
            fichier_lu=readdir(rep);
            continue;
        }
        char nom_fichier[30]="Blockchain/";
        strcat(nom_fichier,fichier_lu->d_name);
        bloque=lire_bloc_fichier(nom_fichier);
        tarbre[i]=create_node(bloque);
        i++;
        fichier_lu=readdir(rep);
    }
    closedir(rep);

    tarbre=(CellTree **)(realloc(tarbre,(i)*sizeof(CellTree *)));
    for(int j=0;j<i;j++){
        for(int k=0;k<i;k++){
            if(tarbre[k]->block->previous_hash && strcmp((char *)tarbre[k]->block->previous_hash,(char *)tarbre[j]->block->hash)==0 && j!=k){
                add_child(tarbre[j],tarbre[k]);
            }
        }
    }

    CellTree *arbre=tarbre[0];
    while(arbre->father){
        arbre=arbre->father;
    }
    free(tarbre);
    return arbre;
}

Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV){
    CellProtected *liste=liste_decla_block(tree);
    Key *gagnant=compute_winner(liste,candidates,voters,sizeC,sizeV);
    //delete_list_protected(liste);
    return gagnant;
}