
#include "../include/arg_helpers.h"
#include "../include/dircontrol.h"
#include "../include/logging.h"

void show_help()
{
  // Print the header in blue color
  printf("\nLiteFM - Lightweight Terminal File Manager\n");
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
  printf("\nLiteFM Version - v2.2\n");

  // Print OS and Kernel Information
  struct utsname sysinfo;
  if (uname(&sysinfo) == 0)
  {
    printf("\nOperating System: %s\n", sysinfo.sysname);
    printf("Kernel Version: %s\n", sysinfo.release);
  }
  else
  {
    printf("Error retrieving OS information.\n");
  }

  // Check and print the package manager
  printf("\nPackage Manager: ");
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

  // Check and print the display server
  /*printf("\n%sDisplay Server:%s %s\n",  current_display_server());*/

  // Print filesystem type
  printf("\nFilesystem Type: ");
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
  printf("\nRSYNC:      %s\n", command_exists("rsync") ? "Installed" : "Not Installed");
  printf("NCURSES:      %s\n", library_installed("libncurses") ? "Installed" : "Not Installed");
  printf("LIBARCHIVE:   %s\n", library_installed("libarchive") ? "Installed" : "Not Installed");
  printf("LIBYAML:      %s\n", library_installed("libyaml") ? "Installed" : "Not Installed");
  printf("--------------------------------------\n");
  printf("UNZIP:        %s\n", library_installed("zip") ? "Installed" : "Not Installed");
  printf("TAR:          %s\n", library_installed("tar") ? "Installed" : "Not Installed");
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
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
      endwin();
      show_help();
      return 0;
    }
    else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
    {
      endwin();
      show_version();
      return 0;
    }
    else if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--log-dir") == 0)
    {
      endwin();
      printf("Log file at: ~/%s\n", LOG_FILE_RELATIVE_PATH);
      return 0;
    }
    else if (strcmp(argv[1], "-lc") == 0 || strcmp(argv[1], "--log-clear") == 0)
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
  } else {
    get_current_working_directory(current_path, PATH_MAX);
  }
  return 1;
}
