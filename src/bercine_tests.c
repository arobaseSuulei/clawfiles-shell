



#include "../include/sgf.h"
#include "../include/bercine_shell.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------ */
/* Variables globales requises par sgf.h                               */
/* ------------------------------------------------------------------ */
int  current_inode;
extern int inode_actuel;
#define MAX_NOM 28

typedef struct {
    char nom[MAX_NOM];
    int  inode_num;
} entree_rep;

/* ------------------------------------------------------------------ */
/* Initialisation disque de test                                        */
/* ------------------------------------------------------------------ */

static void init_disque_test() {
    memset(&d, 0, sizeof(disk));
    current_inode = 0;
    d.sb.magic_number = 0xDEADBEEF;
    inode_actuel = 0;

    /* Répertoire racine : inode 0, type 2 */
    d.inodes[0].type       = 2;
    d.inodes[0].permissions = 0755;
    d.inodes[0].nlinks     = 1;
    /* Marquer inode 0 occupé dans le bitmap */
    d.sb.inode_bitmap[0] |= (1 << 0);
    d.sb.nb_inodes_utilises = 1;

    /* Allouer un bloc pour le répertoire racine */
    int b = allouer_bloc();
    d.inodes[0].blocs[0] = b;
}

/* Crée un fichier texte dans le répertoire courant */
static int creer_fichier_test(const char *nom, const char *contenu) {
    int ni = allouer_inode();
    if (ni == -1) return -1;

    d.inodes[ni].type       = 1;
    d.inodes[ni].permissions = 0644;
    d.inodes[ni].nlinks     = 1;
    d.inodes[ni].taille     = strlen(contenu);

    int b = allouer_bloc();
    if (b == -1) return -1;
    strncpy(d.blocs[b].data, contenu, BLOCK_SIZE - 1);
    d.inodes[ni].blocs[0] = b;

    /* Ajouter dans le répertoire courant */
    entree_rep *entrees = (entree_rep *)d.blocs[d.inodes[current_inode].blocs[0]].data;
    int n = BLOCK_SIZE / sizeof(entree_rep);
    for (int e = 0; e < n; e++) {
        if (entrees[e].nom[0] == '\0') {
            strncpy(entrees[e].nom, nom, MAX_NOM - 1);
            entrees[e].inode_num = ni;
            return ni;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* Tests                                                               */
/* ------------------------------------------------------------------ */

static void test_allouer_liberer_bloc() {
    printf("=== test_allouer_liberer_bloc ===\n");
    init_disque_test();

    int avant = NB_BLOCKS - d.sb.nb_blocs_utilises;
    int b = allouer_bloc();
    assert(b != -1);
    assert(d.sb.bloc_bitmap[b / 32] & (1 << (b % 32)));
    assert((NB_BLOCKS - d.sb.nb_blocs_utilises) == avant - 1);

    liberer_bloc(b);
    assert(!(d.sb.bloc_bitmap[b / 32] & (1 << (b % 32))));
    assert((NB_BLOCKS - d.sb.nb_blocs_utilises) == avant);

    printf("OK\n\n");
}

static void test_myclose() {
    printf("=== test_myclose ===\n");
    init_disque_test();

    /* inode valide et utilisé */
    assert(myclose(0) == 0);

    /* inode invalide */
    assert(myclose(-1)      == -1);
    assert(myclose(NB_INODES) == -1);

    /* inode libre (type == 0) */
    assert(myclose(5) == -1);

    printf("OK\n\n");
}

static void test_mylink() {
    printf("=== test_mylink ===\n");
    init_disque_test();

    creer_fichier_test("fichier1.txt", "contenu test");
    printf("inode racine blocs[0] = %d\n", d.inodes[current_inode].blocs[0]);
    printf("current_inode = %d\n", current_inode);
    entree_rep *e2 = (entree_rep *)d.blocs[d.inodes[current_inode].blocs[0]].data;
    printf("premiere entree : nom='%s' inode=%d\n", e2[0].nom, e2[0].inode_num);
    assert(mylink("fichier1.txt", "lien1.txt") == 0);

    /* Trouver l'inode de fichier1.txt */
    entree_rep *entrees = (entree_rep *)d.blocs[d.inodes[0].blocs[0]].data;
    int n = BLOCK_SIZE / sizeof(entree_rep);
    int ni = -1;
    for (int e = 0; e < n; e++)
        if (strncmp(entrees[e].nom, "fichier1.txt", MAX_NOM) == 0) { ni = entrees[e].inode_num; break; }
    assert(ni != -1);
    assert(d.inodes[ni].nlinks == 2);

    assert(mylink("inexistant.txt", "lien2.txt") == -1);
    printf("OK\n\n");
}

static void test_cmd_cat() {
    printf("=== test_cmd_cat ===\n");
    init_disque_test();
    creer_fichier_test("hello.txt", "Bonjour monde");
    printf("Attendu : 'Bonjour monde'\nObtenu  : ");
    cmd_cat("hello.txt");
    cmd_cat("inexistant.txt");
    printf("OK\n\n");
}

static void test_cmd_cp() {
    printf("=== test_cmd_cp ===\n");
    init_disque_test();
    creer_fichier_test("source.txt", "données à copier");
    cmd_cp("source.txt", "copie.txt");

    entree_rep *entrees = (entree_rep *)d.blocs[d.inodes[0].blocs[0]].data;
    int n = BLOCK_SIZE / sizeof(entree_rep);
    int src = 0, dst = 0;
    for (int e = 0; e < n; e++) {
        if (strncmp(entrees[e].nom, "source.txt", MAX_NOM) == 0) src = 1;
        if (strncmp(entrees[e].nom, "copie.txt",  MAX_NOM) == 0) dst = 1;
    }
    assert(src && dst);
    printf("OK\n\n");
}

static void test_cmd_rm() {
    printf("=== test_cmd_rm ===\n");
    init_disque_test();
    creer_fichier_test("a_supprimer.txt", "supprime moi");
    cmd_rm("a_supprimer.txt");

    entree_rep *entrees = (entree_rep *)d.blocs[d.inodes[0].blocs[0]].data;
    int n = BLOCK_SIZE / sizeof(entree_rep);
    int trouve = 0;
    for (int e = 0; e < n; e++)
        if (strncmp(entrees[e].nom, "a_supprimer.txt", MAX_NOM) == 0) trouve = 1;
    assert(!trouve);
    printf("OK\n\n");
}

static void test_cmd_mv() {
    printf("=== test_cmd_mv ===\n");
    init_disque_test();
    creer_fichier_test("ancien.txt", "contenu déplacé");
    cmd_mv("ancien.txt", "nouveau.txt");

    entree_rep *entrees = (entree_rep *)d.blocs[d.inodes[0].blocs[0]].data;
    int n = BLOCK_SIZE / sizeof(entree_rep);
    int ancien = 0, nouveau = 0;
    for (int e = 0; e < n; e++) {
        if (strncmp(entrees[e].nom, "ancien.txt",  MAX_NOM) == 0) ancien  = 1;
        if (strncmp(entrees[e].nom, "nouveau.txt", MAX_NOM) == 0) nouveau = 1;
    }
    assert(!ancien && nouveau);
    printf("OK\n\n");
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */

int main() {
    printf("========== TESTS BERCINE ==========\n\n");
    test_allouer_liberer_bloc();
    test_myclose();
    test_mylink();
    test_cmd_cat();
    test_cmd_cp();
    test_cmd_rm();
    test_cmd_mv();
    printf("========== TOUS LES TESTS PASSES ==========\n");
    return 0;
}