#ifndef QUARK_INTERVAL_H_
#define QUARK_INTERVAL_H_

typedef struct Interval Interval;

struct Interval {
    int id;
    int timeout;
    int interval;
    int start_time;
};

Interval *Interval_create(int id, int timeout, int interval, int start_time);

#endif