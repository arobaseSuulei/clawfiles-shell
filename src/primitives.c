#include "sgf.h"

typedef struct {
    char nom[28];
    int inode_num;
} entree_repertoire;

int myopen(const char *nom, int mode){
    (void)mode;

    if (nom == NULL || current_inode < 0 || current_inode >= NB_INODES) {
        return -1;
    }

    inode *repertoire = &d.inodes[current_inode];
    if (repertoire->type != 2) {
        return -1;
    }

    int nb_entrees = BLOCK_SIZE / sizeof(entree_repertoire);

    for (int i = 0; i < 12; i++) {
        int bloc_num = repertoire->blocs[i];

        if (bloc_num < 0 || bloc_num >= NB_BLOCKS) {
            continue;
        }

        entree_repertoire *entrees = (entree_repertoire *)d.blocs[bloc_num].data;

        for (int j = 0; j < nb_entrees; j++) {
            int inode_num = entrees[j].inode_num;

            if (inode_num >= 0 && inode_num < NB_INODES &&
                d.inodes[inode_num].type == 1 &&
                strcmp(entrees[j].nom, nom) == 0) {
                return inode_num;
            }
        }
    }

    return -1;
}

int myrmdir(const char *nom){
    if (nom == NULL || current_inode < 0 || current_inode >= NB_INODES) {
        return -1;
    }

    inode *repertoire = &d.inodes[current_inode];
    if (repertoire->type != 2) {
        return -1;
    }

    int nb_entrees = BLOCK_SIZE / sizeof(entree_repertoire);

    for (int i = 0; i < 12; i++) {
        int bloc_num = repertoire->blocs[i];

        if (bloc_num < 0 || bloc_num >= NB_BLOCKS) {
            continue;
        }

        entree_repertoire *entrees = (entree_repertoire *)d.blocs[bloc_num].data;

        for (int j = 0; j < nb_entrees; j++) {
            int inode_num = entrees[j].inode_num;

            if (inode_num < 0 || inode_num >= NB_INODES ||
                strcmp(entrees[j].nom, nom) != 0) {
                continue;
            }

            inode *cible = &d.inodes[inode_num];
            if (cible->type != 2) {
                return -1;
            }

            for (int k = 0; k < 12; k++) {
                int cible_bloc = cible->blocs[k];

                if (cible_bloc < 0 || cible_bloc >= NB_BLOCKS) {
                    continue;
                }

                entree_repertoire *cible_entrees = (entree_repertoire *)d.blocs[cible_bloc].data;

                for (int l = 0; l < nb_entrees; l++) {
                    if (cible_entrees[l].inode_num >= 0 &&
                        cible_entrees[l].inode_num < NB_INODES &&
                        cible_entrees[l].nom[0] != '\0') {
                        return -1;
                    }
                }
            }

            for (int k = 0; k < 12; k++) {
                int cible_bloc = cible->blocs[k];

                if (cible_bloc >= 0 && cible_bloc < NB_BLOCKS) {
                    liberer_bloc(cible_bloc);
                    cible->blocs[k] = -1;
                }
            }

            liberer_inode(inode_num);
            entrees[j].inode_num = -1;
            entrees[j].nom[0] = '\0';
            repertoire->mtime = time(NULL);

            return 0;
        }
    }

    return -1;
}
