void cmd_ln(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("Usage : ln <source> <destination>\n");
        return;
    }
    mylink(args[1], args[2]);
}
void cmd_echo(char **args) {
    int pos_redirect = -1;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            pos_redirect = i;
            break;
        }
    }
    if (pos_redirect == -1 || args[pos_redirect + 1] == NULL) {
        for (int i = 1; args[i] != NULL; i++)
            printf("%s ", args[i]);
        printf("\n");
        return;
    }
    char texte[TAILLE_BLOC] = "";
    for (int i = 1; i < pos_redirect; i++) {
        strcat(texte, args[i]);
        if (i < pos_redirect - 1) strcat(texte, " ");
    }
    strcat(texte, "\n");

    char *nom_fichier = args[pos_redirect + 1];
    int fd = myopen(nom_fichier, 1);
    if (fd == -1) {
        printf("Erreur : impossible d'ouvrir '%s'.\n", nom_fichier);
        return;
    }
    mywrite(fd, texte, strlen(texte));
    myclose(fd);
}
