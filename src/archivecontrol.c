// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>


// Function to extract an archive specified by archive_path
int extract_archive(const char *archive_path) {
    char command[PATH_MAX + 100]; // +100 for safety margin
    int result;
    long file_size_before, file_size_after; 

    // Get the directory of the archive (example.zip)
    char archive_dir[PATH_MAX];
    strncpy(archive_dir, archive_path, PATH_MAX);
    dirname(archive_dir); // Get the directory part of the archive path

    // Create the extraction directory (filename_extracted)
    char *archive_basename = basename(archive_path);
    char extraction_dir[PATH_MAX];
    snprintf(extraction_dir, sizeof(extraction_dir), "%s/%s_extracted", archive_dir, archive_basename);

    // Determine archive type and construct extraction command
    if (strstr(archive_path, ".zip")) {
        snprintf(command, sizeof(command), "mkdir -p \"%s\" && unzip -o \"%s\" -d \"%s\" > /dev/null 2>&1", extraction_dir, archive_path, extraction_dir);
    } else if (strstr(archive_path, ".tar")) {
        snprintf(command, sizeof(command), "mkdir -p \"%s\" && tar -xf \"%s\" -C \"%s\" > /dev/null 2>&1", extraction_dir, archive_path, extraction_dir);
    } else {
        endwin(); // End NCurses mode before returning
        return -1; // Unsupported archive format
    }

    // Get file size before extraction
    file_size_before = get_file_size(archive_path);

    show_term_message("Extracting...",0);

    // Execute command
    FILE *extract_process = popen(command, "r");
    if (extract_process == NULL) {
        endwin(); // End NCurses mode before returning
        return -1; // Failed to execute command
    } 

    pclose(extract_process); // Close the process

    return 0; // Extraction successful
}

int add_directory_to_archive(struct archive *a, const char *dir_path, const char *base_path) {
    struct archive_entry *entry;
    char full_path[PATH_MAX];
    struct dirent *dp;
    DIR *dir;
    struct stat st;

    // Open the directory
    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    while ((dp = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, dp->d_name);
        if (stat(full_path, &st) == -1) {
            perror("stat");
            closedir(dir);
            return -1;
        }

        entry = archive_entry_new();
        archive_entry_set_pathname(entry, full_path + strlen(base_path) + 1);
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, S_ISDIR(st.st_mode) ? AE_IFDIR : AE_IFREG);
        archive_entry_set_perm(entry, st.st_mode & 0777);

        if (archive_write_header(a, entry) != ARCHIVE_OK) {
            fprintf(stderr, "archive_write_header: %s\n", archive_error_string(a));
            archive_entry_free(entry);
            closedir(dir);
            return -1;
        }

        if (!S_ISDIR(st.st_mode)) {
            FILE *file = fopen(full_path, "rb");
            if (file == NULL) {
                perror("fopen");
                archive_entry_free(entry);
                closedir(dir);
                return -1;
            }

            char buffer[8192];
            ssize_t len;
            while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                if (archive_write_data(a, buffer, len) != len) {
                    fprintf(stderr, "archive_write_data: %s\n", archive_error_string(a));
                    fclose(file);
                    archive_entry_free(entry);
                    closedir(dir);
                    return -1;
                }
            }
            fclose(file);
        }

        archive_entry_free(entry);
    }

    closedir(dir);
    return 0;
}

int compress_directory(const char *dir_path, const char *archive_path, int format) {
    struct archive *archive_writer;
    int r;

    // Create a new archive for writing
    archive_writer = archive_write_new();
    
    if (format == 1) { // TAR format
        archive_write_set_format_pax_restricted(archive_writer); // Use TAR format
    } else if (format == 2) { // ZIP format
        archive_write_set_format_zip(archive_writer); // Use ZIP format
    } else {
        archive_write_free(archive_writer);
        return -1; // Unsupported format
    }

    // Open the archive file for writing
    if (archive_write_open_filename(archive_writer, archive_path) != ARCHIVE_OK) {
        archive_write_free(archive_writer);
        return -1; // Error opening archive file
    }

    // Add files/directories to the archive
    r = add_directory_to_archive(archive_writer, dir_path, dir_path);
    if (r != 0) {
        archive_write_close(archive_writer);
        archive_write_free(archive_writer);
        return -1;
    }

    // Close and free the archive
    archive_write_close(archive_writer);
    archive_write_free(archive_writer);

    return 0;
}
