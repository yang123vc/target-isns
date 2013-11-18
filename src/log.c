/*
 * This file is part of target-isns.
 *
 * Copyright (C) 2013 Christophe Vu-Brugier <cvubrugier@yahoo.fr>
 *
 * target-isns is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * target-isns is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with target-isns; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#define _BSD_SOURCE
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/time.h>


static bool use_syslog = true;
static int max_priority = LOG_INFO;
static struct timeval start;


static int timeval_substract(struct timeval *result,
			     struct timeval *x,
			     struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}


void log_init(const char *progname, bool daemon, int priority)
{
	if (daemon)
		openlog(progname, 0, LOG_DAEMON);
	else {
		gettimeofday(&start, NULL);
		use_syslog = false;
	}
	max_priority = priority;
}


void log_close(void)
{
	if (use_syslog)
		closelog();
}


static void __log_print(int priority, const char *format, va_list ap)
{
	if (use_syslog)
		vsyslog(priority, format, ap);
	else {
		struct timeval now, elapsed;
		char c;

		switch (priority) {
		case LOG_ERR:
			c = 'E';
			break;
		case LOG_WARNING:
			c = 'W';
			break;
		case LOG_NOTICE:
			c = 'N';
			break;
		case LOG_INFO:
			c = 'I';
			break;
		case LOG_DEBUG:
			c = 'D';
			break;
		default:
			c = '?';
		}

		gettimeofday(&now, NULL);
		timeval_substract(&elapsed, &now, &start);

		fprintf(stderr, "%4lu.%06lu %c: ",
			elapsed.tv_sec, elapsed.tv_usec, c);
		vfprintf(stderr, format, ap);
		fprintf(stderr, "\n");
		fflush(stderr);
	}
}


void log_print(int priority, const char *format, ...)
{
	if (priority <= max_priority) {
		va_list ap;
		va_start(ap, format);
		__log_print(priority, format, ap);
		va_end(ap);
	}
}
