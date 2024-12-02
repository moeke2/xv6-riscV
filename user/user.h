#ifndef XV6_USER_USER_H
#define XV6_USER_USER_H

#include "kernel/types.h"

struct stat;

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
void* sbrk(int);
int sleep(int);
int uptime(void);
void vmprintmappings(void);
void halt(void) __attribute__((noreturn));
int puts(const char*);
int getnumsyscalls(void);
void traceme(int);
int mmap(uint64 addr, int perms);
int chmod(const char*, int);
int symlink(const char*, const char*);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
char* fgets(char *buf, int max, int fd);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
int fastuptime(void);

// umalloc.c
void* malloc(uint);
void free(void*);

#endif
