// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/* BY nots1dd */

#include "../include/archivecontrol.h"
#include "../include/cursesutils.h"
#include "../include/dircontrol.h"

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

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
  {
    const char* current_file = archive_entry_pathname(entry);
    char        full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", extraction_dir, current_file);
    archive_entry_set_pathname(entry, full_path);

    if ((r = archive_write_header(ext, entry)) != ARCHIVE_OK)
    {
      fprintf(stderr, "%s\n", archive_error_string(ext));
    }
    else
    {
      copy_data(a, ext);
      archive_write_finish_entry(ext);
    }
  }

  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);

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
      fprintf(stderr, "%s\n", archive_error_string(aw));
      return r;
    }
  }
}

int add_directory_to_archive(struct archive* a, const char* dir_path, const char* base_path)
{
  struct archive_entry* entry;
  char                  full_path[PATH_MAX];
  struct dirent*        dp;
  DIR*                  dir;
  struct stat           st;

  // Open the directory using file descriptor
  int dir_fd = open(dir_path, O_RDONLY | O_DIRECTORY);
  if (dir_fd == -1)
  {
    perror("open");
    return -1;
  }

  dir = fdopendir(dir_fd);
  if (dir == NULL)
  {
    perror("fdopendir");
    close(dir_fd);
    return -1;
  }

  while ((dp = readdir(dir)) != NULL)
  {
    // Skip "." and ".."
    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
    {
      continue;
    }

    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, dp->d_name);
    if (fstatat(dir_fd, dp->d_name, &st, 0) == -1)
    {
      perror("fstatat");
      closedir(dir);
      close(dir_fd);
      return -1;
    }

    entry = archive_entry_new();
    archive_entry_set_pathname(entry, full_path + strlen(base_path) + 1);
    archive_entry_set_size(entry, st.st_size);
    archive_entry_set_filetype(entry, S_ISDIR(st.st_mode) ? AE_IFDIR : AE_IFREG);
    archive_entry_set_perm(entry, st.st_mode & 0777);

    if (archive_write_header(a, entry) != ARCHIVE_OK)
    {
      fprintf(stderr, "archive_write_header: %s\n", archive_error_string(a));
      archive_entry_free(entry);
      closedir(dir);
      close(dir_fd);
      return -1;
    }

    if (!S_ISDIR(st.st_mode))
    {
      int file_fd = openat(dir_fd, dp->d_name, O_RDONLY);
      if (file_fd == -1)
      {
        perror("openat");
        archive_entry_free(entry);
        closedir(dir);
        close(dir_fd);
        return -1;
      }

      FILE* file = fdopen(file_fd, "rb");
      if (file == NULL)
      {
        perror("fdopen");
        close(file_fd);
        archive_entry_free(entry);
        closedir(dir);
        close(dir_fd);
        return -1;
      }

      char    buffer[8192];
      ssize_t len;
      while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0)
      {
        if (archive_write_data(a, buffer, len) != len)
        {
          fprintf(stderr, "archive_write_data: %s\n", archive_error_string(a));
          fclose(file);
          close(file_fd);
          archive_entry_free(entry);
          closedir(dir);
          close(dir_fd);
          return -1;
        }
      }
      fclose(file);
      close(file_fd);
    }

    archive_entry_free(entry);
  }

  closedir(dir);
  close(dir_fd);
  return 0;
}

int compress_directory(const char* dir_path, const char* archive_path, int format)
{
  struct archive* archive_writer;
  int             r;

  // Create a new archive for writing
  archive_writer = archive_write_new();

  if (format == 1)
  {                                                          // TAR format
    archive_write_set_format_pax_restricted(archive_writer); // Use TAR format
  }
  else if (format == 2)
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

  return 0;
}
