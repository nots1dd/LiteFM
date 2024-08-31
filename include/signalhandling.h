// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        signalhandling.h
 *
 *  Description: Header file for handling signals that might
 *               lead to unexpected errors or issues
 *
 *  Author:      Siddharth Karanam
 *  Created:     <03/08/24>
 *
 *  Copyright:   <2024> nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       This header provides function declarations for signwinch,
 *               the signal sent to terminal when a resizing event occurs.
 *
 *  Revision History:
 *      <03/08/24> - <Initial Creation>
 *      <31/08/24> - Made all funcs static
 *
 * ---------------------------------------------------------------------------
 */

#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <sys/wait.h>

static void ignore_sigwinch()
{
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, NULL);
}

// Function to restore default SIGWINCH handler
static void restore_sigwinch()
{
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, NULL);
}

#endif
