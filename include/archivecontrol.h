// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/*
 * ---------------------------------------------------------------------------
 *  File:        archivecontrol.h
 *  Description: This header file provides function declarations for managing 
 *                archive operations in the LiteFM file manager application. 
 *                It includes functionalities for extracting archives, 
 *                compressing directories, and handling file sizes.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 * 
 *  Copyright:   2024 nots1dd. All rights reserved.
 * 
 *  License:     <GNU GPL v3>
 *
 *  Notes:       This header is intended for use with the `libarchive` library, 
 *                which provides a unified interface for reading and writing 
 *                various archive formats. Ensure that `libarchive` is installed 
 *                and properly linked when building the LiteFM application.
 * 
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef ARCHIVE_CONTROL_H
#define ARCHIVE_CONTROL_H

#include <archive.h>
#include <archive_entry.h>
#include <string.h>
#include <ncurses.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

long get_file_size(const char *file_path);
int extract_archive(const char *archive_path);
int add_directory_to_archive(struct archive *a, const char *dir_path, const char *base_path);
int compress_directory(const char *dir_path, const char *archive_path, int format);

#endif
