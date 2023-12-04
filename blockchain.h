#ifndef BLOCKCHAIN
#define BLOCKCHAIN

#include </usr/include/openssl/sha.h>
#include "keys.h"
#include "rw_data.h"

typedef struct block {
    Key *author;
    CellProtected *votes;
    unsigned char *hash;
    unsigned char *previous_hash;
    int nonce;
} Block;

typedef struct block_tree_cell {
    Block * block ;
    struct block_tree_cell * father ;
    struct block_tree_cell * firstChild ;
    struct block_tree_cell * nextBro ;
    int height ;
} CellTree ;

void ecrire_bloc_fichier(Block *bloc, char *nom_fichier);
Block *lire_bloc_fichier(char *nom_fichier);
char *block_to_str(Block *bloc);
unsigned char *hach_SHA256(const char *chaine);
int count_lead_zeros(unsigned char *chaine );
void compute_proof_of_work(Block *B, int d);
int verify_block(Block* B, int d);
void delete_block(Block* B);
CellTree* create_node(Block* b);
int update_height(CellTree* father, CellTree* child);
void add_child(CellTree* father, CellTree* child);
void print_tree(CellTree* arbre);
void delete_node(CellTree* node);
void delete_tree(CellTree* arbre);
CellTree* highest_child(CellTree* cell);
CellTree* last_node(CellTree* tree);
void fusion_liste_decla(CellProtected *arbre1, CellProtected *arbre2);
CellProtected *liste_decla_block(CellTree *arbre);

#endif //BLOCKCHAIN
