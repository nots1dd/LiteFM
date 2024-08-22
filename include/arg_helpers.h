#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

void show_help();
void show_version();
int handle_arguments(int argc, char *argv[], char *current_path);

#endif
