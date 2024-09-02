#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

/* VERSION and SYSTEM INFO  */

#define VERSION               "v2.7"
#define OS_LABEL              "Operating System"
#define KERNEL_LABEL          "Kernel Version"
#define PACKAGE_MANAGER_LABEL "Package Manager"
#define FS_TYPE_LABEL         "Filesystem Type"
#define CHECK_MARK            "\u2714"
#define CROSS_MARK            "\u2718"
#define LIBRARY_LABEL         "Installed"
#define NOT_INSTALLED_LABEL   "Not Installed"
#define HORIZONTAL_LINE                                                                        \
  "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500" \
  "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"

/* COMMAND-LINE ARGUMENTS */

#define HELP_SHORT_ARG      "-h"
#define HELP_LONG_ARG       "--help"
#define VERSION_SHORT_ARG   "-v"
#define VERSION_LONG_ARG    "--version"
#define LOG_DIR_SHORT_ARG   "-l"
#define LOG_DIR_LONG_ARG    "--log-dir"
#define LOG_CLEAR_SHORT_ARG "-lc"
#define LOG_CLEAR_LONG_ARG  "--log-clear"

void show_help();
void show_version();
int  handle_arguments(int argc, char* argv[], char* current_path);

#endif
