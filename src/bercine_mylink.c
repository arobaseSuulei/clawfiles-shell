

#include "../include/sgf.h"
#include <stdio.h>
#include <string.h>

#define MAX_NOM 28

typedef struct {
    char nom[MAX_NOM];
    int  inode_num;
} entree_rep;

static int trouver_inode_rep(const char *nom) {
    inode *rep = &d.inodes[current_inode];
    for (int b = 0; b < 12; b++) {
        int nb = rep->blocs[b];
        if (nb < 0) continue;
        entree_rep *entrees = (entree_rep *)d.blocs[nb].data;
        int n = BLOCK_SIZE / sizeof(entree_rep);
        for (int e = 0; e < n; e++)
            if (entrees[e].nom[0] != '\0' &&
                strncmp(entrees[e].nom, nom, MAX_NOM) == 0)
                return entrees[e].inode_num;
    }
    return -1;
}

/*
 * mylink : crée dans le répertoire courant une entrée new
 * pointant vers le même inode que old. Incrémente nlinks.
 * Retourne 0 ou -1.
 */
int mylink(const char *old, const char *new) {
    if (!old || !new) return -1;

    int ni = trouver_inode_rep(old);
    if (ni == -1) { fprintf(stderr, "mylink : '%s' introuvable.\n", old); return -1; }
    if (trouver_inode_rep(new) != -1) { fprintf(stderr, "mylink : '%s' existe déjà.\n", new); return -1; }

    inode *rep = &d.inodes[current_inode];
    for (int b = 0; b < 12; b++) {
        int nb = rep->blocs[b];
        if (nb < 0) continue;
        entree_rep *entrees = (entree_rep *)d.blocs[nb].data;
        int n = BLOCK_SIZE / sizeof(entree_rep);
        for (int e = 0; e < n; e++) {
            if (entrees[e].nom[0] == '\0') {
                strncpy(entrees[e].nom, new, MAX_NOM - 1);
                entrees[e].inode_num = ni;
                d.inodes[ni].nlinks++;
                return 0;
            }
        }
    }
    /* Nouveau bloc pour le répertoire */
    for (int b = 0; b < 12; b++) {
        if (rep->blocs[b] <= 0) {
            int nouveau = allouer_bloc();
            if (nouveau == -1) return -1;
            rep->blocs[b] = nouveau;
            entree_rep *entrees = (entree_rep *)d.blocs[nouveau].data;
            strncpy(entrees[0].nom, new, MAX_NOM - 1);
            entrees[0].inode_num = ni;
            d.inodes[ni].nlinks++;
            return 0;
        }
    }
    fprintf(stderr, "mylink : répertoire plein.\n");
    return -1;
}