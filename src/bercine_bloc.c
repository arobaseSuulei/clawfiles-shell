


#include "../include/sgf.h"
#include <stdio.h>
#include <string.h>

/*
 * allouer_bloc : cherche un bloc libre dans le bitmap (bit à bit).
 * Le marque occupé et remet ses données à zéro.
 * Retourne le numéro du bloc ou -1 si disque plein.
 */
int allouer_bloc(void) {
    for (int i = 0; i < NB_BLOCKS; i++) {
        int word = i / 32;
        int bit  = i % 32;
        if (!(d.sb.bloc_bitmap[word] & (1 << bit))) {
            d.sb.bloc_bitmap[word] |= (1 << bit);
            d.sb.nb_blocs_utilises++;
            memset(d.blocs[i].data, 0, BLOCK_SIZE);
            return i;
        }
    }
    fprintf(stderr, "Erreur : aucun bloc libre disponible.\n");
    return -1;
}

/*
 * liberer_bloc : remet le bloc num à l'état libre dans le bitmap
 * et efface son contenu.
 */
void liberer_bloc(int num) {
    if (num < 0 || num >= NB_BLOCKS) {
        fprintf(stderr, "Erreur liberer_bloc : numéro invalide (%d).\n", num);
        return;
    }
    int word = num / 32;
    int bit  = num % 32;
    if (!(d.sb.bloc_bitmap[word] & (1 << bit))) {
        fprintf(stderr, "Attention : le bloc %d est déjà libre.\n", num);
        return;
    }
    d.sb.bloc_bitmap[word] &= ~(1 << bit);
    d.sb.nb_blocs_utilises--;
    memset(d.blocs[num].data, 0, BLOCK_SIZE);
}