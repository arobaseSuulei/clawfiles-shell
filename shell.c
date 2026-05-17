#include <string.h>
#include <stdio.h>
#include "sgf.h"

void lancer() {
    char ligne[256];
    while (1) {
        printf("[myshell %s] $ ", chemin_courant);
        if (fgets(ligne, 256, stdin) == NULL){
           sauvegarder();
           break;
    }
    
        if (strncmp(ligne, "exit", 4) == 0){
            sauvegarder();
            break;
        }
        
  char **args = parser(ligne);
  if(args[0] != NULL){
      executer(args);
    }    
  }  
}

  char **parser(char *ligne) {
    static char *args[64];
    int i = 0;
    char *token = strtok(ligne, " \t\n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return args;
}
  
void executer(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        cmd_cd(args[1]);
    } else if (strcmp(args[0], "mkdir") == 0) {
        mymkdir(args[1]);
    } else if (strcmp(args[0], "ls") == 0) {
    } else {
        printf("Commande inconnue : %s\n", args[0]);
    }
}

void cmd_cd(char *nom){
    if(nom == NULL){
       printf("Erreur : argument manquant\n");
       return;
      }

int cible = chercher_inode(nom, inode_courant);
  if (cible == -1) {
    printf("Erreur : %s n'existe pas\n", nom);
    return;
}
  if (disk.inodes[cible].type_fichier != 2) {
    printf("Erreur : %s n'est pas un repertoire\n", nom);
    return;
  }
  
  inode_courant = cible;
if (strcmp(nom, "..") == 0) {

    char *derniere_barre = strrchr(chemin_courant, '/');
    if (derniere_barre != chemin_courant) {
        *derniere_barre = '\0';
    }
} else {

    if (strcmp(chemin_courant, "/") != 0) {
        strcat(chemin_courant, "/");
    }
    strcat(chemin_courant, nom);
}
  
}
