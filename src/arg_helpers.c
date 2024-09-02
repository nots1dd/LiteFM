
#include "../include/arg_helpers.h"
#include "../include/dircontrol.h"
#include "../include/logging.h"

void show_help()
{
  // Print the header in blue color
  printf("\nLiteFM - Curses based File Manager\n");
  printf("\nUsage:\n");
  printf("lfm [options] [directory]\n");

  // Print options
  printf("\nOptions:\n");
  printf("  -h,  --help        Show this help message and exit\n");
  printf("  -v,  --version     Show version information\n");
  printf("  -l,  --log-dir     Print the logging directory and exit\n");
  printf("  -lc, --log-clear   Clear litefm.log\n");

  // Print arguments
  printf("\nArguments:\n");
  printf("  lfm [DIR]       Open litefm in that directory\n");

  // Print help
  printf("\nHelp:\n");
  printf("  Visit https://github.com/nots1dd/litefm for more information.\n");
  printf("  When creating an issue in the repository, it is advised to also "
         "attach litefm.log as well.\n");
}

// Function to check if a command exists in the system's PATH
int command_exists(const char* cmd)
{
  char path[1024];
  snprintf(path, sizeof(path), "command -v %s > /dev/null 2>&1", cmd);
  return system(path) == 0;
}

// Function to check if a library is installed
int library_installed(const char* library)
{
  char path[1024];
  snprintf(path, sizeof(path), "ldconfig -p | grep %s > /dev/null 2>&1", library);
  return system(path) == 0;
}

// Function to check the currently running display server
const char* current_display_server()
{
  FILE* fp = popen("echo $XDG_SESSION_TYPE", "r");
  if (fp)
  {
    static char display_server[32];
    if (fgets(display_server, sizeof(display_server), fp) != NULL)
    {
      // Remove newline character
      display_server[strcspn(display_server, "\n")] = '\0';
      pclose(fp);
      if (strcmp(display_server, "wayland") == 0)
      {
        return "Wayland";
      }
      else if (strcmp(display_server, "x11") == 0)
      {
        return "X11";
      }
    }
    pclose(fp);
  }
  return "Unknown or not installed";
}

void show_version()
{
  // Print the version of LiteFM
  printf("\n\u24C8 LiteFM Version - %s\n", VERSION);

  // Print OS and Kernel Information
  struct utsname sysinfo;
  if (uname(&sysinfo) == 0)
  {
    printf("\n\u2699 %s: %s\n", OS_LABEL, sysinfo.sysname);
    printf("\u2699 %s: %s\n", KERNEL_LABEL, sysinfo.release);
  }
  else
  {
    printf("%s Error retrieving OS information.\n", CROSS_MARK);
  }

  // Check and print the package manager
  printf("\n\u2318 %s: ", PACKAGE_MANAGER_LABEL);
  if (command_exists("apt"))
  {
    printf("APT (Debian/Ubuntu)\n");
  }
  else if (command_exists("yum"))
  {
    printf("YUM (RedHat/CentOS)\n");
  }
  else if (command_exists("dnf"))
  {
    printf("DNF (Fedora)\n");
  }
  else if (command_exists("pacman"))
  {
    printf("Pacman (Arch Linux)\n");
  }
  else
  {
    printf("Unknown or not installed\n");
  }

  // Print filesystem type
  printf("\n\u267B %s: ", FS_TYPE_LABEL);
  FILE* fp = popen("df -T / | awk 'NR==2 {print $2}'", "r");
  if (fp)
  {
    char fs_type[32];
    if (fgets(fs_type, sizeof(fs_type), fp) != NULL)
    {
      printf("%s", fs_type);
    }
    else
    {
      printf("Unable to determine filesystem type.\n");
    }
    pclose(fp);
  }
  else
  {
    printf("Error retrieving filesystem type.\n");
  }

  // Check for specific libraries/tools
  printf("\n");
  printf("%s RSYNC:        %s\n", command_exists("rsync") ? CHECK_MARK : CROSS_MARK,
         command_exists("rsync") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s NCURSES:      %s\n", library_installed("libncurses") ? CHECK_MARK : CROSS_MARK,
         library_installed("libncurses") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s LIBARCHIVE:   %s\n", library_installed("libarchive") ? CHECK_MARK : CROSS_MARK,
         library_installed("libarchive") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s LIBYAML:      %s\n", library_installed("libyaml") ? CHECK_MARK : CROSS_MARK,
         library_installed("libyaml") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s SDL2:         %s\n", library_installed("SDL2") ? CHECK_MARK : CROSS_MARK,
         library_installed("SDL2") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s SDL2_MIXER:   %s\n", library_installed("SDL2_mixer") ? CHECK_MARK : CROSS_MARK,
         library_installed("SDL2_mixer") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf(HORIZONTAL_LINE "\n");
  printf("%s UNZIP:        %s\n", library_installed("zip") ? CHECK_MARK : CROSS_MARK,
         library_installed("zip") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  printf("%s TAR:          %s\n", library_installed("tar") ? CHECK_MARK : CROSS_MARK,
         library_installed("tar") ? LIBRARY_LABEL : NOT_INSTALLED_LABEL);
  // Add checks for other libraries or tools similarly
}

int handle_arguments(int argc, char* argv[], char* current_path)
{
  if (argc == 2)
  {
    if (is_directory(argv[1]))
    {
      realpath(argv[1], current_path);
    }
    else if (strcmp(argv[1], HELP_SHORT_ARG) == 0 || strcmp(argv[1], HELP_LONG_ARG) == 0)
    {
      endwin();
      show_help();
      return 0;
    }
    else if (strcmp(argv[1], VERSION_SHORT_ARG) == 0 || strcmp(argv[1], VERSION_LONG_ARG) == 0)
    {
      endwin();
      show_version();
      return 0;
    }
    else if (strcmp(argv[1], LOG_DIR_SHORT_ARG) == 0 || strcmp(argv[1], LOG_DIR_LONG_ARG) == 0)
    {
      endwin();
      printf("Log file at: ~/%s\n", LOG_FILE_RELATIVE_PATH);
      return 0;
    }
    else if (strcmp(argv[1], LOG_CLEAR_SHORT_ARG) == 0 || strcmp(argv[1], LOG_CLEAR_LONG_ARG) == 0)
    {
      endwin();
      char termbuf[256];
      snprintf(termbuf, sizeof(termbuf), "echo '' > ~/%s", LOG_FILE_RELATIVE_PATH);
      system(termbuf);
      printf("Cleared log for LiteFM.\n");
      return 0;
    }
    else
    {
      get_current_working_directory(current_path, PATH_MAX);
    }
  }
  else
  {
    get_current_working_directory(current_path, PATH_MAX);
  }
  return 1;
}
