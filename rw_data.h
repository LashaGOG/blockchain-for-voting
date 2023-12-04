#ifndef READ_WRITE_DATA
#define READ_WRITE_DATA

#include <stdlib.h>
#include <string.h>
#include "keys.h" 


typedef struct cellKey {
    Key *data;  
    struct cellKey* next; 
} CellKey;

typedef struct cellProtected {
    Protected *data;
    struct cellProtected* next;
} CellProtected; 

CellKey* create_cell_key(Key* key); 
void ajout_cell_key_tete (CellKey **lst_Key, Key* key);
CellKey* read_public_keys (char *file_Name);
void print_list_keys(CellKey* LCK);
void delete_cell_key(CellKey* c); 
void delete_list_keys (CellKey* c);

CellProtected* create_cell_protected(Protected* pr); 
void ajout_cell_protec_tete (CellProtected **lst_protec, Protected* pr);
CellProtected* read_protected(char *file_name);
void print_list_protec (CellProtected* LCP);
void delete_cell_protected(CellProtected* c); 
void delete_list_protected (CellProtected *c);

#endif //READ_WRITE_DATA