// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        inodeinfo.h
 *  Description: Displaying of each inode's info onto the info_win,
 *               all its functions, both popup and main window logic
 *
 *  Author:      Siddharth Karanam
 *  Created:     <29/08/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       inodeinfo also has the function `truncate_symlink_name`.
 *               It made sense as the function is only required in two areas;
 *               in string search, and for proper file info retreival
 *
 *  Revision History:
 *      <29/08/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */ 

#ifndef INODE_INFO_H
#define INODE_INFO_H

#include <dirent.h>
#include <grp.h>
#include <ncurses.h>
#include <stdio.h>

#define MAX_ITEM_NAME_LENGTH 80

void get_file_info_popup(WINDOW* main_win, const char* path, const char* filename);
void get_file_info(WINDOW* info_win, const char* path, const char* filename);
void truncate_symlink_name(char* name);

#endif
