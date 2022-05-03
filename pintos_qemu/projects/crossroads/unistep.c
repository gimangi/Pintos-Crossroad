
#include <stdio.h>
#include <list.h>
#include "projects/crossroads/unistep.h"
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

static int is_finished() {
    int i;
    int ret = vi_cnt;
    for (i=0; i<vi_cnt; i++) {
        if (vi_list[i] != NULL && vi_list[i]->state == VEHICLE_STATUS_FINISHED)
            ret--;
    }
    return (ret == 0);
}

int is_empty_waiter(struct semaphore *sem) {
    return list_empty(&sem->waiters);
}

void check_unitstep() {
    int i;
    char flag;

    while (1) {
        flag = 1;

        for (i=0; i<vi_cnt; i++) {
            // Vehicle that have not yet moved 
            if (vi_list[i] != NULL && vi_list[i]->state == VEHICLE_STATUS_RUNNING && is_empty_waiter(&vi_list[i]->stop)) {
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
