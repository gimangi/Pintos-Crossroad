
#include "projects/crossroads/intersection.h"

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

int is_intersect(struct position pos) {
    return (pos.col >= 2 && pos.col <= 4 && pos.row >= 2 && pos.row <= 4);
}

void wait_intersect(struct vehicle_info *vi) {

    /* vehicle is in intersection */
    if (sem_first.value == 1) {
        sema_down(&sem_first);
        allowed_list[vi->start-'A'] = vi;
        entered = vi;
        return;
    }

// 동시에 들어가도 맨 뒤에 들어온 애까지 다 나가기 전에는 sema up 하면 안되는데...
    if ( entered != NULL && allow_direction(entered, vi) ) {

        // vi is to the left of the enterted
        if (vi->start == get_left(entered->start) && allow_enter(vi)) {
            sema_down(&sem_left);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = LEFT;
        }
        // vi is to the right of the entered
        else if (vi->start == get_right(entered->start) && allow_enter(vi)) {
            sema_down(&sem_right);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = RIGHT;
        }
        // vi is to the opposite of the entered
        else if (vi->start == get_opposite(entered->start) && allow_enter(vi)) {
            sema_down(&sem_opp);
            allowed_list[vi->start-'A'] = vi;
            vi->allow_dir = OPPOITE;
        }

    } else {
        sema_down(&sem_first);
    }

}

void signal_intersect(struct vehicle_info *vi) {

    switch(vi->allow_dir) {
        case OPPOITE:
            sema_up(&sem_opp);
            break;
        case LEFT:
            sema_up(&sem_left);
            break;
        case RIGHT:
            sema_up(&sem_right);
            break;
        default:
            sema_up(&sem_first);
    }
    allowed_list[vi->start-'A'] = NULL;
    entered = NULL;
}

int allow_enter(struct vehicle_info *target) {
    int i;
    for (i=0; i<4; i++) {
        if (!allow_direction(allowed_list[i], target))
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
        if (get_opposite(std->start) == target->start) {
            if (is_straight(target) || is_turn_right(target))
                return 1;
        }
    
    }

    // now allowed
    return 0;
}

