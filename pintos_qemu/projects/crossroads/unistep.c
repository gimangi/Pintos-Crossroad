#include "projects/crossroads/unistep.h"
#include "projects/crossroads/ats.h"
#include "projects/crossroads/crossroads.h"

static void release_move() {
    int i;

    for (i=0; i<vi_cnt; i++) {
        if (vi_list[i] != NULL)
            sema_up(&(vi_list[i]->moved));
    }
}

static int is_finished() {
    int i;
    int ret = vi_cnt;
    for (i=0; i<vi_cnt; i++) {
        if (vi_list[i] == NULL || vi_list[i]->state == VEHICLE_STATUS_FINISHED)
            ret--;
    }
    return ret;
}

void check_unistep() {
    int i;
    char flag;

    while (1) {

        if (is_finished())
            break;

        flag = 1;

        for (i=0; i<vi_cnt; i++) {
            // Vehicle that have not yet moved 
            if (vi_list[i] != NULL && vi_list[i]->state != VEHICLE_STATUS_FINISHED && vi_list[i]->moved.value == 1) {
                print(vi_list[i]->id);
                flag = 0;
            }
        }
        // increase unistep
        if (flag) {
            release_move();

            unitstep_changed();
            crossroads_step++;
        }

    }
}