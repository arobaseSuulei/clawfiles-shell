#ifndef SGF_H
#define SGF_H

#include <time.h>

#define NB_INODES 128
#define NB_BLOCS 1024
#define TAILLE_BLOC 1024

    typedef struct {
    int nb_inodes;
    int nb_blocs;
    int nb_inodes_libres;
    int nb_blocs_libres;
    int taille_bloc;
    char bitmap_inodes[128];
    char bitmap_blocs[1024];
} superbloc;

    typedef struct {
    int type_fichier;
    char permissions[10];
    int proprietaire;
    int taille;
    int nlinks;
    time_t date_modif;
    int blocs_utilises[30];
    int nb_blocs;
}inode;

    typedef struct {
    char donnees[TAILLE_BLOC];
} bloc;

    typedef struct {
    char nom[28];
    int inode;
} entree_rep;
    
    typedef struct {
    superbloc superbloc;
    inode inodes[NB_INODES];
    bloc blocs[NB_BLOCS];
} disk;

void formater();
void sauvegarder();
void charger();

int mycreat(char *nom, char *mode);
int mymkdir(char *nom);

void lancer();
char **parser(char *ligne);
void executer(char **args);
void cmd_cd(char *nom);

// variables globales
extern disk disk;
extern int inode_courant;
extern char chemin_courant[256];
#endif
