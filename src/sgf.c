#include "sgf.h"

void sauvegarder(void) {
    FILE *f = fopen("disk.img", "wb");
    if (f == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir disk.img en écriture.\n");
        exit(1);
    }
    if (fwrite(&disque, sizeof(disk), 1, f) != 1) {
        fprintf(stderr, "Erreur: écriture dans disk.img incomplète.\n");
        fclose(f);
        exit(1);
    }
    fclose(f);
}

void afficher_df(void) {
    int  inodes_utilises = disque.superbloc.nb_inodes - disque.superbloc.nb_inodes_libres;
    int  blocs_utilises  = disque.superbloc.nb_blocs  - disque.superbloc.nb_blocs_libres;
    long espace_libre    = (long)disque.superbloc.nb_blocs_libres * disque.superbloc.taille_bloc;

    printf("  Informations sur le système de fichiers\n");
    printf("  Taille d'un bloc    : %d octets\n", disque.superbloc.taille_bloc);
    printf("\n");
    printf("  Inodes totaux       : %d\n", disque.superbloc.nb_inodes);
    printf("  Inodes utilisés     : %d\n", inodes_utilises);
    printf("  Inodes libres       : %d\n", disque.superbloc.nb_inodes_libres);
    printf("\n");
    printf("  Blocs totaux        : %d\n", disque.superbloc.nb_blocs);
    printf("  Blocs utilisés      : %d\n", blocs_utilises);
    printf("  Blocs libres        : %d\n", disque.superbloc.nb_blocs_libres);
    printf("\n");
    printf("  Espace libre        : %ld octets\n", espace_libre);
}
