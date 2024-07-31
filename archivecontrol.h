// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#ifndef ARCHIVE_CONTROL_H
#define ARCHIVE_CONTROL_H

#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

long get_file_size(const char *file_path);
int extract_archive(const char *archive_path);
int add_directory_to_archive(struct archive *a, const char *dir_path, const char *base_path);
int compress_directory(const char *dir_path, const char *archive_path, int format);

#endif
