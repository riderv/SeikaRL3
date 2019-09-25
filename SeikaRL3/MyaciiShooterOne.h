#pragma once


 #define AssertSDL(cond, msg)\
	if(!cond) { __debugbreak();ShowSDLError(msg); }

using std::string;
using std::vector;

struct Item;

struct Player
{
	int hp, ar, xp, str;
	string name;
	int x, y;
	int attack_start;
	vector<int> under_feet; // item index
	vector<Item> inv;  // item index
};



struct Enemy
{
	int hp, ar, xp, str;
	string name;
	int x, y;
	void(*state)(Enemy*);
	int fight_start;
	SDL_Point walkto;
	int walkstart;
	char Char;
	vector<Item> inv;
};


struct Item
{
	string name;
	int x,y;
};

struct Map
{
	vector<string> level;
	vector<Enemy> enemies;
	vector<Item> items;
};


std::string const& AnsiToUTF8(const std::string& codepage_str);
void draw_text(string msg, int y);
void ShowSDLError(const char* m1);
const char* IntToCStr(int val);
std::string IntToStr(int val);

char map_get( Map *m, int x, int y );

void main_menu_draw();
void main_menu_input();

void game_draw();
void game_input();
void mainloop();
void game_over_state();
void game_over_input();
void game_over_state();
void game_over_input();

void load_level(string file, Map* map);

void enemy_idle( Enemy *e );
void enemy_fight( Enemy *e);
void enemy_setup( Enemy *e );
void enemy_punch( Enemy *e, Player *p );
void enemy_dead( Enemy *e );
void enemy_action();
void enemy_figth_begin(Enemy *e, Player *p);
bool is_player_nearby( Enemy *e, Player *p);
void enemy_walk_begin( Enemy *e );
void enemy_walk( Enemy *e );
void enemy_scan( Enemy *e );
char map_get( Map *m, int x, int y )

bool is_enemy_nearby (int x, int y, Enemy **e);
void attack_enemy(Player*p, Enemy *e);
void try_move( Player *p, int x, int y );
void try_get( Player *p );
void try_open_door( Player *p, int x, int y );
void close_door(int x, int y);
void show_inv( Player *p );

typedef void(*State_draw_t)();
State_draw_t state_draw = main_menu_draw;

typedef void(*State_input_t)();
State_input_t state_input = main_menu_input;
