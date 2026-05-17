#include <string.h>
#include <stdio.h>
#include "sgf.h"

disk disk;

void formater() {
disk.superbloc.nb_inodes = NB_INODES;
disk.superbloc.nb_blocs = NB_BLOCS;
disk.superbloc.nb_inodes_libres = 127;
disk.superbloc.nb_blocs_libres = 1023;
disk.superbloc.taille_bloc = TAILLE_BLOC;

memset(disk.superbloc.bitmap_inodes, 0, NB_INODES);
memset(disk.superbloc.bitmap_blocs, 0, NB_BLOCS);

for (int i = 0; i < NB_INODES; i++) {
    disk.inodes[i].type_fichier = 0;
    for (int j = 0; j < 30; j++) {
        disk.inodes[i].blocs_utilises[j] = -1;
    }
}

for (int i = 0; i < NB_BLOCS; i++) {
    memset(disk.blocs[i].donnees, 0, TAILLE_BLOC);
}

disk.superbloc.bitmap_inodes[0] = 1;
disk.superbloc.bitmap_blocs[0] = 1;
disk.inodes[0].type_fichier = 2;
disk.inodes[0].nlinks = 2;
disk.inodes[0].taille = 0;
disk.inodes[0].blocs_utilises[0] = 0;
strcpy(disk.inodes[0].permissions, "rwxr-xr-x");

entree_rep *entrees = (entree_rep *) disk.blocs[0].donnees;

strcpy(entrees[0].nom, ".");
entrees[0].inode = 0;

strcpy(entrees[1].nom, "..");
entrees[1].inode = 0;

sauvegarder();

}


