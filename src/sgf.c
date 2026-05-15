#include "sgf.h"


disk d;
int current_inode = 0;

int allouer_inode(void){
    for (int i = 0; i < NB_INODES; i++) {
        int index = i / 32;
        int bit = i % 32;

        if (((d.sb.inode_bitmap[index] >> bit) & 1) == 0) {
            d.sb.inode_bitmap[index] |= (1 << bit);
            d.sb.nb_inodes_utilises++;

            d.inodes[i].type = 0;
            d.inodes[i].taille = 0;
            d.inodes[i].permissions = 0;
            d.inodes[i].nlinks = 0;
            d.inodes[i].mtime = time(NULL);
            for (int j = 0; j < 12; j++) {
                d.inodes[i].blocs[j] = -1;
            }

            return i;
        }
    }

    return -1;
}

void liberer_inode(int num){
    if (num < 0 || num >= NB_INODES) {
        return;
    }

    int index = num / 32;
    int bit = num % 32;

    if (((d.sb.inode_bitmap[index] >> bit) & 1) == 0) {
        return;
    }

    d.sb.inode_bitmap[index] &= ~(1 << bit);
    d.sb.nb_inodes_utilises--;

    d.inodes[num].type = 0;
    d.inodes[num].taille = 0;
    d.inodes[num].permissions = 0;
    d.inodes[num].nlinks = 0;
    d.inodes[num].mtime = 0;
    for (int i = 0; i < 12; i++) {
        d.inodes[num].blocs[i] = -1;
    }
}
