#include "sgf.h"
static void ls(char **args) {
    int long_format = 0;

    if (args[1] != NULL && strcmp(args[1], "-l") == 0)
        long_format = 1;

    inode *dir = &disque.inodes[inode_courant];
    int i, j;

    if (long_format)
        printf("total %d\n", dir->nb_blocs * (TAILLE_BLOC / 512));

    for (i = 0; i < dir->nb_blocs; i++) {
        int bloc_idx = dir->blocs_utilises[i];
        if (bloc_idx == -1) continue;

        entree_rep *entrees = (entree_rep *) disque.blocs[bloc_idx].donnees;
        for (j = 0; j < ENTREES_PAR_BLOC; j++) {
            if (entrees[j].nom[0] == '\0') continue;

            if (long_format) {
                int    ni   = entrees[j].num_inode;
                inode *n    = &disque.inodes[ni];
                char   type = (n->type_fichier == TYPE_REPERTOIRE) ? 'd' : '-';

                char date_str[32];
                struct tm *tm_info = localtime(&n->date_modif);
                strftime(date_str, sizeof(date_str), "%b %d %H:%M", tm_info);

                printf("%c%s %2d %d %6d %s %s\n",
                       type,
                       n->permissions,
                       n->nlinks,
                       n->proprietaire,
                       n->taille,
                       date_str,
                       entrees[j].nom);
            } else {
                printf("%s  ", entrees[j].nom);
            }
        }
    }

    if (!long_format) printf("\n");
}

static void mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkdir: opérande manquante\n");
        return;
    }
    int i;
    for (i = 1; args[i] != NULL; i++) {
        if (mymkdir(args[i]) == -1)
            fprintf(stderr, "mkdir: impossible de créer '%s'\n", args[i]);
    }
}

static void rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rmdir: opérande manquante\n");
        return;
    }
    int i;
    for (i = 1; args[i] != NULL; i++) {
        if (myrmdir(args[i]) == -1)
            fprintf(stderr, "rmdir: impossible de supprimer '%s'\n", args[i]);
    }
}

static void df(char **args) {
    (void)args;
    afficher_df();
}

void boucle_principale(void) {
    char ligne[1024];

    while (1) {
        printf("[myshell %s]$ ", chemin_courant);
        fflush(stdout);

        if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
            sauvegarder();
            printf("\nAu revoir !\n");
            break;
        }

        ligne[strcspn(ligne, "\n")] = '\0';
        if (strlen(ligne) == 0) continue;

        char **args = parser(ligne);
        if (args[0] == NULL) continue;

        executer(args);
    }
}
