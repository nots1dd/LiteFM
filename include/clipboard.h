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
 *               copy/paste modules to `yank` file/dirname & file/dirpath
 *
 *  Author:      nots1dd
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

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 256

void yank_selected_item(char *selected_item);

#endif
