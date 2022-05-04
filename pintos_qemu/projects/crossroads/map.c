
#include "projects/crossroads/map.h"
// debug
#include "projects/crossroads/unitstep.h"
#include "projects/crossroads/intersection.h"

#define ANSI_NONE "\033[0m"
#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

#define ON_ANSI_BLACK "\033[40m"
#define ON_ANSI_RED "\033[41m"
#define ON_ANSI_GREEN "\033[42m"
#define ON_ANSI_YELLOW "\033[43m"
#define ON_ANSI_BLUE "\033[44m"
#define ON_ANSI_MAGENTA "\033[45m"
#define ON_ANSI_CYAN "\033[46m"
#define ON_ANSI_WHITE "\033[47m"

#define clear() printf("\033[H\033[J")
#define gotoxy(y,x) printf("\033[%d;%dH", (y), (x))


const char map_draw_default[7][7] = {
	{'X', 'X', ' ', 'X', ' ', 'X', 'X'}, 
	{'X', 'X', ' ', 'X', ' ', 'X', 'X'}, 
	{' ', ' ', ' ', '-', ' ', ' ', ' '}, 
	{'-', '-', '-', '-', '-', '-', '-'}, 
	{' ', ' ', ' ', '-', ' ', ' ', ' '}, 
	{'X', 'X', ' ', '-', ' ', 'X', 'X'}, 
	{'X', 'X', ' ', '-', ' ', 'X', 'X'}, 
};


void map_draw(void)
{
	int i, j;

	clear();

	for (i=0; i<7; i++) {
		for (j=0; j<7; j++) {
			printf("%c ", map_draw_default[i][j]);
		}
		printf("\n");
	}
	printf("unit step: %d\n", crossroads_step);

	// debug
	
	if (entered != NULL)
		printf("entered: %c\n", entered->id);
	if (allowed_list[0] != NULL)
		printf("allowed A: %c\n", allowed_list[0]->id);
		if (allowed_list[1] != NULL)
		printf("allowed B: %c\n", allowed_list[1]->id);
		if (allowed_list[2] != NULL)
		printf("allowed C: %c\n", allowed_list[2]->id);
		if (allowed_list[3] != NULL)
		printf("allowed D: %c\n", allowed_list[3]->id);
	
	printf("vi_list.moved: ");
	for (int i=0; i<vi_cnt; i++) {
		if (vi_list[i] != NULL)
			printf("[%c]-stop:%d-st:%d-alloweddir:%d-debug:%d\n", vi_list[i]->id, !stopped(&vi_list[i]->stop), vi_list[i]->state, vi_list[i]->allow_dir, vi_list[i]->debug);
	}
	printf("\n");
	printf("cur thread: %s\n", thread_current()->name);
	printf("intersection sem: first %d opp %d left %d right %d\n", sem_first.value, sem_opp.value, sem_left.value, sem_right.value);
	
	// ~debug
	gotoxy(0, 0);
}

void map_draw_vehicle(char id, int row, int col)
{
	if (row >= 0 && col >= 0) {
		gotoxy(row + 1, col * 2 + 1);
		printf("%c ", id);
		gotoxy(0, 0);
	}
}

void map_draw_reset(void)
{
	released = true;
	clear();
}