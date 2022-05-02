#ifndef __PROJECTS_PROJECT1_MAPDATA_H__
#define __PROJECTS_PROJECT1_MAPDATA_H__

#include <stdio.h>
#include "projects/crossroads/position.h"
#include "threads/synch.h"
// debug
#include "threads/thread.h"

extern int crossroads_step;
struct semaphore sem_released;

void map_draw(void);
void map_draw_vehicle(char id, int row, int col);
void map_draw_reset(void);

//debug
struct thread **ths;


#endif /* __PROJECTS_PROJECT1_MAPDATA_H__ */