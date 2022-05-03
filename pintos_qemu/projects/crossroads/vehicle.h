#ifndef __PROJECTS_PROJECT1_VEHICLE_H__
#define __PROJECTS_PROJECT1_VEHICLE_H__

#include "projects/crossroads/position.h"
#include "threads/synch.h"

#define VEHICLE_STATUS_READY 	0
#define VEHICLE_STATUS_RUNNING	1
#define VEHICLE_STATUS_FINISHED	2

enum allow_dirs {
	UNDEFINED, FIRST, OPPOITE, LEFT, RIGHT
};

struct vehicle_info {
	char id;
	char state;
	char start;
	char dest;
	char allow_dir;
	char debug;
	struct semaphore stop;
	struct position position;
	struct lock **map_locks;
};

void vehicle_loop(void *vi);

#endif /* __PROJECTS_PROJECT1_VEHICLE_H__ */