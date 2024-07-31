// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

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
