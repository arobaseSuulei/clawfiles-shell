#include "sgf.h"
int chercher_inode(const char *nom, int inode_dir) {
    if (inode_dir < 0 || inode_dir >= NB_INODES) return -1;

    inode *dir = &disque.inodes[inode_dir];
    if (dir->type_fichier != TYPE_REPERTOIRE) return -1;

    int i, j;
    for (i = 0; i < dir->nb_blocs; i++) {
        int bloc_idx = dir->blocs_utilises[i];
        if (bloc_idx == -1) continue;

        entree_rep *entrees = (entree_rep *) disque.blocs[bloc_idx].donnees;
        for (j = 0; j < ENTREES_PAR_BLOC; j++) {
            if (entrees[j].nom[0] != '\0' &&
                strncmp(entrees[j].nom, nom, MAX_NOM) == 0) {
                return entrees[j].num_inode;
            }
        }
    }
    return -1;
}


int mywrite(int fd, const char *buffer, int nb_octets) {
    if (fd < 0 || fd >= MAX_FICHIERS_OUVERTS ||
        table_fichiers[fd].num_inode == -1) {
        fprintf(stderr, "Erreur: descripteur %d invalide.\n", fd);
        return -1;
    }
    if (table_fichiers[fd].mode == MODE_LECTURE) {
        fprintf(stderr, "Erreur: fichier ouvert en lecture seule.\n");
        return -1;
    }

    int    num_inode = table_fichiers[fd].num_inode;
    int    position  = table_fichiers[fd].position;
    inode *n         = &disque.inodes[num_inode];

    int ecrits = 0;
    while (ecrits < nb_octets) {
        int bloc_logique = (position + ecrits) / TAILLE_BLOC;
        int offset       = (position + ecrits) % TAILLE_BLOC;
        if (bloc_logique >= n->nb_blocs) {
            if (n->nb_blocs >= MAX_BLOCS_PAR_INODE) {
                fprintf(stderr, "Erreur: limite de blocs par fichier atteinte.\n");
                break;
            }
            int nouveau_bloc = allouer_bloc();
            if (nouveau_bloc == -1) break;
            n->blocs_utilises[n->nb_blocs] = nouveau_bloc;
            n->nb_blocs++;
        }

        int bloc_physique        = n->blocs_utilises[bloc_logique];
        int disponible_dans_bloc = TAILLE_BLOC - offset;
        int a_copier             = nb_octets - ecrits;
        if (a_copier > disponible_dans_bloc)
            a_copier = disponible_dans_bloc;

        memcpy(disque.blocs[bloc_physique].donnees + offset,
               buffer + ecrits,
               a_copier);
        ecrits += a_copier;
    }

    table_fichiers[fd].position += ecrits;

    if (table_fichiers[fd].position > n->taille)
        n->taille = table_fichiers[fd].position;

    n->date_modif = time(NULL);
    return ecrits;
}
