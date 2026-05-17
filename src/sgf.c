#include <stdio.h>
#include <stdlib.h>
#include "sgf.h"
disk  disque;
int   inode_courant = 0;
char  chemin_courant[256] = "/";
fichier_ouvert table_fd[MAX_FD];

void charger() {
    FILE *f = fopen("disk.img", "rb");
    if (f == NULL) {
        printf("Disque non trouvé, formatage en cours...\n");
        formater();
        return;
    }
    fread(&disque, sizeof(disk), 1, f);
    fclose(f);
    printf("Disque chargé.\n");
}
void sauvegarder() {
    FILE *f = fopen("disk.img", "wb");
    if (f == NULL) {
        printf("Erreur : impossible d'ouvrir disk.img en écriture.\n");
        return;
    }
    fwrite(&disque, sizeof(disk), 1, f);
    fclose(f);
    printf("Disque sauvegardé.\n");
}
void afficher_df() {
    printf("=== Informations du disque ===\n");
    printf("Inodes totaux     : %d\n", disque.sb.nb_inodes);
    printf("Inodes libres     : %d\n", disque.sb.nb_inodes_libres);
    printf("Inodes utilisés   : %d\n", disque.sb.nb_inodes - disque.sb.nb_inodes_libres);
    printf("Blocs totaux      : %d\n", disque.sb.nb_blocs);
    printf("Blocs libres      : %d\n", disque.sb.nb_blocs_libres);
    printf("Blocs utilisés    : %d\n", disque.sb.nb_blocs - disque.sb.nb_blocs_libres);
    printf("Taille d'un bloc  : %d octets\n", disque.sb.taille_bloc);
    printf("Espace libre      : %d octets\n", disque.sb.nb_blocs_libres * disque.sb.taille_bloc);
}
