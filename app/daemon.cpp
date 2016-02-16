/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "daemon.h"

void log_message(const char *filename,const char *message)
{
    FILE *logfile;
    logfile=fopen(filename,"a");
    if(!logfile) return;
    fprintf(logfile,"%s\n",message);
    fclose(logfile);
}

void signal_handler(int sig)
{
    switch(sig) {
    case SIGHUP:
        log_message(LOG_FILE, "hangup signal catched");
        break;
    case SIGTERM:
        log_message(LOG_FILE, "terminate signal catched");
        exit(0);
        break;
    }
}

void daemonize()
{
    int i,lfp;
    char str[10];
    if(getppid()==1) {
//        printf("already a daemon");
        return; /* already a daemon */
    }
    i=fork();
    if (i<0) {
//        printf("fork a daemon error");
        exit(1); /* fork error */
    };
    if (i>0) {
//        printf("parent exits\n");
        exit(0); /* parent exits */
    }
    /* child (daemon) continues */
//    printf("child (daemon) continues\n");
    setsid(); /* obtain a new process group */
    for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
    i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
    umask(027); /* set newly created file permissions */
    chdir(RUNNING_DIR); /* change running directory */
    lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
    if (lfp<0) {
        exit(1); /* can not open */
    }
    if (lockf(lfp,F_TLOCK,0)<0) {
        log_message(LOG_FILE, "there is an instance already running!\n");
        exit(0); /* can not lock */
    }
    /* first instance continues */
    sprintf(str,"%d\n",getpid());
    write(lfp,str,strlen(str)); /* record pid to lockfile */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signal_handler); /* catch hangup signal */
    signal(SIGTERM,signal_handler); /* catch kill signal */
}
