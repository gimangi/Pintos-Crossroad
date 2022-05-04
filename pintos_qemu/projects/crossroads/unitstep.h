
#include "projects/crossroads/vehicle.h"
#include "threads/synch.h"
#include "threads/thread.h"

#define PRI_UNISTEP 20

struct semaphore sem_unitstep;
struct vehicle_info **vi_list;
int vi_cnt;

void init_unitstep();
void check_unitstep();
int stopped(struct semaphore *sem);
void step_point(struct vehicle_info *vi);