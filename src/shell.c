#include "sgf.h"

int parser(char *ligne, char **args){
    int nb_tokens = 0;
    char *token;

    if (ligne == NULL || args == NULL) {
        return 0;
    }

    token = strtok(ligne, " ");
    while (token != NULL) {
        args[nb_tokens] = token;
        nb_tokens++;
        token = strtok(NULL, " ");
    }

    args[nb_tokens] = NULL;
    return nb_tokens;
}
