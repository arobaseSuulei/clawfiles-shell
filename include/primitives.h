#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "sgf.h"
int mycreat(char *nom, int mode);
int myopen(char *nom, int mode);
void myclose(int fd);
int myread(int fd, char *buffer, int nb_octets);
int mywrite(int fd, char *buffer, int nb_octets);
int chercher_inode(char *nom, int inode_rep);
void mymkdir(char *nom);
void myrmdir(char *nom);
void mylink(char *nom1, char *nom2);
void myunlink(char *nom);
#endif
