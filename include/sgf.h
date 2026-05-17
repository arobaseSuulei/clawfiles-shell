typedef struct {
    char nom[MAX_NOM];
    int  inode_num;
} entree_rep;
typedef struct {
    superbloc sb;
    inode     inodes[NB_INODES];
    bloc      blocs[NB_BLOCS];
} disk;
