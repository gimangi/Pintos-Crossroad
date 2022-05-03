
#include <stdio.h>

#include "threads/thread.h"
#include "threads/synch.h"
#include "projects/crossroads/vehicle.h"
#include "projects/crossroads/map.h"
#include "projects/crossroads/ats.h"
#include "projects/crossroads/intersection.h"
#include "projects/crossroads/unistep.h"

/* path. A:0 B:1 C:2 D:3 */
const struct position vehicle_path[4][4][10] = {
	/* from A */ {
		/* to A */
		{{-1,-1},},
		/* to B */
		{{4,0},{4,1},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1}}
	},
	/* from B */ {
		/* to A */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1}},
		/* to B */
		{{-1,-1},},
		/* to C */
		{{6,4},{5,4},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from C */ {
		/* to A */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1}},
		/* to C */
		{{-1,-1},},
		/* to D */
		{{2,6},{2,5},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from D */ {
		/* to A */
		{{0,2},{1,2},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1}},
		/* to D */
		{{-1,-1},}
	}
};

static int is_position_outside(struct position pos)
{
	return (pos.row == -1 || pos.col == -1);
}

/* return 0:termination, 1:success, -1:fail */
static int try_move(int start, int dest, int step, struct vehicle_info *vi)
{
	struct position pos_cur, pos_next;

	pos_next = vehicle_path[start][dest][step];
	pos_cur = vi->position;

	vi->debug = 1;

	if (vi->state == VEHICLE_STATUS_RUNNING) {
		/* check termination */
		if (is_position_outside(pos_next)) {
			/* actual move */
			vi->position.row = vi->position.col = -1;
			/* release previous */
			lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
			return 0;
		}
	}


	vi->debug = 2;

	/* check enter the intersection area */
	if (!is_intersect(pos_cur) && is_intersect(pos_next)) {
		wait_intersect(vi);
	}
	/* exit intersection */
	else if (is_intersect(pos_cur) && !is_intersect(pos_next)) {
		signal_intersect(vi);
	}

	vi->debug=10;

	
	if (vi->map_locks[pos_next.row][pos_next.col].semaphore.value < 1)
		sema_down(&vi->stop);
	/* lock next position */
	lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
	if (vi->state == VEHICLE_STATUS_READY) {
		/* start this vehicle */
		vi->state = VEHICLE_STATUS_RUNNING;
	} else {
		/* release current position */
		lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
	}
	/* update position */
	vi->position = pos_next;

	return 1;
}

void init_on_mainthread(int thread_cnt){
	/* Called once before spawning threads */
	int i;

	/* vehicle list for unistep */
	vi_cnt = 0;
	vi_list = malloc(sizeof(struct vehicle_info*) * thread_cnt);

	/* unistep check thread */
	thread_create("unitstep", PRI_UNISTEP, check_unitstep, NULL);
	sema_init(&sem_released, 1);

	/* vehicles enter the intersection */
	entered = NULL;
	allowed_list = malloc(sizeof(struct vehicle_info*) * 4);
	for (i=0; i<4; i++) {
		allowed_list[i] = NULL;
	}

	/* intersection semaphores */
	sema_init(&sem_first, 1);
	sema_init(&sem_opp, 1);
	sema_init(&sem_left, 1);
	sema_init(&sem_right, 1);
}

void vehicle_loop(void *_vi)
{
	int res;
	int start, dest, step;

	struct vehicle_info *vi = _vi;

	start = vi->start - 'A';
	dest = vi->dest - 'A';

	vi->position.row = vi->position.col = -1;
	vi->state = VEHICLE_STATUS_READY;
	vi->allow_dir = UNDEFINED;

	/* semaphore for moving in a unitstep */
	sema_init(&vi->stop, 0);
	/* save to vi_list */
	vi_list[vi_cnt++] = vi;

	step = 0;
	while (1) {

		/* Do not move until the unitstep has progressed */
		//sema_down(&(vi->moved));
		vi->debug = 0;
		
		/* vehicle main code */
		res = try_move(start, dest, step, vi);
		

		if (res == 1) {
			step++;
		}

		/* termination condition. */ 
		if (res == 0) {
			break;
		}

		sema_down(&vi->stop);

	}	

	/* status transition must happen before sema_up */
	vi->state = VEHICLE_STATUS_FINISHED;
}
