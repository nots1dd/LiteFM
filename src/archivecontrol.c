// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/* BY nots1dd */

#include "../include/archivecontrol.h"
#include "../include/cursesutils.h"
#include "../include/dircontrol.h"
#include "../include/logging.h"

int copy_data(struct archive* ar, struct archive* aw);

long get_file_size(const char* file_path)
{
  struct stat st;
  if (stat(file_path, &st) == 0)
  {
    return st.st_size;
  }
  return -1;
}

int ensure_directory_exists(const char* dir)
{
  struct stat st = {0};
  if (stat(dir, &st) == -1)
  {
    if (mkdir(dir, 0700) != 0)
    {
      return -1; // Failed to create directory
    }
  }
  return 0; // Directory exists or was successfully created
}

// Function to extract an archive specified by archive_path
int extract_archive(const char* archive_path)
{
  struct archive*       a;
  struct archive*       ext;
  struct archive_entry* entry;
  int                   flags;
  int                   r;
  char                  archive_dir[PATH_MAX];
  char*                 archive_basename;
  char                  extraction_dir[PATH_MAX];
  clock_t               start_time, end_time;
  double                time_taken;

  // Get the directory of the archive
  strncpy(archive_dir, archive_path, PATH_MAX);
  archive_basename = basename(archive_dir); // Get the base name part of the archive path
  dirname(archive_dir);                     // Get the directory part of the archive path

  // Create the extraction directory (filename_extracted)
  snprintf(extraction_dir, sizeof(extraction_dir), "%s/%s_extracted", archive_dir,
           archive_basename);

  // Ensure extraction directory exists
  if (ensure_directory_exists(extraction_dir) != 0)
  {
    endwin();  // End NCurses mode before returning
    return -1; // Failed to create extraction directory
  }

  // Select the type of archive to open
  flags = ARCHIVE_EXTRACT_TIME;
  a     = archive_read_new();
  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);

  if ((r = archive_read_open_filename(a, archive_path, 10240)))
  {
    archive_read_free(a);
    archive_write_free(ext);
    endwin();  // End NCurses mode before returning
    return -1; // Failed to open archive
  }

  show_term_message("Extracting...", 0);
  log_message(LOG_LEVEL_INFO, "---- Extracting %s ----", extraction_dir);

  // Start measuring time
  start_time = clock();

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
  {
    const char* current_file = archive_entry_pathname(entry);
    char        full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", extraction_dir, current_file);
    archive_entry_set_pathname(entry, full_path);

    if ((r = archive_write_header(ext, entry)) != ARCHIVE_OK)
    {
      log_message(LOG_LEVEL_ERROR, "  %s", archive_error_string(ext));
    }
    else
    {
      copy_data(a, ext);
      archive_write_finish_entry(ext);
    }
  }

  // End measuring time
  end_time   = clock();
  time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  // Close and free archives
  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);

  // Log and display the extraction time
  log_message(LOG_LEVEL_INFO, "Time taken for extraction: %.2f seconds", time_taken);
  log_message(LOG_LEVEL_INFO, "---- Extraction done ----");

  char extraction_message[100];
  snprintf(extraction_message, sizeof(extraction_message),
           "Extraction successful. Time taken: %.2f seconds.", time_taken);
  show_term_message(extraction_message, 0);

  return 0; // Extraction successful
}

int copy_data(struct archive* ar, struct archive* aw)
{
  const void* buff;
  size_t      size;
  la_int64_t  offset;

  while (1)
  {
    int r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return ARCHIVE_OK;
    if (r != ARCHIVE_OK)
      return r;
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK)
    {
      log_message(LOG_LEVEL_ERROR, "%s", archive_error_string(aw));
      return r;
    }
  }
}

int add_directory_to_archive(struct archive* archive_writer, const char* dir_path, const char* base_path)
{
    struct dirent* entry;
    struct stat    statbuf;
    char           full_path[PATH_MAX];
    DIR*           dir;

    // Open the directory
    dir = opendir(dir_path);
    if (!dir)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Construct the full path of the file/directory
        snprintf(full_path, PATH_MAX, "%s/%s", dir_path, entry->d_name);

        // Get file status
        if (lstat(full_path, &statbuf) == -1)
        {
            perror("lstat");
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            // Recursively add subdirectories
            if (add_directory_to_archive(archive_writer, full_path, base_path) != 0)
            {
                closedir(dir);
                return -1;
            }
        }
        else
        {
            // Add file to archive
            struct archive_entry* entry = archive_entry_new();
            archive_entry_set_pathname(entry, full_path + strlen(base_path) + 1); // Relative path
            archive_entry_set_size(entry, statbuf.st_size);
            archive_entry_set_filetype(entry, statbuf.st_mode);
            archive_entry_set_perm(entry, statbuf.st_mode);

            archive_write_header(archive_writer, entry);

            if (S_ISREG(statbuf.st_mode))
            {
                // Write file data to archive
                FILE* file = fopen(full_path, "rb");
                if (!file)
                {
                    perror("fopen");
                    archive_entry_free(entry);
                    closedir(dir);
                    return -1;
                }

                char buffer[8192];
                size_t bytes_read;
                while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
                {
                    archive_write_data(archive_writer, buffer, bytes_read);
                }

                fclose(file);
            }

            archive_entry_free(entry);
        }
    }

    closedir(dir);
    return 0;
}

int compress_directory(const char* dir_path, const char* archive_path, int format)
{
    struct archive* archive_writer;
    int             r;
    struct timespec start_time, end_time;
    double          elapsed_time;

    // Start the timer
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Create a new archive for writing
    archive_writer = archive_write_new();

    if (format == TAR_COMPRESSION_FORMAT)
    {                                                          // TAR format
        archive_write_set_format_pax_restricted(archive_writer); // Use TAR format
    }
    else if (format == ZIP_COMPRESSION_FORMAT)
    {                                               // ZIP format
        archive_write_set_format_zip(archive_writer); // Use ZIP format
    }
    else
    {
        archive_write_free(archive_writer);
        return -1; // Unsupported format
    }

    // Open the archive file for writing
    if (archive_write_open_filename(archive_writer, archive_path) != ARCHIVE_OK)
    {
        archive_write_free(archive_writer);
        return -1; // Error opening archive file
    }

    // Add files/directories to the archive
    r = add_directory_to_archive(archive_writer, dir_path, dir_path);
    if (r != 0)
    {
        archive_write_close(archive_writer);
        archive_write_free(archive_writer);
        return -1;
    }

    // Close and free the archive
    archive_write_close(archive_writer);
    archive_write_free(archive_writer);

    // Stop the timer
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate elapsed time in seconds
    elapsed_time =
        (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    // Print the elapsed time
    char message[PATH_MAX];
    snprintf(message, PATH_MAX, "Directory compression completed in %.2f seconds.", elapsed_time);
    show_term_message(message, 0);

    return 0;
}
