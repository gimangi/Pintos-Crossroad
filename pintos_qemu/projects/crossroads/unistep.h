
#include "projects/crossroads/vehicle.h"
#include "threads/synch.h"

#define PRI_UNISTEP 40

struct vehicle_info **vi_list;
int vi_cnt;

void check_unistep();