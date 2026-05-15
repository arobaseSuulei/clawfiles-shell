
#include "../include/sgf.h"
#include "../include/bercine_shell.h"
#include <stdio.h>
#include <string.h>

#define MAX_NOM 28

typedef struct {
    char nom[MAX_NOM];
    int  inode_num;
} entree_rep;

/* ------------------------------------------------------------------ */
/* Fonctions internes                                                   */
/* ------------------------------------------------------------------ */

static int trouver_inode(const char *nom) {
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

static int ajouter_entree(const char *nom, int ni) {
    inode *rep = &d.inodes[current_inode];
    for (int b = 0; b < 12; b++) {
        int nb = rep->blocs[b];
        if (nb < 0) continue;
        entree_rep *entrees = (entree_rep *)d.blocs[nb].data;
        int n = BLOCK_SIZE / sizeof(entree_rep);
        for (int e = 0; e < n; e++) {
            if (entrees[e].nom[0] == '\0') {
                strncpy(entrees[e].nom, nom, MAX_NOM - 1);
                entrees[e].inode_num = ni;
                return 0;
            }
        }
    }
    for (int b = 0; b < 12; b++) {
        if (rep->blocs[b] <= 0) {
            int nouveau = allouer_bloc();
            if (nouveau == -1) return -1;
            rep->blocs[b] = nouveau;
            entree_rep *entrees = (entree_rep *)d.blocs[nouveau].data;
            strncpy(entrees[0].nom, nom, MAX_NOM - 1);
            entrees[0].inode_num = ni;
            return 0;
        }
    }
    return -1;
}

static void supprimer_entree(const char *nom) {
    inode *rep = &d.inodes[current_inode];
    for (int b = 0; b < 12; b++) {
        int nb = rep->blocs[b];
        if (nb < 0) continue;
        entree_rep *entrees = (entree_rep *)d.blocs[nb].data;
        int n = BLOCK_SIZE / sizeof(entree_rep);
        for (int e = 0; e < n; e++)
            if (strncmp(entrees[e].nom, nom, MAX_NOM) == 0) {
                memset(&entrees[e], 0, sizeof(entree_rep));
                return;
            }
    }
}

static void liberer_inode_complet(int ni) {
    inode *in = &d.inodes[ni];
    for (int b = 0; b < 12; b++) {
        if (in->blocs[b] > 0) { liberer_bloc(in->blocs[b]); in->blocs[b] = 0; }
    }
    memset(in, 0, sizeof(inode));
}

/* ------------------------------------------------------------------ */
/* Commandes                                                            */
/* ------------------------------------------------------------------ */

void cmd_cat(const char *nom) {
    int ni = trouver_inode(nom);
    if (ni == -1) { fprintf(stderr, "cat : '%s' introuvable.\n", nom); return; }
    inode *in = &d.inodes[ni];
    int restant = in->taille;
    for (int b = 0; b < 12 && restant > 0; b++) {
        int nb = in->blocs[b];
        if (nb < 0) continue;
        int a_lire = restant > BLOCK_SIZE ? BLOCK_SIZE : restant;
        fwrite(d.blocs[nb].data, 1, a_lire, stdout);
        restant -= a_lire;
    }
    printf("\n");
}

void cmd_cp(const char *nom1, const char *nom2) {
    int ni_src = trouver_inode(nom1);
    if (ni_src == -1) { fprintf(stderr, "cp : '%s' introuvable.\n", nom1); return; }
    if (trouver_inode(nom2) != -1) { fprintf(stderr, "cp : '%s' existe déjà.\n", nom2); return; }

    int ni_dst = allouer_inode();
    if (ni_dst == -1) { fprintf(stderr, "cp : plus d'inode disponible.\n"); return; }

    inode *src = &d.inodes[ni_src];
    inode *dst = &d.inodes[ni_dst];

    dst->type        = src->type;
    dst->permissions = src->permissions;
    dst->taille      = src->taille;
    dst->nlinks      = 1;
    dst->mtime       = src->mtime;
    memset(dst->blocs, 0, sizeof(dst->blocs));

    for (int b = 0; b < 12; b++) {
        int nb_src = src->blocs[b];
        if (nb_src <= 0) continue;
        int nb_dst = allouer_bloc();
        if (nb_dst == -1) { fprintf(stderr, "cp : disque plein.\n"); liberer_inode_complet(ni_dst); return; }
        memcpy(d.blocs[nb_dst].data, d.blocs[nb_src].data, BLOCK_SIZE);
        dst->blocs[b] = nb_dst;
    }
    if (ajouter_entree(nom2, ni_dst) == -1) liberer_inode_complet(ni_dst);
}

void cmd_rm(const char *nom) {
    int ni = trouver_inode(nom);
    if (ni == -1) { fprintf(stderr, "rm : '%s' introuvable.\n", nom); return; }
    if (d.inodes[ni].type == 2) { fprintf(stderr, "rm : '%s' est un répertoire.\n", nom); return; }
    supprimer_entree(nom);
    d.inodes[ni].nlinks--;
    if (d.inodes[ni].nlinks <= 0) liberer_inode_complet(ni);
}

void cmd_mv(const char *nom1, const char *nom2) {
    if (trouver_inode(nom1) == -1) { fprintf(stderr, "mv : '%s' introuvable.\n", nom1); return; }
    cmd_cp(nom1, nom2);
    cmd_rm(nom1);
}