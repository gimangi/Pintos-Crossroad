
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

int stopped(struct semaphore *sem) {
    return !list_empty(&sem->waiters);
}

void check_unitstep() {
    int i;
    char flag;

    while (1) {
        // prevent busy wait 
        sema_down(&sem_unitstep);

        flag = 1;

        for (i=0; i<vi_cnt; i++) {
            // Vehicle that have not yet moved 
            if (vi_list[i] != NULL && vi_list[i]->state == VEHICLE_STATUS_RUNNING && !stopped(&vi_list[i]->stop)) {
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

    }
}

void step_point(struct vehicle_info *vi) {
    ASSERT (vi != NULL);

    sema_up(&sem_unitstep);
    sema_down(&vi->stop);
}