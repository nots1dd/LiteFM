// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

#include "../include/filepreview.h"
#include "../include/cursesutils.h"
#include "../include/highlight.h"
#include "../include/logging.h"
#include "../include/signalhandling.h"

#define MAX_LINES 60            // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256     // Define the maximum line length
#define MAX_FILE_HEADER_SIZE 18 // Maximum size needed for the longest signature
#define BUFFER_SIZE 1024
#define MAX_FILE_TYPE_LENGTH 256

int singlecommentslen = 0;

const char* determine_file_type(const char* filename);

const char* get_file_extension(const char* filename)
{
  const char* dot = strrchr(filename, '.');

  // If there's no dot in the filename, check file type
  if (dot == NULL)
  {
    const char* file_type = determine_file_type(filename);
    if (file_type && strcmp(file_type, "text/x-shellscript") == 0)
    {
      return "sh"; // Return the extension for shell scripts
    }
    return ""; // No extension found and not a shell script
  }

  // Return the extension without the dot
  return dot + 1;
}

const char* determine_file_type(const char* filename)
{
  static char file_type[MAX_FILE_TYPE_LENGTH];
  char        command[MAX_FILE_TYPE_LENGTH + 50];

  // Construct the command to run the 'file' command with the provided filename
  snprintf(command, sizeof(command), "file --brief --mime-type \"%s\"", filename);

  FILE* fp = popen(command, "r");
  if (!fp)
  {
    return "Error";
  }

  // Read the output from the 'file' command
  if (fgets(file_type, sizeof(file_type), fp) == NULL)
  {
    pclose(fp);
    return "Unknown";
  }

  // Remove trailing newline character
  file_type[strcspn(file_type, "\n")] = '\0';

  pclose(fp);
  return file_type;
}

const char* read_lines(const char* filename, size_t max_lines)
{
  FILE* file = fopen(filename, "r"); // Open file in text mode
  if (file == NULL)
  {
    perror("Error opening file");
    return NULL;
  }

  // Allocate buffer for the entire content
  char* content = (char*)malloc(BUFFER_SIZE);
  if (content == NULL)
  {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t content_size = BUFFER_SIZE;
  size_t total_length = 0;
  size_t line_count   = 0;

  while (line_count < max_lines)
  {
    if (fgets(content + total_length, content_size - total_length, file) == NULL)
    {
      if (feof(file))
      {
        break; // End of file reached
      }
      perror("Error reading file");
      free(content);
      fclose(file);
      return NULL;
    }

    total_length = strlen(content);
    if (total_length > 0 && content[total_length - 1] == '\n')
    {
      line_count++;
    }

    // Reallocate if needed
    if (total_length + BUFFER_SIZE > content_size)
    {
      content_size += BUFFER_SIZE;
      content = (char*)realloc(content, content_size);
      if (content == NULL)
      {
        perror("Memory reallocation failed");
        fclose(file);
        return NULL;
      }
    }
  }

  fclose(file);

  // Ensure the string is null-terminated
  content[total_length] = '\0';

  return content;
}

const char* empty_message[] = {" _____ __  __ ____ _______   __  _____ ___ _     _____   _ ",
                               "| ____|  \\/  |  _ \\_   _\\ \\ / / |  ___|_ _| |   | ____| | |",
                               "|  _| | |\\/| | |_) || |  \\ V /  | |_   | || |   |  _|   | |",
                               "| |___| |  | |  __/ | |   | |   |  _|  | || |___| |___  |_/",
                               "|_____|_|  |_|_|    |_|   |_|   |_|   |___|_____|_____| (_)",
                               " "};

const char* get_keywords_file(const char* ext)
{
  static char keywords_file[256];
  char*       project_dir;
  char        resolved_path[PATH_MAX];

  // Get the full path of the current file
  if (realpath(__FILE__, resolved_path) != NULL)
  {
    // Get the directory where this source file is located
    project_dir = dirname(resolved_path);

    // Map multiple extensions to the same YAML file
    if (strcmp(ext, "h") == 0 || strcmp(ext, "hpp") == 0 || strcmp(ext, "cpp") == 0 ||
        strcmp(ext, "c") == 0 || strcmp(ext, "cxx") == 0)
    {
      snprintf(keywords_file, sizeof(keywords_file), "%s/../keywords/c-keywords.yaml", project_dir);
    }
    // Add more mappings as needed
    else
    {
      snprintf(keywords_file, sizeof(keywords_file), "%s/../keywords/%s-keywords.yaml", project_dir,
               ext);
    }
    log_message(LOG_LEVEL_DEBUG, " [SYNHASH] Loading in %s", keywords_file);
  }
  else
  {
    log_message(LOG_LEVEL_ERROR, " [SYNHASH] Could not resolve the project directory.");
    return NULL;
  }

  return keywords_file;
}

void display_file(WINDOW* info_win, const char* filename)
{

  HashTable* keywords       = create_table();
  HashTable* singlecomments = create_table();
  HashTable* multicomments1 = create_table();
  HashTable* multicomments2 = create_table();
  HashTable* strings        = create_table();
  HashTable* functions      = create_table();
  HashTable* symbols        = create_table();
  HashTable* operators      = create_table();

  FILE* file = fopen(filename, "r");
  if (!file)
  {
    werase(info_win); // Clear the window first
    mvwprintw(info_win, 1, 2, "Error opening file");
    box(info_win, 0, 0);
    wrefresh(info_win); // Refresh the window to show the error message
    return;
  }
  char        keywords_file[256];
  const char* ext                = get_file_extension(filename);
  const char* keywords_file_path = get_keywords_file(ext);
  bool        syntaxLoad =
    load_syntax(keywords_file_path, keywords, singlecomments, multicomments1, multicomments2,
                strings, functions, symbols, operators, &singlecommentslen);
  werase(info_win);                             // Clear the window before displaying content
  mvwprintw(info_win, 0, 2, " File Preview: "); // Add a title to the window

  char line[MAX_LINE_LENGTH];
  int  row        = 3; // Start at row 3 to account for the title and spacing
  int  lines_read = 0;
  char sanitizedCurPath[PATH_MAX];
  if (strncmp(filename, "//", 2) == 0)
  {
    snprintf(sanitizedCurPath, sizeof(sanitizedCurPath), "%s", filename + 1);
  }
  else
  {
    strcpy(sanitizedCurPath, filename);
  }
  wattron(info_win, A_BOLD | COLOR_PAIR(9));
  print_limited(info_win, 1, 1, sanitizedCurPath);
  wattroff(info_win, A_BOLD | COLOR_PAIR(9));

  // Initialize color pairs for syntax highlighting
  start_color();
  use_default_colors();
  if (can_change_color())
  {
    // Normalize RGB values to the range 0-1000
    init_color(COLOR_CYAN, 70 * 1000 / 255, 70 * 1000 / 255, 70 * 1000 / 255);
  }

  init_pair(21, COLOR_CYAN, -1);    // comments
  init_pair(22, COLOR_GREEN, -1);   // strings
  init_pair(23, COLOR_YELLOW, -1);  // numbers
  init_pair(24, COLOR_BLUE, -1);    // keywords
  init_pair(25, COLOR_MAGENTA, -1); // symbols
  init_pair(26, 167, 235);          // functions
  init_pair(27, COLOR_RED, -1);
  init_pair(28, 108, 235);

  // Read file and display lines with syntax highlighting
  if (syntaxLoad)
  {
    const char* text = read_lines(filename, MAX_LINES);
    highlight_code(info_win, 3, 1, text, keywords, singlecomments, multicomments1, multicomments2,
                   strings, functions, symbols, operators, &singlecommentslen);
    wrefresh(info_win);
  }
  else
  {
    while (fgets(line, sizeof(line), file) && row < MAX_LINES - 1)
    {
      // Remove newline character from the line if present
      line[strcspn(line, "\n")] = '\0';

      // Highlight syntax in the line and display
      mvwprintw(info_win, row, 1, "%s", line);
      row++;
      lines_read++;
    }
  }

  if (lines_read == 0 && !syntaxLoad)
  {
    int empty_message_size = sizeof(empty_message) / sizeof(empty_message[0]);
    for (int j = 0; j < empty_message_size; j++)
    {
      mvwprintw(info_win, j + 3, 2, "%s", empty_message[j]);
    }
    mvwprintw(info_win, empty_message_size + 4, 2, "Printed by LiteFM");
  }
  fclose(file);

  // Draw border and refresh window
  draw_colored_border(info_win, 4);
  wrefresh(info_win); // Refresh the window to show the content
}

int is_readable_extension(const char* filename)
{
  // List of supported extensions (excluding the dot)
  const char* supported_extensions[] = {
    "txt", "sh", "json",   "conf", "log", "md",   "c",    "h",   "hpp",  "cpp",
    "cxx", "cc", "js",     "ts",   "jsx", "java", "dart", "asm", "py",   "rs",
    "go",  "pl", "backup", "rasi", "nix", "yaml", "html", "css", "scss", NULL};

  // Check if the file has a supported extension
  const char* ext = get_file_extension(filename);
  if (ext[0] != '\0')
  {
    for (int i = 0; supported_extensions[i] != NULL; i++)
    {
      if (strcmp(ext, supported_extensions[i]) == 0)
      { // Compare directly with the extension list
        return 1;
      }
    }
  }

  // Check file type based on MIME type
  const char* file_type = determine_file_type(filename);
  if (file_type &&
      (strcmp(file_type, "text/plain") == 0 || strcmp(file_type, "text/x-shellscript") == 0 ||
       strcmp(file_type, "application/json") == 0 ||
       strcmp(file_type, "text/x-c") == 0 ||           // C source code
       strcmp(file_type, "text/x-c++") == 0 ||         // C++ source code
       strcmp(file_type, "text/x-python") == 0 ||      // Python script
       strcmp(file_type, "text/x-java-source") == 0 || // Java source code
       strcmp(file_type, "text/html") == 0 || strcmp(file_type, "text/css") == 0))
  {
    return 1;
  }

  return 0;
}

const char* format_file_size(off_t size)
{
  static char formatted_size[64];

  // Check if the size is at least 1 byte
  if (size >= 1ULL << 30)
  { // GiB
    snprintf(formatted_size, sizeof(formatted_size), "%.2f GiB", (double)size / (1ULL << 30));
  }
  else if (size >= 1ULL << 20)
  { // MiB
    snprintf(formatted_size, sizeof(formatted_size), "%.2f MiB", (double)size / (1ULL << 20));
  }
  else if (size >= 1ULL << 10)
  { // KiB
    snprintf(formatted_size, sizeof(formatted_size), "%.2f KiB", (double)size / (1ULL << 10));
  }
  else
  { // Bytes
    snprintf(formatted_size, sizeof(formatted_size), "%ld bytes", size);
  }
  return formatted_size;
}

int is_image(const char* filename)
{
  const char* image_extensions[] = {".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".webp"};
  size_t      num_extensions     = sizeof(image_extensions) / sizeof(image_extensions[0]);

  const char* ext = strrchr(filename, '.');
  if (!ext)
    return 0;

  char   ext_lower[16];
  size_t i;
  for (i = 0; ext[i] && i < sizeof(ext_lower) - 1; i++)
  {
    ext_lower[i] = tolower((unsigned char)ext[i]);
  }
  ext_lower[i] = '\0';

  for (i = 0; i < num_extensions; i++)
  {
    if (strcmp(ext_lower, image_extensions[i]) == 0)
    {
      return 1;
    }
  }

  return 0;
}

int is_audio(const char* filename)
{
  const char* image_extensions[] = {".mp3", ".wav", ".flac", ".opus", ".m4a"};
  size_t      num_extensions     = sizeof(image_extensions) / sizeof(image_extensions[0]);

  const char* ext = strrchr(filename, '.');
  if (!ext)
    return 0;

  char   ext_lower[16];
  size_t i;
  for (i = 0; ext[i] && i < sizeof(ext_lower) - 1; i++)
  {
    ext_lower[i] = tolower((unsigned char)ext[i]);
  }
  ext_lower[i] = '\0';

  for (i = 0; i < num_extensions; i++)
  {
    if (strcmp(ext_lower, image_extensions[i]) == 0)
    {
      return 1;
    }
  }

  return 0;
}

bool is_valid_editor(const char* editor)
{
  for (size_t i = 0; i < strlen(editor); ++i)
  {
    if (!isalnum(editor[i]) && editor[i] != '-' && editor[i] != '_')
    {
      return false;
    }
  }
  return true;
}

void launch_env_var(WINDOW* win, const char* current_path, const char* filename,
                    const char* env_var)
{
  const char* editor   = getenv(env_var);
  int         isVISUAL = 0;
  if (strcmp(env_var, "VISUAL") == 0)
    isVISUAL = 1; // just getting info on what env_var is being launched based
                  // on that will decide the delay
  /*
   * THIS IS BECAUSE WHEN WINDOW RESIZES, LITEFM HAS A FUNCTION TO RESIZE THE
   * WINDOWING SYSTEM
   *
   * BUT SINCE A WINDOW ON HYPRLAND FOR INSTANCE KILLS IN LESS THAN A SECOND,
   * THE WINDOW DIMENSIONS ARE NOT REGISTERED TO CHANGE AND HENCE WOULD RESULT
   * IN A WEIRD HALF TERMINAL USED CONDITION
   *
   * IF YOU USE A TERMINAL BASED EDITOR (NVIM, VIM, VI, NANO, etc) THIS WILL NOT
   * BE A PROBLEM AS IN WILL NOT LEAD TO RESIZING ISSUES
   *
   * THIS FUNCTIONALITY IS OF COURSE NOT PERFECT BUT IS SIMPLE AND QUITE ELEGANT
   * IN A LOT OF WAYS I FIND MANY NCURSES APPLICATIONS STRUGGLING TO COPE WITH
   * RESIZING (THIS DOES TOO TBF), BUT ATLEAST IT DOES TRY TO STAY AND
   * DYNAMICALLY RESIZE.
   *
   * ANY SUGGESTIONS ON HOW I CAN APPROACH THIS ISSUE/SOLN IS MUCH APPRECIATED.
   *
   */
  if (editor == NULL)
  {
    if (strcmp(env_var, "EDITOR") == 0)
    {
      editor   = getenv("VISUAL");
      isVISUAL = 1;
    }
    else if (strcmp(env_var, "VISUAL") == 0)
    {
      editor = getenv("EDITOR");
    }
    if (editor == NULL)
    {
      editor = "nano"; // Default to GNU nano if neither EDITOR nor VISUAL is set
    }
  }

  log_message(LOG_LEVEL_DEBUG, "Launching `%s` in %s...", filename, editor);

  // Construct the full file path
  char file_path[PATH_MAX];
  snprintf(file_path, sizeof(file_path), "%s/%s", current_path, filename);

  // End NCurses mode before launching the editor
  endwin();

  ignore_sigwinch(); // ignore the signal sent when resizing a window

  // Fork a new process to launch the editor
  pid_t pid = fork();
  if (pid == 0)
  {
    // Child process
    char* args[] = {(char*)editor, file_path, NULL};
    execvp(editor, args); // Execute the editor

    // If execvp fails
    perror("Failed to launch environment variable");
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    // Fork failed
    log_message(LOG_LEVEL_ERROR, "Fork failed.");
    show_term_message("Error while forking process", 1);
  }
  else
  {
    // Parent process
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      log_message(LOG_LEVEL_DEBUG, "%s: Exited editor successfully", filename);
    }
    else
    {
      log_message(LOG_LEVEL_ERROR, "Error while executing `%s`", editor);
      show_term_message("Error while calling editor", 1);
    }
  }

  // Restore default SIGWINCH handler after the editor exits
  restore_sigwinch();
  if (isVISUAL == 1)
  {
    sleep(1);
  }

  // Reinitialize NCurses mode after the editor exits
  initscr();
  cbreak();
  noecho();
  keypad(win, TRUE);
  refresh();
  clear();
}

void print_permissions(WINDOW* info_win, struct stat* file_stat)
{

  wattron(info_win, COLOR_PAIR(4));
  wprintw(info_win, (S_ISDIR(file_stat->st_mode)) ? "d" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IRUSR) ? "r" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IWUSR) ? "w" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IXUSR) ? "x" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IRGRP) ? "r" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IWGRP) ? "w" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IXGRP) ? "x" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IROTH) ? "r" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IWOTH) ? "w" : "-");
  wprintw(info_win, (file_stat->st_mode & S_IXOTH) ? "x" : "-");
  wattroff(info_win, COLOR_PAIR(4));
}

/*
 * AN EXPLANATION OF HOW A FEW FUNCTIONS WITH POPEN/SYSTEM CALLS ARE SUPPOSED TO
 * BE STRUCTURED:
 *
 * SUCH FUNCTIONS ARE SUSCEPTIBLE TO CWE (COMMON WEAKNESS ENUMERATIONS) LIKE 78,
 * 88 WHICH INVOLVE OS COMMAND INJECTION, STRING MANIPULATION SO ON
 *
 * IN ORDER TO FIX SUCH ISSUES,
 *
 *  -> We create a pipe using pipe to communicate between the parent and child
 * processes.
 *
 *  -> We fork a child process using fork.
 *
 *  -> In the child process, we close the read end of the pipe and execute the
 * command using execlp, passing the arguments separately to avoid command
 * injection.
 *
 *  -> In the parent process, we close the write end of the pipe and read from
 * the pipe using fdopen and fgets. We display the output using mvwprintw.
 *
 *  -> We wait for the child process to finish using waitpid.
 *
 */

void display_archive_contents(WINDOW* info_win, const char* full_path, const char* file_ext)
{
  // Validate and sanitize user input
  char* file_name = basename(full_path);
  char* dir_name  = dirname(full_path);
  if (file_name == NULL || dir_name == NULL)
  {
    show_message(info_win, "Invalid file path.");
    return;
  }

  // Whitelist valid file extensions
  const char* valid_exts[] = {".zip", ".7z", ".tar", ".gz"};
  int         ext_len      = strlen(file_ext);
  bool        is_valid_ext = false;
  for (int i = 0; i < sizeof(valid_exts) / sizeof(valid_exts[0]); i++)
  {
    if (strcmp(file_ext, valid_exts[i]) == 0)
    {
      is_valid_ext = true;
      break;
    }
  }
  if (!is_valid_ext)
  {
    show_message(info_win, "Unsupported file extension.");
    return;
  }

  // Construct the command
  char* cmd;
  if (strcmp(file_ext, ".zip") == 0)
  {
    asprintf(&cmd, "unzip -l %s/%s", dir_name, file_name);
  }
  else if (strcmp(file_ext, ".7z") == 0)
  {
    asprintf(&cmd, "7z l %s/%s", dir_name, file_name);
  }
  else
  {
    asprintf(&cmd, "tar -tvf %s/%s", dir_name, file_name);
  }

  // Execute the command and capture its output
  FILE* fp = popen(cmd, "r");
  if (fp == NULL)
  {
    show_message(info_win, "Error executing command.");
    return;
  }

  // Read from the pipe and display the output
  char line[256];
  int  line_num = 12;
  while (fgets(line, sizeof(line), fp) != NULL && line_num < getmaxy(info_win) - 1)
  {
    mvwprintw(info_win, line_num++, 2, "%s", line);
  }
  pclose(fp);

  // Refresh the window to display the output
  wrefresh(info_win);
}
