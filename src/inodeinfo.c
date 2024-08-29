
#include "../include/inodeinfo.h"
#include "../include/cursesutils.h"
#include "../include/filepreview.h"
#include "../include/logging.h"

const char* denied_message[] = {" .__   __.   ______           _______. __    __      ",
                                " |  \\ |  |  /  __  \\         /       ||  |  |  |     ",
                                " |   \\|  | |  |  |  |       |   (----`|  |  |  |     ",
                                " |  . `  | |  |  |  |        \\   \\    |  |  |  |     ",
                                " |  |\\   | |  `--'  |    .----)   |   |  `--'  |  __ ",
                                " |__| \\__|  \\______/     |_______/     \\______/  (__)",
                                "                                                     "};

void get_file_info_popup(WINDOW* main_win, const char* path, const char* filename)
{
  struct stat file_stat;
  char        full_path[PATH_MAX];
  snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

  // Get file information
  if (stat(full_path, &file_stat) == -1)
  {
    log_message(LOG_LEVEL_ERROR, "Error retrieving file information for `%s`", full_path);
    show_message(main_win, "Error retrieving file information.");
    return;
  }

  // Create a new window for displaying file information
  int info_win_height = 13;
  int info_win_width  = (COLS / 3) + 4;
  int info_win_y      = (LINES - info_win_height) / 2;
  int info_win_x      = (COLS - info_win_width) / 2;

  WINDOW* info_win = newwin(info_win_height, info_win_width, info_win_y, info_win_x);
  draw_3d_info_win(info_win, info_win_y, info_win_x, info_win_height, info_win_width, 1, 2);
  box(info_win, 0, 0);

  // Display file information with colors
  colorLine(info_win, "File Information: ", 1, 1,
            2); /* colorLine params: win, string, color_pair, x, y */
  colorLine(info_win, "Name: ", 3, 3, 2);

  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s", filename);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  colorLine(info_win, "Size: ", 3, 4, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s", format_file_size(file_stat.st_size));
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  const char* file_ext = strrchr(filename, '.');
  colorLine(info_win, "File Type: ", 3, 5, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  if (file_ext != NULL)
  {
    wprintw(info_win, "%s", determine_file_type(full_path));
  }
  else
  {
    wprintw(info_win, "none");
  }
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  colorLine(info_win, "Last Modified: ", 3, 6, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  char mod_time[20];
  strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
  wprintw(info_win, "%s", mod_time);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  colorLine(info_win, "Permissions: ", 3, 7, 2);
  print_permissions(info_win, &file_stat);

  colorLine(info_win, "Inode: ", 3, 8, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%lu", file_stat.st_ino);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  // Additional file attributes can be displayed here
  struct passwd* pwd = getpwuid(file_stat.st_uid);
  wattron(info_win, COLOR_PAIR(ARCHIVE_COLOR_PAIR));
  mvwprintw(info_win, 9, 2, "Owner: ");
  wattroff(info_win, COLOR_PAIR(ARCHIVE_COLOR_PAIR));
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s (%d)", pwd->pw_name, file_stat.st_uid);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  colorLine(info_win, "Press any key to close this window.", 2, info_win_height - 2, 2);
  wrefresh(info_win);

  wgetch(info_win); // Wait for user input
  wclear(info_win); // Clear info window before deleting
  delwin(info_win); // Delete info window

  // Refresh the main window to ensure no artifacts remain
  wrefresh(main_win);
}

void get_file_info(WINDOW* info_win, const char* path, const char* filename)
{
  werase(info_win);
  struct stat file_stat;
  char        full_path[PATH_MAX];
  char        truncated_file_name[MAX_ITEM_NAME_LENGTH + 4]; // +4 for ellipsis and space
  char        symlink_target[PATH_MAX];
  ssize_t     len;

  snprintf(full_path, PATH_MAX, "%s/%s", path, filename);
  truncate_symlink_name(full_path);

  // Get file information using lstat to handle symlinks
  if (lstat(full_path, &file_stat) == -1)
  {
    log_message(LOG_LEVEL_ERROR, "Error retrieving file information for %s", full_path);
    show_message(info_win, "Error retrieving file/dir info.");
    box(info_win, 0, 0);
    wrefresh(info_win);
    return;
  }

  if (access(full_path, R_OK) != 0)
  {
    log_message(LOG_LEVEL_ERROR, "Access denied for %s", full_path);
    int denied_message_size = sizeof(denied_message) / sizeof(denied_message[0]);
    for (int j = 0; j < denied_message_size; j++)
    {
      mvwprintw(info_win, j + 3, 2, "%s", denied_message[j]);
    }
    show_message(info_win, "Access denied for you!");
    box(info_win, 0, 0);
    wrefresh(info_win);
    return;
  }

  // Truncate the filename if necessary
  if (strlen(filename) > MAX_ITEM_NAME_LENGTH)
  {
    snprintf(truncated_file_name, sizeof(truncated_file_name), "%.80s...", filename);
  }
  else
  {
    snprintf(truncated_file_name, sizeof(truncated_file_name), "%s", filename);
  }

  // Display file information
  wattron(info_win, A_BOLD);
  mvwprintw(info_win, 1, 2, "File/Dir Information:");
  wattroff(info_win, A_BOLD);

  clearLine(info_win, 3, 2);
  colorLine(info_win, "Name: ", 3, 3, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s", truncated_file_name);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  clearLine(info_win, 4, 2);
  colorLine(info_win, "Size: ", 3, 4, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s", format_file_size(file_stat.st_size));
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  const char* file_ext = strrchr(filename, '.');
  clearLine(info_win, 5, 2);
  colorLine(info_win, "Inode Type: ", 3, 5, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  if (!S_ISDIR(file_stat.st_mode))
  {
    wprintw(info_win, "%s", determine_file_type(full_path));
  }
  else
  {
    wprintw(info_win, "Directory");
  }
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  char mod_time[20];
  strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
  clearLine(info_win, 6, 2);
  colorLine(info_win, "Last Modified: ", 3, 6, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%s", mod_time);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  colorLine(info_win, "Inode: ", 3, 7, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  wprintw(info_win, "%lu", file_stat.st_ino);
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  clearLine(info_win, 8, 2);
  colorLine(info_win, "Group: ", 3, 8, 2);
  wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
  struct group* grp = getgrgid(file_stat.st_gid);
  if (grp != NULL)
  {
    wprintw(info_win, "%s", grp->gr_name);
  }
  else
  {
    wprintw(info_win, "Unknown");
  }
  wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));

  clearLine(info_win, 9, 2);
  colorLine(info_win, "Type: ", 3, 9, 2);
  wattron(info_win, COLOR_PAIR(IMAGE_COLOR_PAIR));
  if (S_ISREG(file_stat.st_mode))
  {
    wprintw(info_win, "Regular File");

    // Check if the file is an archive
    if (file_ext != NULL && (strcmp(file_ext, ".zip") == 0 || strcmp(file_ext, ".7z") == 0 ||
                             strcmp(file_ext, ".tar") == 0 || strcmp(file_ext, ".gz") == 0))
    {
      // Display archive contents
      display_archive_contents(info_win, full_path, file_ext);
    }
  }
  else if (S_ISDIR(file_stat.st_mode))
  {
    wattron(info_win, COLOR_PAIR(SPECIAL_COLOR_PAIR));
    wprintw(info_win, "Directory");
    wattroff(info_win, COLOR_PAIR(SPECIAL_COLOR_PAIR));

    // Horizontal Layout for Parent Directories and Subdirectories
    wattron(info_win, A_BOLD | COLOR_PAIR(DARK_BG_COLOR_PAIR));
    mvwprintw(info_win, 12, 2, " Parent Directories: ");
    mvwprintw(info_win, 12, getmaxx(info_win) / 2, " Children: ");
    wattroff(info_win, A_BOLD | COLOR_PAIR(DARK_BG_COLOR_PAIR));

    char parent_dir[PATH_MAX];
    char current_path[PATH_MAX];
    strncpy(current_path, path, sizeof(current_path));
    current_path[sizeof(current_path) - 1] = '\0'; // Ensure null termination
    char* parent_dir_ptr                   = dirname(current_path);

    // Make a copy of the parent directory path to avoid modifying the original path
    strncpy(parent_dir, parent_dir_ptr, sizeof(parent_dir));
    parent_dir[sizeof(parent_dir) - 1] = '\0'; // Ensure null termination
    int line                           = 13;
    int sub_dir_line                   = 13;
    int max_y, max_x;
    getmaxyx(info_win, max_y, max_x);

    // Print parent directories on the left
    DIR*           dir = opendir(parent_dir);
    struct dirent* entry;
    if (dir != NULL)
    {
      wattron(info_win, A_BOLD | COLOR_PAIR(DARK_BG_COLOR_PAIR));
      mvwprintw(info_win, 10, 2, " Parent Directories: ");
      wattroff(info_win, A_BOLD | COLOR_PAIR(DARK_BG_COLOR_PAIR));

      int col = 2; // Starting column
      wattron(info_win, A_BOLD | COLOR_PAIR(DIR_COLOR_PAIR));
      while ((entry = readdir(dir)) != NULL)
      {
        if (entry->d_type == DT_DIR)
        {
          // Exclude the current directory and the special entries "." and ".."
          if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
              entry->d_name[0] != '.')
          {
            // Truncate directory name if necessary
            char truncated_dir_name[MAX_ITEM_NAME_LENGTH + 4]; // +4 for ellipsis and space
            if (strlen(entry->d_name) > MAX_ITEM_NAME_LENGTH)
            {
              snprintf(truncated_dir_name, sizeof(truncated_dir_name), "%.40s...", entry->d_name);
            }
            else
            {
              snprintf(truncated_dir_name, sizeof(truncated_dir_name), "%s", entry->d_name);
            }
            // Print the directory name
            mvwprintw(info_win, line++, 2, "%s", truncated_dir_name);
          }
        }
      }
      closedir(dir);
      wattroff(info_win, A_BOLD | COLOR_PAIR(DIR_COLOR_PAIR));
    }
    else
    {
      show_message(info_win, "Error opening parent directory.");
    }

    // Print subdirectories and files on the right
    dir = opendir(full_path);
    if (dir != NULL)
    {
      while ((entry = readdir(dir)) != NULL && sub_dir_line < max_y - 1)
      {
        if (entry->d_type == DT_DIR)
        {
          if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
          {
            // Truncate directory name if necessary
            char truncated_sub_dir_name[MAX_ITEM_NAME_LENGTH + 4];
            if (strlen(entry->d_name) > MAX_ITEM_NAME_LENGTH)
            {
              snprintf(truncated_sub_dir_name, sizeof(truncated_sub_dir_name), "%.40s...",
                       entry->d_name);
            }
            else
            {
              snprintf(truncated_sub_dir_name, sizeof(truncated_sub_dir_name), "%s", entry->d_name);
            }
            wattron(info_win, A_BOLD | COLOR_PAIR(DIR_COLOR_PAIR));
            mvwprintw(info_win, sub_dir_line++, max_x / 2, "  %s", truncated_sub_dir_name);
            wattroff(info_win, A_BOLD | COLOR_PAIR(DIR_COLOR_PAIR));
          }
        }
      }
      rewinddir(dir);
      while ((entry = readdir(dir)) != NULL && sub_dir_line < max_y - 1)
      {
        if (entry->d_type != DT_DIR)
        {
          // Truncate file name if necessary
          char truncated_file_name[MAX_ITEM_NAME_LENGTH + 4];
          if (strlen(entry->d_name) + sub_dir_line > MAX_ITEM_NAME_LENGTH)
          {
            snprintf(truncated_file_name, sizeof(truncated_file_name), "%.40s...", entry->d_name);
          }
          else
          {
            snprintf(truncated_file_name, sizeof(truncated_file_name), "%s", entry->d_name);
          }
          wattron(info_win, A_BOLD | COLOR_PAIR(FILE_COLOR_PAIR));
          mvwprintw(info_win, sub_dir_line++, max_x / 2, "  %s", truncated_file_name);
          wattroff(info_win, A_BOLD | COLOR_PAIR(FILE_COLOR_PAIR));
        }
      }
      closedir(dir);
    }
    else
    {
      show_message(info_win, "Error opening directory.");
    }
  }
  else if (S_ISLNK(file_stat.st_mode))
  {
    wattron(info_win, COLOR_PAIR(SYMLINK_COLOR_PAIR));
    wprintw(info_win, "Symbolic Link");

    // Read the symlink target
    len = readlink(full_path, symlink_target, sizeof(symlink_target) - 1);
    if (len != -1)
    {
      symlink_target[len] = '\0'; // Null-terminate the string
      wattron(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
      wprintw(info_win, "  to  %s", symlink_target);
      wattroff(info_win, COLOR_PAIR(AUDIO_COLOR_PAIR));
    }
    else
    {
      show_message(info_win, "Error reading symlink target.");
    }
    wattroff(info_win, COLOR_PAIR(SYMLINK_COLOR_PAIR));
  }
  else if (S_ISFIFO(file_stat.st_mode))
  {
    wprintw(info_win, "FIFO");
  }
  else if (S_ISCHR(file_stat.st_mode))
  {
    wprintw(info_win, "Character Device");
  }
  else if (S_ISBLK(file_stat.st_mode))
  {
    wprintw(info_win, "Block Device");
  }
  else if (S_ISSOCK(file_stat.st_mode))
  {
    wprintw(info_win, "Socket");
  }
  else
  {
    wprintw(info_win, "Unknown");
  }
  wattroff(info_win, COLOR_PAIR(IMAGE_COLOR_PAIR));

  clearLine(info_win, 10, 2);
  wattron(info_win, COLOR_PAIR(ARCHIVE_COLOR_PAIR));
  mvwprintw(info_win, 10, 2, "Permissions: ");
  wattroff(info_win, COLOR_PAIR(ARCHIVE_COLOR_PAIR));
  print_permissions(info_win, &file_stat);

  box(info_win, 0, 0);
  wrefresh(info_win);
}

/*
 * TRUNCATING A SYMLINK NAME IS NECESSARY AS THE ITEM NAME MIGHT HAVE
 * THE STRING QUERY WE ARE LOOKING FOR,
 *
 * EXAMPLE: LIB64 ->LIB
 * IF I WANT TO SEARCH FOR LIB, I SHOULD GO TO THE ITEM THAT ACTUALLY
 * HAS THE STRING `LIB`, AND NOT A SYMLINK OF IT.
 *
 */
void truncate_symlink_name(char* name)
{
  char* arrow = strstr(name, " ->");
  if (arrow)
  {
    *arrow = '\0'; // Truncate the name at the " -> " point
  }
}
