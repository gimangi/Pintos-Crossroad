
#include "projects/crossroads/vehicle.h"
#include "threads/synch.h"

struct vehicle_info **vi_list;
int vi_cnt;

void check_unistep();