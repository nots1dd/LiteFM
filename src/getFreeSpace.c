// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/* BY nots1dd */

#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

double system_free_space(const char* path)
{
  struct statvfs stat;

  if (statvfs(path, &stat) != 0)
  {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }

  unsigned long free_space     = stat.f_bsize * stat.f_bavail;
  double        free_space_gib = (double)free_space / (1 << 30); // Convert to GiB
  return free_space_gib;
}

double system_total_space(const char* path)
{
  struct statvfs stat;

  if (statvfs(path, &stat) != 0)
  {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }

  unsigned long total_space     = stat.f_bsize * stat.f_blocks;
  double        total_space_gib = (double)total_space / (1 << 30); // Convert to GiB
  return total_space_gib;
}
