// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/*
 * ---------------------------------------------------------------------------
 *  File:        filepreview.h
 *  Description: This header file provides function declarations for the file 
 *                preview functionality within the LiteFM file manager. It 
 *                includes methods for displaying file content in an NCurses 
 *                window, checking file extensions for readability, and formatting 
 *                file sizes for display.
 *
 *  Author:      nots1dd
 *  Created:     <31/07/24>
 * 
 *  Copyright:   2024 nots1dd. All rights reserved.
 * 
 *  License:     <GNU GPL v3>
 *
 *  Notes:       The functions in this header are designed to work with NCurses 
 *                to provide a text-based preview of files, suitable for file 
 *                management applications. Ensure that file access permissions 
 *                are correctly handled to avoid errors when attempting to read 
 *                file content.
 * 
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef FILEPREVIEW_H
#define FILEPREVIEW_H

#include <ncurses.h>
#include <limits.h>  // For PATH_MAX
#include <stdio.h>   // For FILE, fopen, fgets, fclose, perror
#include <string.h>

// Constants
#define MAX_LINES 60       // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length

// Function Prototypes
void display_file(WINDOW *info_win, const char *filename);
int is_readable_extension(const char *filename);
const char *format_file_size(off_t size);

#endif // FILEPREVIEW_H
