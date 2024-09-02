
#include "../include/kbinput.h"
#include "../include/archivecontrol.h"
#include "../include/cursesutils.h"
#include "../include/dircontrol.h"
#include "../include/filepreview.h"
#include "../include/inodeinfo.h"
#include "../include/logging.h"
#include "../include/musicpreview.h"
#include "../include/structs.h"

void handleInputScrollUp(int* highlight, int* scroll_position)
{
  show_term_message("", -1);
  if (*highlight > 0)
  {
    (*highlight)--;
    if (*highlight < *scroll_position)
    {
      (*scroll_position)--;
    }
  }
}

void handleInputScrollDown(int* highlight, int* scroll_position, int* item_count, int* height)
{
  show_term_message("", -1);
  if (*highlight < *item_count - 1)
  {
    (*highlight)++;
    if (*highlight >= *scroll_position + *height - 8)
    {
      (*scroll_position)++;
    }
  }
}

void handleInputToggleHidden(int* show_hidden, int* scroll_position, int* highlight)
{
  *show_hidden     = !*show_hidden; // Toggle show_hidden flag
  *highlight       = 0;
  *scroll_position = 0;
}

void handleInputMovCursBtm(int* highlight, int* item_count, int* scroll_position, int* max_y)
{
  *max_y -= 5; /* -5 for title, hostname and other lines in the window */
  if (*item_count > *max_y)
  {

    *scroll_position = *item_count - *max_y + 3;
    *highlight       = *item_count - 1;
  }
  else
  {
    *highlight = *item_count - 1;
  }
}

void handleInputMovCursTop(int* highlight, int* scroll_position)
{
  show_term_message("", -1);
  *highlight       = 0;
  *scroll_position = 0;
}

int find_item(const char* query, FileItem items[], int* item_count, int* start_index, int direction)
{
  char lower_query[NAME_MAX];
  for (int i = 0; query[i] && i < NAME_MAX; i++)
  {
    lower_query[i] = tolower(query[i]);
  }
  lower_query[strlen(query)] = '\0';

  if (direction == 1)
  { // Forward search
    for (int i = *start_index; i < *item_count; i++)
    {
      char truncated_name[NAME_MAX];
      strcpy(truncated_name, items[i].name);
      truncate_symlink_name(truncated_name);

      char lower_name[NAME_MAX];
      for (int j = 0; truncated_name[j] && j < NAME_MAX; j++)
      {
        lower_name[j] = tolower(truncated_name[j]);
      }
      lower_name[strlen(truncated_name)] = '\0';

      if (strstr(lower_name, lower_query) != NULL)
      {
        *start_index = i;
        return i;
      }
    }
    for (int i = 0; i < *start_index; i++)
    {
      char truncated_name[NAME_MAX];
      strcpy(truncated_name, items[i].name);
      truncate_symlink_name(truncated_name);

      char lower_name[NAME_MAX];
      for (int j = 0; truncated_name[j] && j < NAME_MAX; j++)
      {
        lower_name[j] = tolower(truncated_name[j]);
      }
      lower_name[strlen(truncated_name)] = '\0';

      if (strstr(lower_name, lower_query) != NULL)
      {
        *start_index = i;
        return i;
      }
    }
  }
  else if (direction == -1)
  { // Backward search
    for (int i = *start_index; i >= 0; i--)
    {
      char truncated_name[NAME_MAX];
      strcpy(truncated_name, items[i].name);
      truncate_symlink_name(truncated_name);

      char lower_name[NAME_MAX];
      for (int j = 0; truncated_name[j] && j < NAME_MAX; j++)
      {
        lower_name[j] = tolower(truncated_name[j]);
      }
      lower_name[strlen(truncated_name)] = '\0';

      if (strstr(lower_name, lower_query) != NULL)
      {
        *start_index = i;
        return i;
      }
    }
    for (int i = *item_count - 1; i > *start_index; i--)
    {
      char truncated_name[NAME_MAX];
      strcpy(truncated_name, items[i].name);
      truncate_symlink_name(truncated_name);

      char lower_name[NAME_MAX];
      for (int j = 0; truncated_name[j] && j < NAME_MAX; j++)
      {
        lower_name[j] = tolower(truncated_name[j]);
      }
      lower_name[strlen(truncated_name)] = '\0';

      if (strstr(lower_name, lower_query) != NULL)
      {
        *start_index = i;
        return i;
      }
    }
  }

  return -1; // Not found
}

void handleInputStringSearch(WINDOW* win, FileItem items[], int* item_count, int* highlight,
                             int* scroll_position, int* height, char* last_query,
                             const char* current_path)
{
  // Display search indicator
  wattron(win, A_BOLD | COLOR_PAIR(AQUA_COLOR_PAIR));
  mvwprintw(win, LINES - 3, (COLS / 2) - 75, "%s Search ON ", UNICODE_SEARCH);
  wattroff(win, A_BOLD | COLOR_PAIR(AQUA_COLOR_PAIR));
  wrefresh(win);

  // Get user input for the search query
  char query[NAME_MAX];
  get_user_input_from_bottom(stdscr, query, NAME_MAX, "search", current_path);

  // Perform the search if the query is not empty
  if (strlen(query) > 0)
  {
    int start_index = *highlight + 1;
    int found_index = find_item(query, items, item_count, &start_index, 1);

    if (found_index != -1)
    {
      *highlight = found_index;

      if (*highlight >= *scroll_position + *height - 8)
      {
        *scroll_position = *highlight - *height + 8;
      }
      else if (*highlight < *scroll_position)
      {
        *scroll_position = *highlight;
      }

      // Store the last query
      strncpy(last_query, query, NAME_MAX);
    }
    else
    {
      show_term_message("Item not found.", 1);
    }
  }
}

void handleInputStringOccurance(int direction, const char* last_query, FileItem items[],
                                int* item_count, int* highlight, int* scroll_position, int* height)
{
  if (strlen(last_query) > 0)
  {
    int start_index = *highlight + direction;
    int found_index = find_item(last_query, items, item_count, &start_index, direction);

    if (found_index != -1 && found_index != *highlight)
    {
      *highlight = found_index;

      if (*highlight >= *scroll_position + *height - 8)
      {
        *scroll_position = *highlight - *height + 8;
      }
      else if (*highlight < *scroll_position)
      {
        *scroll_position = *highlight;
      }
    }
    else
    {
      const char* message =
        (direction == 1) ? "No more NEXT occurrences found" : "No previous occurrences found";
      log_message(LOG_LEVEL_WARN, "%s for `%s` found", message, last_query);
      show_term_message(message, 1);
    }
  }
}

void handleInputGoToDir(const char* current_path, const char* path, int* highlight,
                        int* scroll_position)
{
  show_term_message("", -1);
  strcpy(current_path, path);
  *highlight       = 0;
  *scroll_position = 0;
}

void handleInputRename(int* item_count, int* highlight, int* scroll_position,
                       const char* current_path, FileItem items[])
{
  if (*item_count > 0)
  {
    const char* current_name = items[*highlight].name;
    char        full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", current_path, current_name);

    // Call the handle_rename function
    handle_rename(stdscr, full_path);

    // Update file list after renaming
    *scroll_position = 0;
    return;
  }
  else
  {
    log_message(LOG_LEVEL_WARN, "No item selected for renaming");
    show_term_message("No item selected for renaming.", 1);
  }
}

void handleInputExtractArchive(WINDOW* win, FileItem items[], const char* current_path,
                               const char* last_query, int* scroll_position, int* highlight)
{
  const char* filename = items[*highlight].name;
  if (strstr(filename, ".zip") || strstr(filename, ".tar") || strstr(filename, ".7z") ||
      strstr(filename, ".jar"))
  {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", current_path, filename);

    // Confirm extraction
    if (confirm_action(win, "Extract this archive? (y/n)"))
    {
      // Extract archive
      if (extract_archive(full_path) == 0)
      {
        *scroll_position = 0;
      }
      else
      {
        log_message(LOG_LEVEL_ERROR, "Extraction of `%s` failed", last_query);
        show_term_message("Extraction failed.", 1);
      }
    }
  }
  else
  {
    log_message(LOG_LEVEL_ERROR, "Cannot extract %s as it is is directory", items[*highlight].name);
    show_term_message("Cannot extract a directory.", 1);
  }
}

void handleInputCompressInode(WINDOW* win, FileItem items[], const char* current_path,
                              int* highlight, int* scroll_position)
{
  if (items[*highlight].is_dir)
  {

    const char* dirname = items[*highlight].name;
    char        full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", current_path, dirname);

    // Ask user for the compression format
    int choice = show_compression_options(win);

    // Define the output archive path
    char archive_path[PATH_MAX];
    snprintf(archive_path, PATH_MAX, "%s/%s.%s", current_path, dirname,
             (choice == TAR_COMPRESSION_FORMAT) ? "tar" : "zip");

    // Confirm compression
    if (choice == TAR_COMPRESSION_FORMAT || choice == ZIP_COMPRESSION_FORMAT)
    {
      int result;
      /*
       * @COMPRESSION:
       *
       * compression_directory is a function in archivecontrol.h with params:
       *
       * compress_directory(const char* dir_path, const char* archive_path, int format)
       *
       * Format type:
       *
       * 1 ==> TAR COMPRESSION
       * 2 ==> ZIP COMPRESSION
       *
       */
      show_term_message("Compressing...(do not close)", 0);
      if (choice == 1)
      {
        result = compress_directory(full_path, archive_path, 1);
      }
      else
      {
        result = compress_directory(full_path, archive_path, 2);
      }

      if (result == 0)
      {
        log_message(LOG_LEVEL_INFO, "Compression of %s (compression type: %d) successful", dirname,
                    choice);
        *scroll_position = 0;
      }
      else
      {
        log_message(LOG_LEVEL_ERROR, "Compression of %s (compression type: %d) failed", dirname,
                    choice);
        show_term_message("Compression failed.", 1);
      }
    }
  }
  else
  {
    log_message(LOG_LEVEL_WARN, "Selected item %s is not a directory", items[*highlight].name);
    show_term_message("Selected item is not a directory.", 1);
  }
}

/*                   NOTE
 * SCOPES ARE NOT WORKING UNLESS IT IS IN MAIN FUNC
 *
 * Finding a way to fix it to make the main func cleaner
 */

void handleInputScopeBack(int* history_count, int* highlight, int* scroll_position,
                          const char* current_path, DirHistory history[])
{
  show_term_message("", -1);
  if (*history_count > 0)
  {
    // Navigate using history
    (*history_count)--;
    log_message(LOG_LEVEL_DEBUG, " [PARENT] Navigating back to %s", current_path);
    strcpy(current_path, history[*history_count].path);
    *highlight = history[*history_count].highlight;
  }
  else
  {
    // Navigate to parent directory
    char parent_dir[1024];
    // Ensure parent_dir is initialized
    snprintf(parent_dir, sizeof(parent_dir), "%s", current_path);

    // Use dirname safely
    char* parent_path = dirname(parent_dir);
    log_message(LOG_LEVEL_DEBUG, " Checking out parent %s", parent_path);

    // Handle edge cases, such as navigating from the root directory
    if (strcmp(parent_path, "/") == 0 && strlen(current_path) > 1)
    {
      // Special handling for root directory
      strcpy(current_path, "/");
    }
    else
    {
      snprintf(current_path, sizeof(current_path), "%s", parent_path);
      log_message(LOG_LEVEL_DEBUG, " [PARENT] Navigating back to %s", current_path);
    }

    *highlight = 0;
  }
  // List the contents of the new directory
  *scroll_position = 0;
}

/*void handleInputScopeForward(WINDOW* win, WINDOW* info_win, int* history_count, int* highlight,*/
/*                             int* scroll_position, bool* firstKeyPress, FileItem items[],*/
/*                             DirHistory history[], const char* cur_user, const char*
 * current_path)*/
/*{*/
/*  show_term_message("", -1);*/
/*  char fullPath[MAX_PATH_LENGTH];*/
/*  snprintf(fullPath, MAX_PATH_LENGTH, "%s/%s", current_path, items[*highlight].name);*/
/**/
/*  // Check access to the directory or file*/
/**/
/*  if (access(fullPath, R_OK) != 0)*/
/*  {*/
/*    // Log the message safely*/
/*    log_message(LOG_LEVEL_ERROR, "[%s] Access denied for inode path %s: %s\n", cur_user,
 * fullPath,*/
/*                strerror(errno));*/
/**/
/*    // Show the message to the user*/
/*    show_term_message("Access denied for this inode. Check log more details..", 1);*/
/*    return;*/
/*  }*/
/**/
/*  // Check access to the realPath*/
/*  if (items[*highlight].is_dir)*/
/*  {*/
/*    if (history_count < MAX_HISTORY)*/
/*    {*/
/*      strcpy(history[*history_count].path, current_path);*/
/*      history[*history_count].highlight = *highlight;*/
/*      (*history_count)++;*/
/*    }*/
/*    strcat(current_path, "/");*/
/*    log_message(LOG_LEVEL_DEBUG, " [CHILD] Checking into %s", items[*highlight].name);*/
/*    strcat(current_path, items[*highlight].name);*/
/*    log_message(LOG_LEVEL_DEBUG, " [CHILD] Navigating into to %s", current_path);*/
/*    *highlight       = 0;*/
/*    *scroll_position = 0;*/
/*  }*/
/*  else*/
/*  {*/
/*    if ((is_readable_extension(items[*highlight].name) || !is_image(items[*highlight].name)) &&*/
/*        !items[*highlight].is_dir && !is_audio(items[*highlight].name))*/
/*    {*/
/*      *firstKeyPress = true;*/
/*      launch_env_var(win, current_path, items[*highlight].name, "EDITOR");*/
/*      /* Since we have set firstKeyPress to true, it will not wgetch(), rather it will just
 * refresh*/
/*       * everything back to how it was */
/*    }*/
/*    else if (is_image(items[*highlight].name) && !items[*highlight].is_dir)*/
/*    {*/
/*      *firstKeyPress = true;*/
/*      launch_env_var(win, current_path, items[*highlight].name, "VISUAL");*/
/*    }*/
/*    else if (is_audio(items[*highlight].name) && !items[*highlight].is_dir)*/
/*    {*/
/*      char file_path[MAX_PATH_LENGTH];*/
/*      snprintf(file_path, MAX_PATH_LENGTH, "%s/%s", current_path, items[*highlight].name);*/
/*      show_term_message(" [PREVIEW] Previewing audio file. Press q to quit.", 0);*/
/*      preview_audio(file_path);*/
/*      show_term_message("", -1);*/
/*    }*/
/*    else*/
/*    {*/
/*      show_term_message("Cannot enter this directory/file.", 1);*/
/*    }*/
/*    check_term_size(win, info_win);*/
/*    werase(win);*/
/*  }*/
/*}*/
