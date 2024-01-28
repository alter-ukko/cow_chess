#ifndef UCI_H
#define UCI_H

#include <stdio.h>

typedef struct {
    int pid;
    FILE *in;
    FILE *out;
} uci_client;

void fork_uci_client(const char *client_exe, uci_client *cli);

#endif //UCI_H
