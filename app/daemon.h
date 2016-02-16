/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
