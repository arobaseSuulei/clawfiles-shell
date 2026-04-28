#ifndef SGF_H
#define SGF_H

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NB_INODES 64
#define NB_BLOCKS 256
#define BLOCK_SIZE 1024

extern disk d;
extern int current_inode;

// INODE
typedef struct {
    int type;
    int taille;
    int permissions;
    int nlinks;
    time_t mtime;
    int blocs[12];
} inode;

// BLOCK
typedef struct {
    char data[BLOCK_SIZE];
} bloc;

//SUPERBLOC
typedef struct {
    int magic_number;
    int nb_inodes_utilises;
    int nb_blocs_utilises;
    int inode_bitmap[64 / 32 + 1];
    int bloc_bitmap[256 / 32 + 1];
} superbloc;

//LE DISQUE
typedef struct {
    superbloc sb;
    inode inodes[NB_INODES];
    bloc blocs[NB_BLOCKS];
} disk;



void formater(const char *diskfile);
void charger(const char *diskfile);
void sauvegarder(const char *diskfile);

int allouer_inode(void);
void liberer_inode(int num);
int allouer_bloc(void);
void liberer_bloc(int num);

int mycreat(const char *nom, int mode);
int myopen(const char *nom, int mode);
int myclose(int inode_num);
int myread(int inode_num, char *buffer, int nb);
int mywrite(int inode_num, const char *buffer, int nb);
int mymkdir(const char *nom);
int myrmdir(const char *nom);
int mylink(const char *old, const char *new);
int myunlink(const char *nom);











#endif