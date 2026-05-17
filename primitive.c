#include <string.h>
#include <stdio.h>
#include <time.h>
#include "sgf.h"

int mycreat(char *nom, char *mode) {
  if(chercher_inode(nom, inode_courant) != -1){
      printf("Erreur : le fichier %s existe déjà\n", nom);
      return -1;
      }
      
  int nouvel_inode = allouer_inode();
    if (nouvel_inode == -1) {
    printf("Erreur : plus d'inodes disponibles\n");
    return -1;
}
   disk.inodes[nouvel_inode].type_fichier = 1;
   disk.inodes[nouvel_inode].taille = 0;
   disk.inodes[nouvel_inode].nlinks = 1;
   disk.inodes[nouvel_inode].date_modif = time(NULL);
   strcpy(disk.inodes[nouvel_inode].permissions, mode);
   
   inode *rep = &disk.inodes[inode_courant];
      for(int i=0; i<30; i++){
          if(rep->blocs_utilises[i] == -1) break;
          entree_rep *entrees = (entree_rep *) disk.blocs[rep->blocs_utilises[i]].donnees;
          
  for (int j = 0; j < 32; j++) {
    if (entrees[j].nom[0] == '\0') {
        strcpy(entrees[j].nom, nom);
        entrees[j].inode = nouvel_inode;
        return nouvel_inode;
      }
  }
}   
  
  printf(" Erreur : Le repertoire est plein\n");
  return -1;

}


int mymkdir(char *nom){
  if(chercher_inode(nom, inode_courant) != -1){
      printf("Erreur : le fichier %s existe déjà\n", nom);
      return -1;
}

int nouvel_inode = allouer_inode();
    if (nouvel_inode == -1) {
    printf("Erreur : plus d'inodes disponibles\n");
    return -1;
}    

int nouveau_bloc = allouer_bloc();
if(nouveau_bloc == -1){
  liberer_inode(nouvel_inode);
  printf("Erreur : plus de blocs disponibles\n");
  return -1;
}

disk.inodes[nouvel_inode].type_fichier = 2;
disk.inodes[nouvel_inode].taille = 0;
disk.inodes[nouvel_inode].nlinks = 2;
disk.inodes[nouvel_inode].date_modif = time(NULL);
disk.inodes[nouvel_inode].blocs_utilises[0] = nouveau_bloc;
strcpy(disk.inodes[nouvel_inode].permissions, "rwxr-xr-x");
   

entree_rep *entrees = (entree_rep *) disk.blocs[nouveau_bloc].donnees;
strcpy(entrees[0].nom, ".");
entrees[0].inode = nouvel_inode;
strcpy(entrees[1].nom, "..");
entrees[1].inode = inode_courant;

inode *rep = &disk.inodes[inode_courant];
      for(int i=0; i<30; i++){
          if(rep->blocs_utilises[i] == -1) break;
          entree_rep *entrees_rep = (entree_rep *) disk.blocs[rep->blocs_utilises[i]].donnees;
          
  for (int j = 0; j < 32; j++) {
    if (entrees_rep[j].nom[0] == '\0') {
        strcpy(entrees_rep[j].nom, nom);
        entrees_rep[j].inode = nouvel_inode;
        disk.inodes[inode_courant].nlinks++;
        return nouvel_inode;
      }
  }
}   
  
  printf(" Erreur : Le repertoire est plein\n");
  return -1;

}





