/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timesys.h>
#include <pthread.h>
#include <rtems/timerdrv.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_pthread_create(void);
void benchmark_pthread_setschedparam(void);
void *test_thread(void *argument);
void set_thread_priority(pthread_t id, int priority);

void set_thread_priority(pthread_t id, int priority)
{
  int status;
  int policy;
  struct sched_param param;
  status = pthread_getschedparam(id, &policy, &param);
  rtems_test_assert( status == 0 );
  param.sched_priority = priority;
  benchmark_timer_initialize();
  status = pthread_setschedparam(id, policy, &param);
  rtems_test_assert( status == 0 );
}

void benchmark_pthread_setschedparam(void)
{
  int status;
  int priority;
  pthread_t thread_ID;

  status = pthread_create(&thread_ID, NULL, test_thread, NULL);
  rtems_test_assert( status == 0 );
  priority = 4;
  set_thread_priority(thread_ID, priority);
}

void *test_thread(
  void *argument
)
{
  long end_time;

  // XXX we are including thread start time. Need to do something
  // XXX that let's it run and voluntarily return. Line
  //      + test_thread created at 6
  //      + POSIX_Init lowers itself to : and switches to test_thread
  //      + test_thread runs and lowers itself to 4 and switches to POSIX_Init
  //      + start timer in POSIX_Init
  //      + POSIX_Init lowers itself to 5 and switches to test_thread

  // XXX moved
  end_time = benchmark_timer_read();

  put_time(
    "pthread_setschedparam - lower own priority",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST PSXTMTHREAD05 ***" );
  rtems_test_exit(0);
  //Empty thread used in pthread_create().
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{

  puts( "\n\n*** POSIX TIME TEST PSXTMTHREAD05 ***" );
  benchmark_pthread_setschedparam();

  rtems_test_assert( 1 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
