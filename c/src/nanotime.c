#include <time.h>
#include <string.h>
#include <stdio.h>
#include "nanotime.h"

#define kT_ns_in_s  (uint64_t)1000000000
#define kT_ns_in_us (uint64_t)1000



uint64_t nanotime_sec(const struct nanotime *nt) {
  return nt->ns / kT_ns_in_s;
}

uint64_t nanotime_usec(const struct nanotime *nt) {
  return nt->ns / kT_ns_in_us;
}

// to follow the c++ api.
uint64_t nanotime_nsec(const struct nanotime *nt) {
  return nt->ns;
}




struct nanotime nanotime_from_sec(const uint64_t sec) {
  struct nanotime nt;
  nt.ns = sec * kT_ns_in_s;
  return nt;
}

struct nanotime nanotime_from_sec_f(const long double sec) {
  struct nanotime nt;
  nt.ns = sec * kT_ns_in_s;
  return nt;
}

struct nanotime nanotime_from_usec(const uint64_t usec) {
  struct nanotime nt;
  nt.ns = usec * kT_ns_in_us;
  return nt;
}

struct nanotime nanotime_from_nsec(const uint64_t nsec) {
  struct nanotime nt;
  nt.ns = nsec;
  return nt;
}




// to follow the c++ api.
struct timeval nanotime_timeval(const struct nanotime *nt) {
  struct timeval tv;
  tv.tv_sec = nanotime_sec(nt);
  tv.tv_usec = nanotime_usec(nt) % kT_ns_in_us;
  return tv;
}

struct timespec nanotime_timespec(const struct nanotime *nt) {
  struct timespec ts;
  ts.tv_sec = nanotime_sec(nt);
  ts.tv_nsec = nt->ns % kT_ns_in_s;
  return ts;
}

struct nanotime nanotime_from_timeval(const struct timeval *tv) {
  struct nanotime nt;
  nt.ns = (tv->tv_sec * kT_ns_in_s) + (tv->tv_usec * kT_ns_in_us);
  return nt;
}

struct nanotime nanotime_from_timespec(const struct timespec *ts) {
  struct nanotime nt;
  nt.ns = (ts->tv_sec * kT_ns_in_s) + (ts->tv_nsec);
  return nt;
}




struct nanotime nanotime_now() {
#ifndef __LINUX__
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return nanotime_from_timeval(&tv);
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return nanotime_from_timespec(&ts);
#endif
}




/* fill buffer `buf` with up to `len` chars of the iso representation of `nt` */
/* returns how many chars were printed into the buffer.                       */
/* may be less than `len` for cases where nanoseconds couldn't fit            */
size_t nanotime_iso(const struct nanotime *nt, char *buf, const size_t len) {
  memset(buf, '\0', len);

  time_t secs = (time_t)nanotime_sec(nt);
  struct tm tm_;
  gmtime_r(&secs, &tm_);
  size_t offset = strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm_);

  if (offset + 7 <= len) {
    sprintf(buf + offset, ".%llu", (nt->ns % kT_ns_in_s) / kT_ns_in_us);
    offset += 7;
  }
  return offset;
}


#undef kT_ns_in_s
#undef kT_ns_in_us
