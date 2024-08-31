// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        filepreview.h
 *  Description: This header file provides function declarations for the file
 *                preview functionality within the LiteFM file manager. It
 *                includes methods for displaying file content in an NCurses
 *                window, checking file extensions for readability, and
 * formatting file sizes for display.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       The functions in this header are designed to work with NCurses
 *                to provide a text-based preview of files, suitable for file
 *                management applications. Ensure that file access permissions
 *                are correctly handled to avoid errors when attempting to read
 *                file content.
 *
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *      <31/08/24> - Refactoring + streamlining of getting file type
 *
 * ---------------------------------------------------------------------------
 */

#ifndef FILEPREVIEW_H
#define FILEPREVIEW_H

#include <ctype.h>
#include <libgen.h>
#include <limits.h> // For PATH_MAX
#include <limits.h>
#include <ncurses.h>
#include <stdio.h> // For FILE, fopen, fgets, fclose, perror
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "highlight.h"

// Constants
#define MAX_LINES 60        // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length

/* TEXT FILE MIME TYPES */

#define MIME_TEXT_PLAIN "text/plain"
#define MIME_TEXT_SHELLSCRIPT "text/x-shellscript"
#define MIME_APPLICATION_JSON "application/json"
#define MIME_TEXT_RUBY "text/x-ruby"
#define MIME_TEXT_C "text/x-c"
#define MIME_TEXT_CPP "text/x-c++"
#define MIME_TEXT_PYTHON "text/x-script.python"
#define MIME_TEXT_JAVA "text/x-java-source"
#define MIME_APPLICATION_OCTET_STREAM "application/octet-stream"
#define MIME_TEXT_MAKEFILE "text/x-makefile"
#define MIME_TEXT_HTML "text/html"
#define MIME_TEXT_CSS "text/css"
#define MIME_EMPTY "inode/x-empty"

/* AUDIO FILE MIME TYPES */

#define MIME_AUDIO_MPEG "audio/mpeg"
#define MIME_AUDIO_WAV "audio/x-wav"
#define MIME_AUDIO_AIFF "audio/x-aiff"
#define MIME_AUDIO_OGG "audio/ogg"
#define MIME_AUDIO_FLAC "audio/flac"
#define MIME_AUDIO_MATROSKA "audio/x-matroska"

/* VIDEO FILE MIME TYPES */

#define MIME_VIDEO_MP4 "video/mp4"
#define MIME_VIDEO_AVI "video/x-msvideo"
#define MIME_VIDEO_MATROSKA "video/x-matroska"
#define MIME_VIDEO_WMV "video/x-ms-wmv"
#define MIME_VIDEO_WEBM "video/webm"
#define MIME_VIDEO_FLV "video/x-flv"
#define MIME_VIDEO_ASF "video/x-ms-asf"

/* IMAGE FILE MIME TYPES */

#define MIME_IMAGE_JPEG "image/jpeg"
#define MIME_IMAGE_PNG "image/png"
#define MIME_IMAGE_GIF "image/gif"
#define MIME_IMAGE_BMP "image/bmp"
#define MIME_IMAGE_ICON "image/x-icon"
#define MIME_IMAGE_TIFF "image/tiff"
#define MIME_IMAGE_WEBP "image/webp"

/* CONST RETURN TYPES */

#define READABLE "READ"
#define AUDIO "AUDIO"
#define VIDEO "VIDEO"
#define IMAGE "IMAGE"

// Function Prototypes
const char* get_file_extension(const char* filename);
void        display_file(WINDOW* info_win, const char* filename);
const char* determine_file_type(const char* filename);
const char* is_readable_extension(const char* filename, const char* current_path);
const char* format_file_size(off_t size);
int         is_image(const char* filename);
int         is_audio(const char* filename);
void launch_env_var(WINDOW* win, const char* current_path, const char* filename, const char* type);
void print_permissions(WINDOW* info_win, struct stat* file_stat);
void display_archive_contents(WINDOW* info_win, const char* full_path, const char* file_ext);

#endif // FILEPREVIEW_H
