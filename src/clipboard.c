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
    pid_t pid = fork();

    if (pid == 0) { // Child process
        // Create a pipe to capture the output of rsync
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }

        pid_t rsync_pid = fork();

        if (rsync_pid == 0) { // Grandchild process (rsync)
            close(pipefd[0]); // Close read end
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
            dup2(pipefd[1], STDERR_FILENO); // Redirect stderr to pipe
            close(pipefd[1]);

            // Replace the child process with the rsync command
            execlp("rsync", "rsync", "-a", "--progress", sourceFile, destinationFile, NULL);

            // If execlp fails, exit the child process
            perror("Error executing rsync");
            exit(EXIT_FAILURE);
        } else if (rsync_pid > 0) { // Child process (main program)
            close(pipefd[1]); // Close write end

            // Create and display the progress window
            FILE *rsync_output = fdopen(pipefd[0], "r");
            if (rsync_output == NULL) {
                perror("Error opening pipe for reading");
                exit(EXIT_FAILURE);
            }

            show_term_message(" Copying...", -1);

            pclose(rsync_output);


            int status;
            waitpid(rsync_pid, &status, 0); // Wait for the rsync process to complete
            exit(WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE);
        } else {
            perror("Error forking rsync process");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) { // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            // Notify user about the successful copy
            char message[256];
            snprintf(message, sizeof(message), "[SUCCESS] Copied 📄 '%s' to '%s'.", sourceFile, destinationFile);
            log_message(LOG_LEVEL_INFO, "Successfully copied '%s' to '%s'.", sourceFile, destinationFile);
            show_term_message(message, 0);
        } else {
            perror("Error copying file with rsync");
        }
    } else {
        perror("Error forking process");
    }
}







