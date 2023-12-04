#ifndef GAGNANT
#define GAGNANT

#include <stdlib.h>
#include "keys.h"
#include "rw_data.h"

typedef struct hashcell {
    Key *key ;
    int val ;
} HashCell ;

typedef struct hashtable {
    HashCell **tab ;
    int size ;
} HashTable ;

void suppr_decla_invalide(CellProtected **cp);
HashCell *create_hashcell(Key* key);
int hash_function(Key* key, int size, int i);
int find_position(HashTable* t, Key* key);
HashTable* create_hashtable(CellKey* keys, int size);
void delete_hascell(HashCell *h);
void delete_hashtable(HashTable *t);
Key* compute_winner(CellProtected* decl, CellKey* candidates,CellKey* voters, int sizeC, int sizeV);

#endif //GAGNANT