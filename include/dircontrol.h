// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        dircontrol.h
 *
 *  Description: Header file for directory and file control operations
 *               in LiteFM.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 *
 *  Copyright:   <2024> nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       This header provides function declarations for operations
 *               such as creating, removing, renaming, and moving files
 *               and directories in the LiteFM application.
 *
 *  Revision History:
 *      <31/07/24> - <Initial Creation>
 *
 * ---------------------------------------------------------------------------
 */

#ifndef DIR_CONTROL_H
#define DIR_CONTROL_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <ncurses.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNICODE_INODE " "

char* get_current_user();
char* get_hostname();
void  get_current_working_directory(char* cwd, size_t size);
int   create_directory(const char* path, const char* dirname, char* timestamp);
void  change_directory_with_popen(const char *path);
int   remove_file(const char* path, const char* filename);
int   remove_directory(const char* path, const char* dirname);
int   remove_directory_recursive(const char* base_path, const char* dirname, int parent_fd);
int   rename_file_or_dir(const char* old_path, const char* new_name);
void  move_file_or_dir(WINDOW* win, const char* basepath, const char* current_path,
                       const char* selected_item);
int   is_directory(const char* path);
void  handle_rename(WINDOW* win, const char* path);
int   create_file(const char* path, const char* filename, char* timestamp);
void  resolve_path(const char* base_path, const char* relative_path, char* resolved_path);

#endif
