// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include "../include/clipboard.h"
#include "../include/cursesutils.h"
#include "../include/logging.h"

// Function to copy the selected item name
void yank_selected_item(char *selected_item) {
    static char copied_item[MAX_PATH_LENGTH];

    // Copy the selected item's name to the internal storage
    strncpy(copied_item, selected_item, MAX_PATH_LENGTH - 1);
    copied_item[MAX_PATH_LENGTH - 1] = '\0';  // Ensure null-termination
    
    // Prepare the copy message
    char copy_msg[256];
    snprintf(copy_msg, sizeof(copy_msg), "Copied %s to register.", copied_item);
    
    // Show the message in the terminal
    show_term_message(copy_msg, 0);
    refresh();

    // Determine the display server and use the appropriate clipboard tool
    const char *display_server = getenv("WAYLAND_DISPLAY");
    const char *clipboard_cmd = display_server ? "wl-copy" : "xclip -selection clipboard";
    
    // Use popen to securely write to the clipboard
    FILE *clipboard = popen(clipboard_cmd, "w");
    if (clipboard == NULL) {
        show_term_message("Failed to open clipboard tool.", 1);
        return;
    }

    if (fputs(copied_item, clipboard) == EOF) {
        show_term_message("Failed to write to clipboard.", 1);
    }

    int result = pclose(clipboard);
    if (result == -1) {
        show_term_message("Failed to close clipboard tool.", 1);
    } else if (WIFEXITED(result) && WEXITSTATUS(result) != 0) {
        show_term_message("Clipboard tool exited with an error.", 1);
    }
}

void copyFileContents(const char *sourceFile, const char *destinationFile) {
    FILE *src = fopen(sourceFile, "r");
    if (src == NULL) {
        perror("Error opening source file");
        return;
    }

    FILE *dest = fopen(destinationFile, "w");
    if (dest == NULL) {
        perror("Error opening destination file");
        fclose(src);
        return;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytesRead, dest);
    }

    fclose(src);
    fclose(dest);

    // Notify user about the successful copy
    char message[256];
    snprintf(message, sizeof(message), "[SUCCESS] Copied 📄 '%s' to '%s'.", sourceFile, destinationFile);
    log_message(LOG_LEVEL_INFO, "Successfully copied '%s' to '%s'.", sourceFile, destinationFile);
    show_term_message(message, 0);
}




