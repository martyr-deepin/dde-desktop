#ifndef DAEMONDESKTOP_H
#define DAEMONDESKTOP_H

#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"dde-desktop.lock"
#define LOG_FILE	"dde-desktop.log"

void log_message(char *filename,char *message);
void signal_handler(int sig);
void daemonize();

#endif // DAEMONDESKTOP_H
