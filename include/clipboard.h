// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        clipboard.h
 *
 *  Description: Header file for checking the display servers of
 *               the user, and accordingly using the right
 *               copy/paste modules to `yank` file contents & file/dirpath
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 *
 *  Copyright:   <2024> nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       This header provides function declaration for
 *               a singular function that is to yank a selected item
 *
 *  Revision History:
 *      <31/07/24> - <Initial Creation>
 *
 * ---------------------------------------------------------------------------
 */

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 256

void yank_selected_item(char* selected_item);
void copyFileContents(const char* sourceFile, const char* destinationFile);

#endif
