// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/stat.h>

double system_free_space(const char *path) {
    struct statvfs stat;

    if (statvfs(path, &stat) != 0) {
        perror("statvfs");
        exit(EXIT_FAILURE);
    }

    unsigned long free_space = stat.f_bsize * stat.f_bavail;
    double free_space_gib = (double)free_space / (1 << 30);  // Convert to GiB
    return free_space_gib;
}

long get_file_size(const char *file_path) {
    struct stat st;
    if (stat(file_path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}
