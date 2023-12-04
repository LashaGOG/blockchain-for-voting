#include <stdio.h>
#include <stdlib.h>

#include "protocole_rsa.h" 
#include "primalite.h" 
#include "keys.h" 
#include "generate_data.h"
#include "rw_data.h"

CellKey *create_cell_key(Key* key){
    CellKey *selleclef=(CellKey *)(malloc(sizeof(CellKey)));
    if(!selleclef){
        free(selleclef);
        printf("Erreur d'allocation (rw_data:11)\n");
        return NULL;
    }

    selleclef->data=(Key *)(malloc(sizeof(Key)));
    if(!selleclef->data){
        free(selleclef->data);
        free(selleclef);
        printf("Erreur d'allocation (rw_data:18)\n");
        return NULL;
    }

    selleclef->data->n=key->n;
    selleclef->data->val=key->val;

    selleclef->next=NULL;

    return selleclef;
}

void ajout_cell_key_tete(CellKey **liste, Key *key){
    if(*liste){
        CellKey *temp=create_cell_key(key);
        temp->next=(*liste);
        (*liste)=temp;
    }
    else{
        (*liste)=create_cell_key(key);
    }
}

CellKey *read_public_keys(char *file_name){
    FILE *fichier=fopen(file_name,"r");
    if(!fichier){
        printf("Erreur d'ouverture de fichier (rw_data:41)");
        return NULL;
    }

    CellKey *selleclef=NULL;

    char buffer[256];
    char *line;
    char cPkey[256];
    char cSkey[256];
    line=fgets(buffer,256,fichier);
    while(line){
        sscanf(buffer,"%s %s",cPkey,cSkey);
        Key *cleftemp=str_to_key(cPkey);
        ajout_cell_key_tete(&selleclef,cleftemp);
        free(cleftemp);
        line=fgets(buffer,256,fichier);
    }

    if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (rw_data:67)");
        exit(-1);
    }
    
    return selleclef;
}

void print_list_keys(CellKey* LCK){
    CellKey *temp=LCK;
    if(!temp){
        printf("La liste est vide\n");
    }
    int i=0;
    while(temp){
        char *clef=key_to_str(temp->data);
        printf("%d: %s\n",i,clef);
        free(clef);
        temp=temp->next;
        i++;
    }
}

void delete_cell_key(CellKey* c){
    free(c->data);
    free(c);
}

void delete_list_keys (CellKey* c){
    CellKey *temp=c;
    while(temp){
        c=c->next;
        delete_cell_key(temp);
        temp=c;
    }
}

CellProtected* create_cell_protected(Protected* pr){
    CellProtected *sellpro=(CellProtected *)(malloc(sizeof(CellProtected)));
    if(!sellpro){
        free(sellpro);
        printf("Erreur d'allocation (rw_data:108)\n");
        return NULL;
    }

    sellpro->data=(Protected *)(malloc(sizeof(Protected)));
    if(!sellpro->data){
        free(sellpro->data);
        free(sellpro);
        printf("Erreur d'allocation (rw_data:115)\n");
        return NULL;
    }

    sellpro->data->message=strdup(pr->message);
    if(!sellpro->data->message){
        free(sellpro->data->message);
        free(sellpro->data);
        free(sellpro);
        printf("Erreur d'allocation (rw_data:123)\n");
        return NULL;
    }

    sellpro->data->pKey=(Key *)(malloc(sizeof(Key)));
    if(!sellpro->data->pKey){
        free(sellpro->data->message);
        free(sellpro->data->pKey);
        free(sellpro->data);
        free(sellpro);
        printf("Erreur d'allocation (rw_data:132)\n");
    }

    sellpro->data->pKey->n=pr->pKey->n;
    sellpro->data->pKey->val=pr->pKey->val;

    sellpro->data->signature=init_signature(pr->signature->tab,pr->signature->taille);

    sellpro->next=NULL;

    return sellpro;
}

void ajout_cell_protec_tete (CellProtected **lst_protec, Protected* pr){
    CellProtected *temp=create_cell_protected(pr);
    temp->next=(*lst_protec);
    (*lst_protec)=temp;
}

CellProtected* read_protected(char *file_name){
    FILE *fichier=fopen(file_name,"r");
    if(!fichier){
        printf("Erreur d'ouverture de fichier (rw_data:184)\n");
        return NULL;
    }

    CellProtected *sellepro=NULL;

    char buffer[1024];
    char *line;
    char message[1024];
    char clef[1024];
    char signature[1024];
    line=fgets(buffer,1024,fichier);

    while(line){
        sscanf(buffer,"%s %s %s",clef,message,signature);
        Key *kclef=str_to_key(clef);
        Signature *ksignature=str_to_signature(signature);
        Protected *pro=init_protected(kclef,message,ksignature);
        ajout_cell_protec_tete(&sellepro,pro);
        free(kclef);
        free(ksignature->tab);
        free(ksignature);
        free(pro->message);
        free(pro->pKey);
        free(pro->signature->tab);
        free(pro->signature);
        free(pro);
        line=fgets(buffer,1024,fichier);
    }

    if(fclose(fichier)==EOF){
        printf("Erreur de fermeture de fichier (rw_data:208)");
        exit(-1);
    }

    return sellepro;
}

void print_list_protec (CellProtected* LCP){
    if(!LCP){
        printf("La liste est vide\n");
        return;
    }
    CellProtected *temp=LCP;

    int i=0;
    while(temp){
        char *cprotec=protected_to_str(temp->data);
        printf("%d: %s\n",i,cprotec);
        free(cprotec);
        temp=temp->next;
        i++;
    }
}

void delete_cell_protected(CellProtected* c){
    free(c->data->message);
    free(c->data->pKey);
    free(c->data->signature->tab);
    free(c->data->signature);
    free(c->data);
    free(c);
}

void delete_list_protected (CellProtected *c){
    CellProtected *temp=c;
    while(temp){
        c=c->next;
        delete_cell_protected(temp);
        temp=c;
    }
}