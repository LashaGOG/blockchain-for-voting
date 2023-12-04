#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <time.h>
#include <dirent.h>

#include "gagnant.h"
#include "protocole_rsa.h"
#include "primalite.h"
#include "keys.h"
#include "generate_data.h"
#include "rw_data.h"
#include "blockchain.h"
#include "processus_vote.h"


void test_protocole_rsa () {
    srand(time(NULL));
    //Generation de cle :
    long p=random_prime_number(3, 7, 5000);
    long q=random_prime_number(3, 7, 5000);
    while(p==q) 
    {
        q=random_prime_number(3, 7, 5000);
    }
    long n, s, u;
    generate_keys_values(p,q,&n,&s,&u);
    //Pour avoir des cles positives :
    if (u<0)
    {
        long t=(p-1)*(q-1);
        u=u+t; //on aura toujours s*u mod t = 1
    }
    //Afichage des cles en hexadecimal
    printf("cle publique = (%lx , %lx ) \n",s,n);
    printf("cle privee = (%lx , %lx) \n",u,n);

    //Chiffrement:
    char mess[10] = "Hello";
    int len = strlen(mess);
    long* crypted = encrypt(mess, s, n); 

    printf ("Initial message : %s \n",mess);
    printf("Encoded representation : \n");
    print_long_vector(crypted, len);

    //Dechiffrement:

    char* decoded = decrypt(crypted,len, u, n);
    printf("Decoded : %s\n", decoded); 

	free(crypted);
	free(decoded);
}

void test_keys () {
    srand ( time ( NULL ) ) ;
	
    //Testing Init Keys
	Key *pKey=(Key *)(malloc(sizeof(Key)));
	Key *sKey=(Key *)(malloc(sizeof(Key)));
	init_pair_keys(pKey, sKey, 3, 7);
	printf("pKey: %lx, %lx\n", pKey->val, pKey->n);
	printf("sKey: %lx, %lx\n", sKey->val, sKey->n);
	
    //Testing Key Serialization
	char *chaine=key_to_str(pKey);
	printf("key to str:%s\n", chaine);
	Key *k=str_to_key(chaine);
	printf("str to key: %lx, %lx\n", k->val, k->n);
    free(k);

	//Testing signature

	//Candidate keys:
	Key *pKeyC=(Key *)malloc(sizeof(Key));
	Key *sKeyC=(Key *)malloc(sizeof(Key));
	init_pair_keys(pKeyC, sKeyC, 3, 7);
	
    //Declaration:
	char *mess=key_to_str(pKeyC);
	char *temp1=key_to_str(pKey);
	printf ("%s votepour %s\n" , temp1 , mess);
	free(temp1);
	Signature *sgn=sign(mess, sKey);
	printf("signature:");
	print_long_vector(sgn->tab, sgn->taille) ;
	free(chaine);
	chaine=signature_to_str(sgn);
	printf("signature to str:%s \n", chaine);
    free(sgn->tab);
	free(sgn);
	sgn=str_to_signature(chaine);
	printf("str to signature:");
	print_long_vector(sgn->tab, sgn->taille);
	
    //Testing protected:
	Protected *pr=init_protected(pKey, mess, sgn);
	
    //Verification:
	if(verify(pr)){
		printf("Signature valide\n");
	} 
	else{
		printf("Signature non valide\n");
	}
	free(chaine);
	chaine=protected_to_str(pr);
	printf("protected to str:%s\n",chaine);
    free(pr->message);
    free(pr->pKey);
    free(pr->signature->tab);
    free(pr->signature);
	free(pr);
	pr=str_to_protected(chaine);
	temp1=key_to_str(pr->pKey);
	char *temp2=signature_to_str(pr->signature);
	printf("str to protected: %s %s %s \n",temp1, pr->message, temp2);
	free(pKey);
	free(sKey);
	free(pKeyC);
	free(sKeyC);
	free(chaine);
	free(temp1);
	free(temp2);
	free(mess);
	free(sgn->tab);
	free(sgn);
    free(pr->message);
    free(pr->pKey);
    free(pr->signature->tab);
    free(pr->signature);
	free(pr);

}

void test_rw_data (void) {
    CellProtected *pro=read_protected("declarations.txt");
	print_list_protec(pro);
	delete_list_protected(pro);

}

void test_winner(void){
	CellProtected *declarations=read_protected("declarations.txt");
	char un[9]="keys.txt", deux[15]="candidates.txt";
	CellKey *votants=read_public_keys(un);
	CellKey *candidats=read_public_keys(deux);
	Key *clef_gagnant=compute_winner(declarations,candidats,votants,12,210);
	char *gagnant=key_to_str(clef_gagnant);
	fprintf(stderr,"et le gagnant est: %s\n",gagnant);
	delete_list_protected(declarations);
	delete_list_keys(votants);
	delete_list_keys(candidats);
	free(clef_gagnant);
	free(gagnant);
}

void test_blockchain(void){
	const char lol[13]="Rosetta code";
	unsigned char *terrible=hach_SHA256(lol);
	for(int i=0;i<SHA256_DIGEST_LENGTH;i++){
		printf("%02x",terrible[i]);
	}
	printf("\n");
}

int main(){
	srand(time(NULL));
	remove("Pending_votes.txt");
	generate_random_data(1000,5);

	CellProtected *declas=read_protected("declarations.txt");
	CellKey *candidats=read_public_keys("candidates.txt");
	CellKey *votants=read_public_keys("keys.txt");
	CellProtected *declat=declas;
	CellTree *arbre=NULL;
	int indice=0,cpt=0;
	while(declat){
		submit_vote(declat->data);
		cpt++;
		if(cpt==10){
			create_block(&arbre,declat->data->pKey,1);
			char nom[30];
			sprintf(nom,"bloc%d.txt",indice++);
			add_block(1,nom);
			cpt=0;
		}
		declat=declat->next;
	}

	CellTree *arbre_final=read_tree();
	//print_tree(arbre_final);
	printf("Et maintenant, calcul du gagnant !\n");
	
	Key *gagnant=compute_winner_BT(arbre_final,candidats,votants,5,1000);
	char *cgagnant=key_to_str(gagnant);
	printf("Et le gagnant est: %s\n",cgagnant);

	while(arbre_final->father){
		arbre_final=arbre_final->father;
	}

	while(arbre->father){
		arbre=arbre->father;
	}
	
	free(cgagnant);
	free(gagnant);
	
	CellKey *cand=candidats;
	while(cand){
		candidats=candidats->next;
		free(cand->data);
		free(cand);
		cand=candidats;
	}

	CellKey *vot=votants;
	while(votants){
		votants=votants->next;
		free(vot->data);
		free(vot);
		vot=votants;
	}
	declat=declas;
	while(declas){
		free(declas->data->message);
		free(declas->data->pKey);
		free(declas->data->signature->tab);
		free(declas->data->signature);
		free(declas->data);
		declas=declas->next;
		free(declat);
		declat=declas;
	}

	CellTree *arbre2=arbre;
	while(arbre2){
		while(arbre2->block->votes){
			free(arbre2->block->votes->data->message);
			free(arbre2->block->votes->data->pKey);
			free(arbre2->block->votes->data->signature->tab);
			free(arbre2->block->votes->data->signature);
			free(arbre2->block->votes->data);
			CellProtected *cpt=arbre2->block->votes;
			arbre2->block->votes=arbre2->block->votes->next;
			free(cpt);
		}

		arbre2=arbre2->firstChild;
	}
	delete_tree(arbre);

	/*CellTree *arbre_final2=arbre_final;
	while(arbre_final2){
		free(arbre_final2->block->author);
		while(arbre_final2->block->votes){
			free(arbre_final2->block->votes->data->message);
			free(arbre_final2->block->votes->data->pKey);
			free(arbre_final2->block->votes->data->signature->tab);
			free(arbre_final2->block->votes->data->signature);
			free(arbre_final2->block->votes->data);
			CellProtected *cpt=arbre_final2->block->votes;
			arbre_final2->block->votes=arbre_final2->block->votes->next;
			while(arbre_final2->block->votes==cpt){
				arbre_final2->block->votes=arbre_final2->block->votes->next;
			}
			free(cpt);
		}
		arbre_final2=arbre_final2->firstChild;
	}*/
	//delete_tree(arbre_final);

	/*CellTree *ctsan=arbre_final, *ctsan2=arbre_final;
	while(ctsan){
		CellProtected *cp1=ctsan->block->votes;
		while(cp1){
			free(cp1->data->message);
			free(cp1->data->pKey);
			free(cp1->data->signature->tab);
			free(cp1->data->signature);
			free(cp1->data);
			cp1=cp1->next;
		}
		while(ctsan==ctsan2){
			ctsan=ctsan->firstChild;
		}
		ctsan2=ctsan;
	}

	CellTree *ct=arbre_final;
	while(ct){
		free(ct->block->author);
		ct=ct->firstChild;
	}
	delete_tree(arbre_final);

	declat=declas;
	while(declas){
		declat=declat->next;
		free(declas->data->message);
		free(declas->data->pKey);
		free(declas->data->signature->tab);
		free(declas->data->signature);
		free(declas->data);
		free(declas);
		declas=declat;
	}

	CellTree *arbreb=arbre;
	while(arbreb){
		CellProtected *vote=arbreb->block->votes;
		while(vote){
			free(vote->data->message);
			free(vote->data->pKey);
			free(vote->data->signature->tab);
			free(vote->data->signature);
			free(vote->data);
			vote=vote->next;
		}
		arbreb=arbreb->firstChild;
	}
	delete_tree(arbre);*/
	return 0;
}