#include "projects/crossroads/unistep.h"
#include "projects/crossroads/ats.h"
#include "projects/crossroads/crossroads.h"

static void release_move() {
    int i;

    for (i=0; i<vi_cnt; i++) {
        sema_up(&(vi_list[i]->moved));
    }
}

void check_unistep() {
    int i;

    for (i=0; i<vi_cnt; i++) {
        // Vehicle that have not yet moved 
        if ((vi_list[i]->state == VEHICLE_STATUS_READY &&  vi_list[i]->moved.value == 1) || vi_list[i]->state == VEHICLE_STATUS_RUNNING)
            return;
    }

    release_move();

    unitstep_changed();
    crossroads_step++;
}