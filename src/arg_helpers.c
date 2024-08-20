
#include "../include/arg_helpers.h"

void show_help() {
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
    printf("  When creating an issue in the repository, it is advised to also attach litefm.log as well.\n");
}

// Function to check if a command exists in the system's PATH
int command_exists(const char *cmd) {
    char path[1024];
    snprintf(path, sizeof(path), "command -v %s > /dev/null 2>&1", cmd);
    return system(path) == 0;
}

// Function to check if a library is installed
int library_installed(const char *library) {
    char path[1024];
    snprintf(path, sizeof(path), "ldconfig -p | grep %s > /dev/null 2>&1", library);
    return system(path) == 0;
}

// Function to check the currently running display server
const char *current_display_server() {
    FILE *fp = popen("echo $XDG_SESSION_TYPE", "r");
    if (fp) {
        static char display_server[32];
        if (fgets(display_server, sizeof(display_server), fp) != NULL) {
            // Remove newline character
            display_server[strcspn(display_server, "\n")] = '\0';
            pclose(fp);
            if (strcmp(display_server, "wayland") == 0) {
                return "Wayland";
            } else if (strcmp(display_server, "x11") == 0) {
                return "X11";
            }
        }
        pclose(fp);
    }
    return "Unknown or not installed";
}

void show_version() {
    // Print the version of LiteFM
    printf("\n%sLiteFM Version - v2.2%s\n", COLOR_BLUE, COLOR_RESET);

    // Print OS and Kernel Information
    struct utsname sysinfo;
    if (uname(&sysinfo) == 0) {
        printf("\n%sOperating System:%s %s\n", COLOR_GREEN, COLOR_RESET, sysinfo.sysname);
        printf("%sKernel Version:%s %s\n", COLOR_GREEN, COLOR_RESET, sysinfo.release);
    } else {
        printf("%sError retrieving OS information.%s\n", COLOR_RED, COLOR_RESET);
    }

    // Check and print the package manager
    printf("\n%sPackage Manager:%s ", COLOR_GREEN, COLOR_RESET);
    if (command_exists("apt")) {
        printf("APT (Debian/Ubuntu)\n");
    } else if (command_exists("yum")) {
        printf("YUM (RedHat/CentOS)\n");
    } else if (command_exists("dnf")) {
        printf("DNF (Fedora)\n");
    } else if (command_exists("pacman")) {
        printf("Pacman (Arch Linux)\n");
    } else {
        printf("Unknown or not installed\n");
    }

    // Check and print the display server
    printf("\n%sDisplay Server:%s %s\n", COLOR_GREEN, COLOR_RESET, current_display_server());

    // Print filesystem type
    printf("\n%sFilesystem Type:%s ", COLOR_GREEN, COLOR_RESET);
    FILE *fp = popen("df -T / | awk 'NR==2 {print $2}'", "r");
    if (fp) {
        char fs_type[32];
        if (fgets(fs_type, sizeof(fs_type), fp) != NULL) {
            printf("%s", fs_type);
        } else {
            printf("Unable to determine filesystem type.\n");
        }
        pclose(fp);
    } else {
        printf("Error retrieving filesystem type.\n");
    }

    // Check for specific libraries/tools
    printf("\n%srsync:%s %s\n", COLOR_GREEN, COLOR_RESET, command_exists("rsync") ? "Installed" : "Not Installed");
    printf("%sncurses:%s %s\n", COLOR_GREEN, COLOR_RESET, library_installed("libncurses") ? "Installed" : "Not Installed");
    printf("%slibarchive:%s %s\n", COLOR_GREEN, COLOR_RESET, library_installed("libarchive") ? "Installed" : "Not Installed");
    printf("%slibyaml:%s %s\n", COLOR_GREEN, COLOR_RESET, library_installed("libyaml") ? "Installed" : "Not Installed");
    printf("--------------------------------------\n");
    printf("%sunzip:%s %s\n", COLOR_GREEN, COLOR_RESET, library_installed("zip") ? "Installed" : "Not Installed");
    printf("%star:%s %s\n", COLOR_GREEN, COLOR_RESET, library_installed("tar") ? "Installed" : "Not Installed");
    // Add checks for other libraries or tools similarly
}
