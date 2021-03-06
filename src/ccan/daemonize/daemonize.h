/*
 * (C) Copyright 2009-2013
 * Rusty Russell <rusty@rustcorp.com.au>
 *
 * SPDX-License-Identifier:	MIT
 */

#pragma once

#include <stdbool.h>

/**
 * daemonize - turn this process into a daemon.
 *
 * This routine forks us off to become a daemon.  It returns false on failure
 * (eg. fork(), chdir or open failed) and sets errno.
 *
 * Side effects for programmers to be aware of:
 *  - PID changes (our parent exits, we become child of init)
 *  - stdin and stdout file descriptors are closed
 *  - stderr is reopened to /dev/null so you don't reuse it
 *  - Current working directory changes to /
 *  - Umask is set to 0.
 */
bool daemonize(void);
