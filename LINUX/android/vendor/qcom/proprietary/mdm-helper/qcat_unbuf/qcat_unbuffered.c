/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <pthread.h>
#include <malloc.h>
#include <getopt.h>
#include <limits.h>
#include <poll.h>

#ifdef LINUX_PC

#define LOGE(tag, ...)  do{printf ("%s ", tag); printf ("ERROR:"); printf (__VA_ARGS__);}while(0)
#define LOGW(tag, ...)  do{printf ("%s ", tag); printf ("WARN:"); printf (__VA_ARGS__);}while(0)
#define LOGI(tag, ...)  do{printf ("%s ", tag); printf ("INFO:"); printf (__VA_ARGS__);}while(0)
#define LOGD(tag, ...)  do{printf ("%s ", tag); printf ("DEBUG:"); printf (__VA_ARGS__);}while(0)


#else /* Not on Linux PC, then android*/
#include <android/log.h>
#define LOGE(tag, ...) __android_log_print (ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#define LOGW(tag, ...) __android_log_print (ANDROID_LOG_WARN, tag, __VA_ARGS__)
#define LOGI(tag, ...) __android_log_print (ANDROID_LOG_INFO, tag, __VA_ARGS__)
#define LOGD(tag, ...) __android_log_print (ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#endif

#define PLOGE(...)  do {LOGE("QCAT_UNBUF", __VA_ARGS__); printf (__VA_ARGS__);}while(0)
#define PLOGW(...)  LOGW("QCAT_UNBUF", __VA_ARGS__)
#define PLOGI(...)  LOGI("QCAT_UNBUF", __VA_ARGS__)
#define PLOGD(...)  LOGD("QCAT_UNBUF", __VA_ARGS__)

#define SIZE_OF_READ_BUFFER (1000)

#define LOGCAT_MAX_SIZE 1023

#if SIZE_OF_READ_BUFFER > LOGCAT_MAX_SIZE
#error "The size of the buffer has to be less than the logcat size to properly print into logcat"
#endif

/* Leave one extra byte for terminating the string. */
char scratch_buffer[SIZE_OF_READ_BUFFER + 1];


struct qcat_args
{
  char *infile;  /* Input file to read from. */
  char *outfile; /* Output file name */
  unsigned int open_sleep_msec; /* milliseconds to sleeps between open calls */
  unsigned int open_retry_count; /* Number of times to retry the open. */
  char *logcat_tag; /* Output to logtag with the following tag. */
  unsigned long read_timeout; /* Bail if no data comes in after this ammount of time in milliseconds */
  char open_sleep_set;
  char open_retry_set;
  char read_timeout_set;
};

struct qcat_args qcat_args_g;

static int qcat_write_file (int fd_out, char *buffer, ssize_t size)
{
  long int written;
  int result = 0;

  while (size > 0)
  {
    written = write (fd_out, buffer, size);
    PLOGD ("Wrote %ld bytes to output file\n", written);
    if (written < 0)
    {
      PLOGE ("Can't write to output file (%ld) %d:%s\n",
            written, errno, strerror (errno));
      result = -1;
      break;
    }
    result = fsync (fd_out);
    if (result != 0)
    {
      PLOGE ("Can't sync while writing to output file (%d) %d:%s\n",
            result, errno, strerror (errno));
      /* We intentionally don't bail here hoping fsync will work on the next
       * call. */
    }
    buffer += written;
    size -= written;
  }

  return result;
}

static int qcat_write_logcat (char *buffer, ssize_t size, char *prefix)
{
  if (size == 0)
    return 0;

  /* Make sure the string is zero terminated.*/
  buffer[size] = 0;

  LOGI(prefix, "%s\n", buffer);
  printf ("%s", buffer);

  return 0;
}

static int qcat_write (int fd_out, char *buffer, ssize_t size, char *prefix)
{
  int result;
  if (fd_out < 0)
    result = qcat_write_logcat (buffer, size, prefix);
  else
    result = qcat_write_file (fd_out, buffer, size);
  return result;
}


static int qcat_unbuffered_fd (int fd_in, int fd_out, char *prefix,
                               int read_timeout)
{
  long int read_bytes;
  unsigned int buf_size = SIZE_OF_READ_BUFFER;
  char *buffer = scratch_buffer;
  int result = 0;

  do
  {
    if (read_timeout != 0)
    {
      struct pollfd fds[1];
      fds[0].fd = fd_in;
      fds[0].events = POLLIN;
      PLOGD ("Waiting for data for %d ms\n", read_timeout);
      result = poll (fds, 1, read_timeout);
      if (result < 0)
      {
        PLOGE ("Poll on input file returned error. %d:%s\n",
               errno, strerror (errno));
        goto cleanup;
      }
      else if (result == 0)
      {
        PLOGE ("Poll  on input file Timed out (%d), will quit.\n",
               read_timeout);
        goto cleanup;
      }
    }

    PLOGD ("Reading from input file (%d)\n", buf_size);
    buffer = scratch_buffer;
    read_bytes = read(fd_in, buffer, buf_size);
    PLOGD ("Read %ld bytes from input\n", read_bytes);

    if (read_bytes < 0)
    {
      PLOGE ("Can't read from input file (%ld) %d:%s\n",
            read_bytes, errno, strerror (errno));
      result = -1;
      goto cleanup;
    }

    result = qcat_write (fd_out, buffer, read_bytes, prefix);
    if (result != 0)
    {
      break;
    }
  } while (read_bytes > 0);

cleanup:
  return result;
}


static int qcat_validate_params (struct qcat_args *qcat_args)
{
  int result = 0;

  /* Validate parameters in the structure. */
  if (qcat_args->infile == NULL)
  {
    result = -1;
    PLOGE ("Required argument --infile\n");
  }
  if (qcat_args->outfile == NULL && qcat_args->logcat_tag == NULL)
  {
    result = -1;
    PLOGE ("No output specified, use --outfile or --logcat_tag\n");
  }
  if (qcat_args->open_sleep_set == 0)
  {
    result = -1;
    PLOGE ("Required argument --open_sleep missing\n");
  }
  if (qcat_args->open_retry_set == 0)
  {
    result = -1;
    PLOGE ("Required argument --open_retry missing\n");
  }
  if (qcat_args->read_timeout_set == 0)
  {
    result = -1;
    PLOGE ("Required argument --read_timeout missing\n");
  }
  else if (qcat_args->read_timeout > INT_MAX)
  {
    result = -1;
    PLOGE ("Argument --read_timeout larger than supported %ld > %d\n", qcat_args->read_timeout, INT_MAX);
  }

  return result;
}

static int qcat_open_file_retry (char *infile, unsigned int retry_count,
                                 unsigned int sleep_ms)
{
  int fd_in = -1;
  unsigned int retry = 0;
  time_t secs;
  struct timespec sleep_time, remaining;
  secs = sleep_ms / 1000;
  sleep_time.tv_sec = secs;
  secs = secs * 1000;
  sleep_time.tv_nsec = ((sleep_ms - secs) * 1000 * 1000);

  for (retry = 0; retry <= retry_count; retry++)
  {
    fd_in = open (infile, O_RDONLY);
    if (fd_in < 0)
    {
      PLOGD ("Failed to open input file %d:%s, check if retry %d,%d\n",
            errno, strerror (errno), retry, retry_count);
      /* Sleep on every retry except the last one. */
      if (retry < retry_count)
        nanosleep(&sleep_time, &remaining);
    }
  }

  return fd_in;
}


int qcat_unbuffered_main (struct qcat_args *qcat_args)
{
  int fd_in = -1;
  int fd_out = -1;
  int result = 0;

  if (qcat_validate_params (qcat_args) != 0)
    return -1;

  fd_in = qcat_open_file_retry (qcat_args->infile, qcat_args->open_retry_count,
                                qcat_args->open_sleep_msec);
  if (fd_in < 0)
  {
    PLOGE ("Failed to open input file %s, error code %d:%s\n",
          qcat_args->infile, errno, strerror (errno));
    result = -1;
    goto cleanup;
  }
  PLOGD ("Successfully open input file %s\n", qcat_args->infile);

  if (qcat_args->outfile != NULL)
  {
    fd_out = open (qcat_args->outfile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd_out < 0)
    {
      PLOGE ("Failed to open output file %s, error code %d (%s)\n",
            qcat_args->outfile, errno, strerror (errno));
      result = -1;
      goto cleanup;
    }
    PLOGD ("Successfully open input file %s\n", qcat_args->outfile);
  }

  result = qcat_unbuffered_fd (fd_in, fd_out, qcat_args->logcat_tag,
                               qcat_args->read_timeout);

cleanup:
  if (fd_in >= 0)
    close (fd_in);

  if (fd_out >= 0)
    close (fd_out);

  return result;
}

void usage(char *program_name)
{
  printf("Usage options:\n");
  printf(
"--help       [Optional] Print this help message\n"
"--infile     [Required] Path to input file from where to read the data.\n"
"--outfile               Path to the output file. Required if --logcat_tag is not set.\n"
"--logcat_tag            Put the output into Logcat with the given tag.\n"
"--open_sleep [Required] Number of milliseconds to sleep before retry if\n"
"                        opening the input file fails.\n"
"--open_retry [Required] Number times to retry opening the input file.\n"
"--read_timeout [Required] Number of milliseconds for no new data to show\n"
"                          up in the input file before exiting the\n"
"                          application. If zero wait until the read returns\n"
"                          zero bytes.\n"
"\n"
"Example Usage:\n"
"\n"
"Read from input file and output to logcat. Retry open 50 times with 10 millisecond delay between each open. Close the file and exit after 30 seconds of no data\n"
    );

  printf ("%s --infile /dev/mhi_pipe_20 --logcat_tag MDM_boot_log --open_sleep 10 --open_retry 50 --read_timeout 30000\n\n", program_name);
  printf ("Read from input, output to file. Fail if file does not exist, exit the application when the read call returns 0 bytes (similar to cat)\n");
  printf ("%s --infile /dev/mhi_pipe_20 --outfile MDM_boot.log --open_sleep 0 --open_retry 0 --read_timeout 0\n\n", program_name);
    return;
}

int main (int argc, char *argv[])
{
  int ret;
  int option;
  struct qcat_args *qcat_args;
  const char *const short_options = "hi:o:s:r:t:l"; /* cmd line short options */
  const struct option long_options[] = {            /* cmd line long options */
    { "help",           no_argument,       NULL, 'h' },
    { "infile",         required_argument, NULL, 'i' },
    { "outfile",        required_argument, NULL, 'o' },
    { "open_sleep",     required_argument, NULL, 's' },
    { "open_retry",     required_argument, NULL, 'r' },
    { "read_timeout",   required_argument, NULL, 't' },
    { "logcat_tag",     required_argument, NULL, 'l' },
    { NULL,             0,                 NULL,  0  }
  };

  ret = 0;
  PLOGD ("%s CALLED LIKE THIS: '", argv[0]);
  for (option=0; option < argc; option++)
  {
    PLOGD ("%s \n", argv[option]);
  }
  PLOGD ("\n");

  if (argc == 1) {
    PLOGE ("not enough arguments\n");
    usage(argv[0]);
    return -1;
  }

  memset (&qcat_args_g, 0, sizeof (qcat_args_g));
  qcat_args = &qcat_args_g;

  /* parse command-line options */
  do
  {
    option = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (option)
    {
    case -1:                /* no more option arguments */
      break;

    case 'h':               /* -h or --help */
      usage(argv[0]);
      return 0;

    case 'i':
      qcat_args->infile = optarg;
      break;

    case 'o':
      qcat_args->outfile = optarg;
      printf ("outfile %s\n", optarg);
      break;

    case 's':
      qcat_args->open_sleep_msec = atoi(optarg);
      qcat_args->open_sleep_set = 1;
      break;

    case 'r':
      qcat_args->open_retry_count = atoi(optarg);
      qcat_args->open_retry_set = 1;
      break;

    case 't':
      errno = 0;
      qcat_args->read_timeout = strtol(optarg, NULL, 10);
      if (errno != 0)
      {
        PLOGE ("Invalid Read Timeout value\n");
      }
      else
      {
        qcat_args->read_timeout_set = 1;
      }
      break;

    case 'l':
      qcat_args->logcat_tag = optarg;
      break;

    default:                /* unknown option. */
      PLOGE("unrecognized option: '%c'\n", option);
      usage (argv[0]);
      return -1;
    }
  } while (option != -1);

  PLOGD ("infile %s\n", qcat_args->infile);
  PLOGD ("outfile %s\n", qcat_args->outfile);
  PLOGD ("open sleep microseconds %d\n", qcat_args->open_sleep_msec);
  PLOGD ("open retry count %d\n", qcat_args->open_retry_count);
  PLOGD ("read timeout %ld\n", qcat_args->read_timeout);
  PLOGD ("Logcat Tag: %s\n", qcat_args->logcat_tag);

  ret = qcat_unbuffered_main (qcat_args);

  PLOGD ("Finished running\n");

  return ret;
}


