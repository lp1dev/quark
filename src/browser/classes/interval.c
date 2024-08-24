#include <stdlib.h>
#include "interval.h"

Interval *Interval_create(int id, int interval, int timeout, int start_time) {
    Interval *i;

    i = malloc(sizeof(Interval));
    i->id = id;
    i->timeout = timeout;
    i->interval = interval;
    i->start_time = start_time;
    return i;
}