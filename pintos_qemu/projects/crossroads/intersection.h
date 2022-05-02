
#include "threads/synch.h"
#include "threads/thread.h"
#include "projects/crossroads/position.h"
#include "projects/crossroads/vehicle.h"

struct semaphore sem_first;
struct semaphore sem_opp;
struct semaphore sem_left;
struct semaphore sem_right;

// vehicle enter the intersection first
struct vehicle_info *entered;
// vehicles list enter the intersection
struct vehicle_info **allowed_list;

int is_intersect(struct position pos);
void wait_intersect(struct vehicle_info *vi);
void signal_intersect(struct vehicle_info *vi);

int allow_direction(struct vehicle_info *std, struct vehicle_info *target);


