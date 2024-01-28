// The method used here comes from https://github.com/lucasart/c-chess-cli

#include "uci.h"
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

void report_error(const int err, const char *msg) {
    if (err < 0) {
        fprintf(stderr, "-=-= error: %d - %s\n", err, msg);
    }
}

void fork_uci_client(const char *client_exe, uci_client *cli) {
    // we'll read from from_uci[0] and write to to_uci[1]
    // the uci client will read from to_uci[0] and write to from_uci[1]
    int to_uci[2] = { 0 };
    int from_uci[2] = { 0 };
    int err = pipe(to_uci);
    report_error(err, "failed to create pip to uci");
    err = pipe(from_uci);
    report_error(err, "failed to create pip from uci");

    // fork the process
    pid_t pid;
    if ((pid = fork()) < 0) {
        // there was an error forking the process
        report_error(pid, "errror forking process");
    } else if (pid == 0) {
        // this is the child process for the uci client
        // point to_uci[0] at stdin and from_uci[1] at stdout
        int cerr = dup2(to_uci[0], STDIN_FILENO);
        report_error(cerr, "error duplicating to_uci[0]");
        cerr = dup2(from_uci[1], STDOUT_FILENO);
        report_error(cerr, "error duplicating from_uci[1]");
        // maybe also dump stderr into from_uci as well?
        // cerr = dup2(from_uci[1], STDERR_FILENO);
#ifndef __linux__
        // from https://github.com/lucasart/c-chess-cli/blob/master/src/engine.c
        // Ugly (and slow) workaround for Apple's BSD-based kernels that lack the ability to
        // atomically set O_CLOEXEC when creating pipes.
        for (int fd = 3; fd < sysconf(FOPEN_MAX); close(fd++))
            ;
#endif
        const char *args[] = { client_exe, NULL };
        cerr = execvp(client_exe, args);
        report_error(cerr, "error executing process");
    } else {
        // this is our original process
        // close the file descriptors we don't need and open the ones we do
        close(from_uci[1]);
        close(to_uci[0]);
        FILE *in = fdopen(from_uci[0], "r");
        FILE *out = fdopen(to_uci[1], "w");
        cli->pid = pid;
        cli->in = in;
        cli->out = out;
    }

}
