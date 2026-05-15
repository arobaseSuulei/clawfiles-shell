

#include "../include/sgf.h"
#include <stdio.h>

/*
 * myclose : vérifie que l'inode est valide.
 * Dans ce SGF, la fermeture ne fait pas de flush disque (géré par sauvegarder).
 * Retourne 0 en cas de succès, -1 sinon.
 */
int myclose(int inode_num) {
    if (inode_num < 0 || inode_num >= NB_INODES) {
        fprintf(stderr, "Erreur myclose : inode invalide (%d).\n", inode_num);
        return -1;
    }
    if (d.inodes[inode_num].type == 0) {
        fprintf(stderr, "Erreur myclose : inode %d non utilisé.\n", inode_num);
        return -1;
    }
    return 0;
}