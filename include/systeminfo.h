// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        systeminfo.h
 *  Description: Some system info like free space and space in current partition
 *
 *  Author:      Siddharth Karanam
 *  Created:     <29/08/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:      Removed getFreeSpace.c from previous commits to this header
 *
 *  Revision History:
 *      <03/09/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

static double system_free_space(const char* path)
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

static double system_total_space(const char* path)
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

#endif
