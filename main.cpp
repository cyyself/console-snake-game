#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include <queue>
using std::queue;

#define screen_height 20
#define screen_width  60
const char sugar = '$';
const char poison = '@';

#define OS_UNIX
//#define OS_WINDOWS

int dir;

#ifdef OS_UNIX
	#include <termios.h>
	void OffBufferedInput() {
		struct termios tmp;
		tcgetattr(STDIN_FILENO,&tmp);
		tmp.c_lflag &=(~ICANON & ~ECHO);
		tcsetattr(STDIN_FILENO,TCSANOW,&tmp);
	}
	int getdir() {
		while (1) {
			char c = getchar();
			switch (c) {
				case 65: return 0;
				case 68: return 1;
				case 66: return 2;
				case 67: return 3;
			}
		}
	}
#endif

#ifdef OS_WINDOWS
	#include <conio.h>
	int getdir() {
		while (1) {
			char c = getch();
			switch (c) {
				case 72: return 0;
				case 75: return 1;
				case 80: return 2;
				case 77: return 3;
			}
		}
	}
#endif


char scr[screen_height][screen_width+1];

struct Pos {
	int x,y;
	Pos(int _x,int _y) {
		x = _x;
		y = _y;
	}
	Pos() {

	}
}snake;
queue <Pos> q;

bool game_over;

void print_screen() {
	#ifdef OS_WINDOWS
		system("cls");
	#endif
	#ifdef OS_UNIX
		system("clear");
	#endif
	if (game_over) printf("score=%lu\tdir=%d\tGame_over!\n",q.size(),dir);
	else printf("score=%lu\tdir=%d\n",q.size(),dir);
	for (int i=0;i<screen_width+2;i++) printf("-");
	printf("\n");
	for (int i=0;i<screen_height;i++) printf("|%s|\n",&scr[i][0]);
	for (int i=0;i<screen_width+2;i++) printf("-");
	printf("\n");
}

void place_sugar() {
	int x = rand() % screen_height;
	int y = rand() % screen_width;
	while (scr[x][y] != ' ') {
		x = rand() % screen_height;
		y = rand() % screen_width;
	}
	scr[x][y] = sugar;
}

void place_poison() {
	int x = rand() % screen_height;
	int y = rand() % screen_width;
	while (scr[x][y] != ' ') {
		x = rand() % screen_height;
		y = rand() % screen_width;
	}
	scr[x][y] = poison;
}
void* worker(void* args) {
	while (!game_over) {
		const int dx[4] = {-1,0,1,0};
		const int dy[4] = {0,-1,0,1};
		Pos head = Pos(
			((snake.x+dx[dir])%screen_height+screen_height)%screen_height,
			((snake.y+dy[dir])%screen_width+screen_width)%screen_width
		);
		snake = head;
		q.push(head);
		if (scr[head.x][head.y] == 'o') {
			game_over = true;
			print_screen();
			break;
		}
		//S=>sugar P=>poison
		if (scr[head.x][head.y] != sugar) {
			Pos tail = q.front();
			q.pop();
			if (q.empty()) {
				game_over = true;
				print_screen();
				break;
			}
			if (scr[head.x][head.y] == poison) {
				Pos tail2 = q.front();
				scr[tail2.x][tail2.y] = ' ';
				q.pop();
				if (q.empty()) {
					game_over = true;
					print_screen();
					break;
				}
				place_poison();
			}
			scr[tail.x][tail.y] = ' ';
		}
		else {
			place_sugar();
		}
		scr[snake.x][snake.y] = 'o';
		if (q.size() >= screen_height * screen_width - 2) game_over = true;
		print_screen();
		usleep(100000);
	}
}
void init() {
	#ifdef OS_UNIX
	OffBufferedInput();
	#endif
	srand((int)time(0));
	game_over = false;
	q.push(Pos(0,0));
	snake.x = 0;
	snake.y = 0;
	memset(scr,' ',sizeof(scr));
	place_sugar();
	place_poison();
	for (int i=0;i<screen_height;i++) scr[i][screen_width] = '\0';
}
int main() {
	init();
	pthread_t t1;
	pthread_create(&t1,NULL,worker,NULL);
	while (1) dir = getdir();
	return 0;
}
