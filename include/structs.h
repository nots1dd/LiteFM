#ifndef STRUCTS_H
#define STRUCTS_H

#include <limits.h>

typedef struct
{
  char name[NAME_MAX];
  int  is_dir;
} FileItem;

typedef struct
{
  char path[PATH_MAX];
  int  highlight;
} DirHistory;

#endif
