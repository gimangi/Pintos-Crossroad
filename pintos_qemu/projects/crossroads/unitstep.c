
#include <stdio.h>
#include <list.h>
#include "projects/crossroads/unitstep.h"
#include "projects/crossroads/ats.h"
#include "projects/crossroads/crossroads.h"
#include "projects/crossroads/map.h"

static void release_move() {
    int i;

    for (i=0; i<vi_cnt; i++) {
        if (vi_list[i] != NULL)
            sema_up(&vi_list[i]->stop);
    }
}

static int stopped(struct semaphore *sem) {
    return !list_empty(&sem->waiters);
}

void init_unitstep() {
    /* init unitstep thread by main */
    sema_init(&sem_unitstep, 0);
	thread_create("unitstep", PRI_UNISTEP, check_unitstep, NULL);
}

void check_unitstep() {
    int i;
    char flag;

    while (1) {
        flag = 1;

        for (i=0; i<vi_cnt; i++) {
            // Vehicle that have not yet moved 
            if (vi_list[i] != NULL && vi_list[i]->state != VEHICLE_STATUS_FINISHED && !stopped(&vi_list[i]->stop)) {
                flag = 0;
            }
        }
        // increase unitstep
        if (flag) {
            release_move();

            unitstep_changed();
            crossroads_step++;


            if (released) { 
                break;
            }
            
        }

        // prevent busy wait 
        sema_down(&sem_unitstep);

    }
}

void step_point(struct vehicle_info *vi) {
    /* may run by vehicle thread */
    ASSERT (vi != NULL);

    sema_up(&sem_unitstep);
    sema_down(&vi->stop);
}