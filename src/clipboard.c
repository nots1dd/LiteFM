// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include "../clipboard.h"
#include "../cursesutils.h"

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
    char command[MAX_PATH_LENGTH + 50];
    
    if (display_server) {
        // Wayland environment detected, use wl-copy
        snprintf(command, sizeof(command), "echo '%s' | wl-copy", copied_item);
    } else {
        // Default to xclip for X11
        snprintf(command, sizeof(command), "echo '%s' | xclip -selection clipboard", copied_item);
    }
    
    int result = system(command);
    if (result == -1) {
        show_term_message("Failed to copy to clipboard.", 1);
    }
}




