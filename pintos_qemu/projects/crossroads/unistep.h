
#include "projects/crossroads/vehicle.h"
#include "threads/synch.h"

#define PRI_UNISTEP 20

struct vehicle_info **vi_list;
int vi_cnt;

struct semaphore sem_unitstep;

void check_unistep();