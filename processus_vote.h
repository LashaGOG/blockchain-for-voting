#ifndef PROCESSVOTE
#define PROCESSVOTE

#include </usr/include/openssl/sha.h>
#include "keys.h"
#include "rw_data.h"
#include "blockchain.h"

void submit_vote(Protected* p);
void create_block(CellTree** tree, Key* author, int d);
void add_block(int d, char* name);
CellTree* read_tree(void);
Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);

#endif //PROCESSVOTE