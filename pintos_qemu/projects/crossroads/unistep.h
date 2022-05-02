
#include "projects/crossroads/vehicle.h"
#include "threads/synch.h"

#define PRI_UNISTEP 50

struct vehicle_info **vi_list;
int vi_cnt;

void check_unistep();