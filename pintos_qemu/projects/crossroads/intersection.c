
#include "projects/crossroads/intersection.h"

/* based on A, B, C, D */
const char opposite[4] = {'C', 'D', 'A', 'B'};
const char left[4] = {'D', 'A', 'B', 'C'};
const char right[4] = {'B', 'C', 'D', 'A'};

static char get_opposite(char dir) {
    return opposite[dir-'A'];
}

static char get_left(char dir) {
    return left[dir-'A'];
}

static char get_right(char dir) {
    return right[dir-'A'];
}

static int is_straight(struct vehicle_info *vi) {
    return get_opposite(vi->start) == vi->dest;
}

static int is_turn_left(struct vehicle_info *vi) {
    return get_left(vi->start) == vi->dest;
}

static int is_turn_right(struct vehicle_info *vi) {
    return get_right(vi->start) == vi->dest;
}

static int is_sem_all_ready() {
    return (sem_first.value == 1 && sem_left.value == 1 && sem_right.value == 1 && sem_opp.value == 1);
}

int is_intersect(struct position pos) {
    return (pos.col >= 2 && pos.col <= 4 && pos.row >= 2 && pos.row <= 4);
}

void wait_intersect(struct vehicle_info *vi) {

    /* vehicle going straight have priority */
    /*
    if (is_straight(vi)) {
        thread_set_priority(thread_get_priority() + 1);
    }
    */
vi->debug = 3;
    
    /* When a vehicle that has already entered exists, additional entry is possible */
    if ( entered != NULL && allow_enter(vi) ) {
vi->debug=5;
        // vi is to the left of the enterted
        if (sem_left.value == 1 && vi->start == get_left(entered->start)) {
            vi->debug=6;
            sema_down(&sem_left);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = LEFT;
            return;
        }
        // vi is to the right of the entered
        else if (sem_right.value == 1 && vi->start == get_right(entered->start)) {
            vi->debug=7;
            sema_down(&sem_right);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = RIGHT;
            return;
        }
        // vi is to the opposite of the entered
        else if (sem_opp.value == 1 && vi->start == get_opposite(entered->start)) {
            vi->debug=8;
            sema_down(&sem_opp);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = OPPOITE;
            return;
        }
    }
vi->debug=9;

    // blocked by intersection
    while (sem_first.value < 1) {
        step_point(vi);
    }

    sema_down(&sem_first);
    while (!(sem_left.value == 1 && sem_right.value == 1 && sem_opp.value == 1)) {
        step_point(vi);
    }
    allowed_list[vi->start-'A'] = vi;
    entered = vi;
    vi->allow_dir = FIRST;


    /* vehicle is in intersection */
    //if (is_sem_all_ready) {
        /* actual moved */
    /*    sema_down(&vi->moved);
        sema_down(&sem_first);
        allowed_list[vi->start-'A'] = vi;
        entered = vi;
        vi->allow_dir = FIRST;
        return;
    } 최근꺼*/
    


    /*
    // will blocked
    if (sem_first.value < 1)
        sema_down(&vi->moved);

    /* wait for enter the intersection */
    //sema_down(&sem_first);

}

void signal_intersect(struct vehicle_info *vi) {
    
    switch(vi->allow_dir) {
        case FIRST:
            sema_up(&sem_first);
            break;
        case OPPOITE:
            sema_up(&sem_opp);
            break;
        case LEFT:
            sema_up(&sem_left);
            break;
        case RIGHT:
            sema_up(&sem_right);
    }
    allowed_list[vi->start-'A'] = NULL;
    vi->allow_dir = UNDEFINED;

    if (is_sem_all_ready)
        entered = NULL;
}

int allow_enter(struct vehicle_info *target) {
    int i;
    for (i=0; i<4; i++) {
        if (allowed_list[i] != NULL && !allow_direction(allowed_list[i], target))
            return 0;
    }
    return 1;
}

/* when standard vehicle is entered in intersection, can the target enter the intersection? */
int allow_direction(struct vehicle_info *std, struct vehicle_info *target) {
    // when standard is straight
    if (is_straight(std)) {
        // target is opposite -> can (straight or turn right)
        if (get_opposite(std->start) == target->start) {
            if (is_straight(target) || is_turn_right(target))
                return 1;
        }
        // target is left -> can turn right
        if (get_left(std->start) == target->start) {
            if (is_turn_right(target))
                return 1;
        }
    }
    // when standard is turn left
    else if (is_turn_left(std)) {
        // only allow : target is left & target turn right
        if (get_left(std->start) == target->start) {
            if (is_turn_right(target))
                return 1;
        }
    }
    // when standard is turn right
    else if (is_turn_right(std)) {
        // target is opposite -> can (straight or turn right)
        if (get_opposite(std->start) == target->start) {
            if (is_straight(target) || is_turn_right(target))
                return 1;
        }
        // target is left -> can turn right
        if (get_left(std->start) == target->start) {
            if (is_turn_right(target))
                return 1;
        }
        // target is right -> can (straight or turn right)
        if (get_right(std->start) == target->start) {
                return 1;
        }
    
    }

    // now allowed
    return 0;
}

