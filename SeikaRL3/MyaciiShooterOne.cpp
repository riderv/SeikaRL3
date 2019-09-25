// MyaciiShooterOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "MyaciiShooterOne.h"
// TODO LIST:
// - сделать таймаут на удар игрока
// - log file needed

using namespace std;



// global variables
bool quit;
vector<string> msg;
std::fstream Log;
Map map;
Player p1;
vector<string> screen;
SDL_Window *window;
SDL_Event window_event;
TTF_Font *gFont;

void load_level(string file, Map* map)
{
	fstream f(file);
	string tmp;
	int y = 0;
	while( getline(f, tmp) )
	{
		map->level.push_back( tmp );
		for (size_t i = 0; i < tmp.size(); ++i)
		{
			if( tmp[i] == '@' )
			{
				p1.x = i;
				p1.y = y;
				map->level[y][i] = '.';
			}else if( tmp[i] == 'i' )
			{
				map->level[y][i] = '.';
				Enemy e;
				e.hp = 25;
				e.ar = 0;
				e.name = "imp";
				e.Char = 'i';
				e.xp = 0;
				e.x = i;
				e.y = y;
				e.str = 5;
				e.state = enemy_idle;
				map->enemies.push_back(e);
			}else if( tmp[i] == '{' )
			{
				map->level[y][i] = '.';
				Item w;
				w.name = "bow";
				w.x = i;
				w.y = y;
				map->items.push_back(w);
			}
		}
		y++;
	}

}

void player_setup(Player *p)
{
	p->ar = 0;
	p->hp = 100;
	p->xp = 0;
	p->str = 7;
	p->name = "";
	p->x = 0;
	p->y = 0;
	p->attack_start = SDL_GetTicks();
}


void setup(Player *p)
{
	srand( (unsigned int)time(0) );

	int res = 0;
	int ret = 0;

	res = SDL_Init(SDL_INIT_VIDEO);//(SDL_INIT_EVERYTHING);
	AssertSDL( (res == 0) , "SDL_Init");
	window = SDL_CreateWindow("Seika Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
	AssertSDL(window, "SDL_CreateWindow");

	SDL_Surface* sdl_surface = SDL_GetWindowSurface(window);
	AssertSDL(sdl_surface, "SDL_GetWindowSurface(gWindow)");

	if( -1 == TTF_Init() )
	{
		MessageBoxA(0, "TTF_Init fail", TTF_GetError(), MB_OK|MB_ICONWARNING);
	}

	gFont = TTF_OpenFont("consola.ttf", 26);
	if(!gFont) {
			MessageBoxA(0, "TTF_OpenFont",
					SDL_GetError(), MB_OK|MB_ICONWARNING);

	}
	//Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        MessageBoxA(0, (string("SDL_image could not initialize! SDL_image Error:")+IMG_GetError()).c_str(), "IMG_Init fail", MB_ICONWARNING );
    }


	int w,h;
	SDL_GetWindowSize(window, &w, &h);


	quit = false;

	state_draw = main_menu_draw;
	state_input = main_menu_input;

	player_setup(p);

	load_level("d:/_p/LearnCPP/MyaciiShooterOne/maps/Map1.txt", &map);
}


void input()
{
	//if (_kbhit ())
	//{
	//	switch(_getch ())
	//	{
	//	case 'a':
	//		dir  = LEFT;
	//	break;
	//	case 'd':
	//		dir  = RIGHT;
	//	break;
	//	case 'w':
	//		dir  = UP;
	//	break;
	//	case 's':
	//		dir  = DOWN;
	//	break;
	//	case 'x':
	//		gameover = true;
	//		break;
	//	}
	//}
}
void logic()
{

}


void draw_text(string msg, int y)
{
	if(msg.size() == 0 )
		msg = " ";
	string s = AnsiToUTF8(msg);
	SDL_Surface *ttf_txt = TTF_RenderUTF8_Shaded(gFont, s.c_str(), {255,255,255}, {0,0,0});
	SDL_Rect dest_rect{0, y * ttf_txt->h , 0, 0};
	SDL_Surface * surface = SDL_GetWindowSurface(window);
	SDL_BlitSurface(ttf_txt, 0, surface, &dest_rect);

	SDL_FreeSurface(ttf_txt);	ttf_txt = 0;
}

void main_menu_draw()
{
	//system("cls");
	//cout << "      - Fugas -      \n";
	//cout << "                     \n";
	//cout << "  1) Start game      \n";
	//cout << "                     \n";
	//cout << "                     \n";
	//cout << " ------------------- \n";
	draw_text("      - Fugas -      ", 0);
	draw_text("  1) Start game      ", 2);
	draw_text("  2) Exit game       ", 3);
}

void main_menu_input()
{
	if (window_event.type == SDL_KEYDOWN)
	{
		switch( window_event.key.keysym.scancode )
		{
		case SDL_SCANCODE_1:
			state_draw = game_draw;
			state_input = game_input;
			break;
		case SDL_SCANCODE_2:

			break;
		}
	}
}

void game_draw()
{
	enemy_action(); // todo отдельн
	//system("cls");
	screen.clear();

	// draw map
	for(size_t i = 0; i < map.level.size() ; ++i)
	{
		screen.push_back( map.level[i] );
	}
	// draw items
	string item_you_see;
	for( size_t i = 0; i < map.items.size() ; ++i )
	{
		Item * w = &map.items[i];
		screen[w->y][w->x] = '{';
		if( p1.x == w->x && p1.y == w->y )
		{
			stringstream ss;
			ss << "Вы видите " << w->name;
			item_you_see = ss.str();
		}
	}
	// draw enemies and enemy's corps
	for( size_t i = 0; i < map.enemies.size() ; ++i)
	{
		Enemy * e = &map.enemies[i];
		if( e->state == enemy_dead )
		{
			screen[e->y][e->x] = '%';
		}else
			screen[e->y][e->x] = e->Char;
	}

	// Draw player
	screen[p1.y][p1.x] = '@';

	//draw screen buffer to console
	size_t line = 0;
	for(; line < screen.size(); ++line )
	{
		//cout << screen[i] << endl;
		draw_text( screen[line], line );
	}

	if( !item_you_see.empty() ) {
		draw_text( item_you_see, line++);
	}

	// draw messages
	if(msg.size() > 10)
	{
		msg.erase( msg.begin(), msg.begin() + msg.size() - 10 );  // trunk to last 10
	}
	for( auto &m: msg )
	{
		//cout << m << endl;
		draw_text(m, line++);
	}

	//draw stats
	//cout << "hp: " << p1.hp;
	stringstream ss;
	ss << "hp: " << p1.hp;
	draw_text( ss.str() , line++ );
}

void try_open_door( Player *p, int x, int y )
{
	if( map.level[y][x] == '+' )
	{
		map.level[y][x] = '/';
		msg.push_back("Вы открыли дверь.");
	}
}

bool is_enemy_nearby (int x, int y, Enemy **e)
{
	for( size_t i = 0; i < map.enemies.size(); i++)
	{
		Enemy *en = &map.enemies[i];
		if( en->x == x && en->y == y && en->state != enemy_dead)
		{
			*e = en;
			return true;
		}
	}
	return false;
}

void attack_enemy(Player*p, Enemy *e)
{
	int dt = SDL_GetTicks() - p->attack_start;
	if( dt < 1000 )
	{	return;
	}
	p->attack_start = SDL_GetTicks();

	e->hp -= p->str;


	stringstream ss;
	ss << "Вы ударили " << e->name << " и нанесли урон в " << p->str;
	msg.push_back( ss.str() );

	if ( e->hp <= 0 )
	{
		ss.str("");
		ss << e->name << " мёртв.";
		e->state = enemy_dead;
		msg.push_back( ss.str() );
	}

}

void try_move( Player *p, int x, int y )
{
	Enemy *e = nullptr;
	if( is_enemy_nearby(x,y, &e) )
	{
		attack_enemy(&p1, e);
	}else
	if( map.level[y][x] == '.' || map.level[y][x] == '/')
	{
		p->x = x;
		p->y = y;

	}else if( map.level[y][x] == '+' )
	{
		try_open_door( p, x, y );
	}
}



void close_door(int x, int y)
{
	if( map.level[y][x] == '/' )
	{
		map.level[y][x] = '+';
		msg.push_back("Вы закрыли дверь.");
	}
	state_input = game_input;

}

void try_close_door()
{
	if (window_event.type == SDL_KEYDOWN)
	{
		switch( window_event.key.keysym.scancode )
		{
			case SDL_SCANCODE_D:
				close_door(p1.x+1, p1.y);
				break;
			case SDL_SCANCODE_A:
				close_door(p1.x-1, p1.y);
				break;
			case SDL_SCANCODE_W:
				close_door(p1.x, p1.y-1);
				break;
			case SDL_SCANCODE_S:
				close_door(p1.x, p1.y+1);
				break;
		}
	}
}

void enemy_action()
{
	for( size_t i = 0; i < map.enemies.size(); i++ )
	{
		Enemy * e = &map.enemies[i];
		e->state(e);
	}
}

void enemy_figth_begin(Enemy *e, Player *p)
{
	e->state = enemy_fight;
	e->fight_start = SDL_GetTicks();
	enemy_punch(e, p);
}

bool is_player_nearby( Enemy *e, Player *p)
{
	if( e->x+1 == p->x && e->y == p->y
     || e->x-1 == p->x && e->y == p->y
     || e->x == p->x && e->y+1 == p->y
	 || e->x == p->x && e->y-1 == p->y)
	{
		return true;
	}
	return false;
}

void enemy_idle( Enemy *e )
{
	if( is_player_nearby(e, &p1))
	{
		enemy_figth_begin(e, &p1);
	}else{
		enemy_walk_begin( e );
	}
}

void enemy_dead( Enemy *e )
{

}

void game_over_state()
{
	//cout << " YOU DIE! GAME OVER." << endl;
	_getch();
	quit = true;
}
void game_over_input()
{

}

void enemy_punch( Enemy *e, Player *p )
{
	p->hp -= e->str;
	stringstream ss;
	ss << "Вы получили урон на " << e->str;
	msg.push_back( ss.str() );
	if( p->hp < 0 )
	{
		state_draw = game_over_state;
		state_input = game_over_input;
	}
}
void enemy_fight( Enemy *e )
{
	if( is_player_nearby(e, &p1) )
	{
		int dt = SDL_GetTicks() - e->fight_start;
		if( dt > 1000 )
		{
			enemy_punch(e, &p1);
			e->fight_start = SDL_GetTicks();
		}
	}else
	{
		e->state = enemy_idle;
	}
}

void enemy_walk_begin( Enemy *e )
{
	vector<SDL_Point> dests;
	if( map.level[e->y][e->x+1] == '.' ) {
		dests.push_back({e->x+1, e->y});
	}
	if( map.level[e->y][e->x-1] == '.' ) {
		dests.push_back({e->x-1, e->y});
	}
	if( map.level[e->y+1][e->x] == '.' ) {
		dests.push_back({e->x, e->y+1});
	}
	if( map.level[e->y-1][e->x] == '.' ) {
		dests.push_back({e->x, e->y-1});
	}
	if( dests.empty() )
		return;
	int dir = rand() % dests.size();
	assert(dir >= 0);
	assert(dir < 4);
	e->walkto = dests[dir];
	e->walkstart = SDL_GetTicks();
	e->state = enemy_walk;
}

void enemy_walk( Enemy *e )
{
	if ( is_player_nearby(e, &p1) )
	{
		e->state = enemy_idle;
		return;
	}
	int t = SDL_GetTicks();
	int dt = t - e->walkstart;
	if( dt >= 1000 ) // todo переделать скорость хотьбы монстра
	{
		e->x = e->walkto.x;
		e->y = e->walkto.y;
		e->state = enemy_idle;
	}
}

void enemy_scan( Enemy *e )
{
	SDL_Point sp { e->x-4, e->y-4 };

	//scan left
	bool found = false;
	for( int x = e->x; x > e->x-4; x--)
	{
		char v = map_get( &map, x, e->y );
		if( v == '#' || v == 0 ) {
			break;
		}
		if( x == p1.x && e->y == p1.y ) {
			found = true;
			break;
		}

	}
	if( found )
	{
		// перейти в состояние идти к игроку
	}
	for( int x = e->x; x < e->x+4; x++)
	{
		char v = map_get( &map, x, e->y );
		if( v == '#' || v == 0 ) {
			break;
		}
		if( x == p1.x && e->y == p1.y ) {
			found = true;
			break;
		}
	}
}

char map_get( Map *m, int x, int y )
{
	if( x < 0 || y < 0 || y >= m->level.size()
	|| x >= m->level[y].size() )
	{
		return 0;
	}
	return m->level[y][x];
}

void game_input()
{
	if (window_event.type == SDL_KEYDOWN)
	{
		switch( window_event.key.keysym.scancode )
		{
		case SDL_SCANCODE_D:
			try_move(&p1, p1.x+1, p1.y);
				break;
		case SDL_SCANCODE_A:
			try_move(&p1, p1.x-1, p1.y);
			break;
		case SDL_SCANCODE_W:
			try_move(&p1, p1.x, p1.y-1);
			break;
		case SDL_SCANCODE_S:
			try_move(&p1, p1.x, p1.y+1);
			break;
		case SDL_SCANCODE_C:
			msg.push_back( "Какую дверь закрыть? (wsda)" );
			state_input = try_close_door;
			break;
		case SDL_SCANCODE_G:
			try_get ( &p1 );
			break;
		case SDL_SCANCODE_I:
			show_inv ( &p1 );
			break;
		}
	}
}

void show_inv( Player *p )
{
	if( p->inv.empty() )
	{
		msg.push_back("Инвентарь пуст!");
		return;
	}
	msg.push_back("Ваш инвентарь состоит из:");
	for( size_t i = 0; i < p->inv.size(); ++i )
	{
		msg.push_back(	IntToStr(i) + ") " + p->inv[i].name );
	}
}

void try_get( Player *p )
{
	for( size_t i = 0; i < map.items.size(); ++i)
	{
		if( p->x == map.items[i].x
		&& p->y == map.items[i].y )
		{

			p->inv.push_back( map.items[i] );
			map.items.erase( map.items.begin()+i );
			msg.push_back( string("Вы подобрали ") +p->inv.rbegin()->name );
		}
	}
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	setlocale(LC_ALL, "Russian");
	setup(&p1);

	mainloop();
	return 0;
}

void mainloop()
{

	bool quit = false;
	while(!quit)
	{
		while(SDL_PollEvent(&window_event))
		{
			if(window_event.type == SDL_QUIT)
				quit = true;

			state_input();
		}
		SDL_Surface *surface = SDL_GetWindowSurface(window);
		SDL_Rect rect{0, 0, surface->w, surface->h};
		SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0, 0, 0));

		state_draw();

		SDL_UpdateWindowSurface(window);

	}
}

void ShowSDLError(const char* m1)
{
	char m[1024 * 10] = { 0 };
	sprintf_s(m, "%s: %s", m1, SDL_GetError() );
	MessageBoxA(0, m, "Error", MB_ICONERROR);
}

const char* IntToCStr(int val)
{
	const int buf_size = 128;
	typedef char buf_t[buf_size];
	const int buf_count = 1000;
	static buf_t bufs[buf_count];
	static int i = 0;
	_i64toa_s(val, bufs[i], buf_size, 10);
	const char *ret=bufs[i];
	i = ++i % buf_count;
	return ret;
}

std::string IntToStr(int val)
{
	return string(IntToCStr(val));
}

std::string const& AnsiToUTF8(const std::string& codepage_str)
{
	//std::string codepage_str;
	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(),
								codepage_str.length(), nullptr, 0);
	static std::wstring utf16_str;
	utf16_str.resize(size, '\0');

	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, codepage_str.c_str(),
					codepage_str.length(), &utf16_str[0], size);

	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
									utf16_str.length(), nullptr, 0,
									nullptr, nullptr);
	static std::string utf8_str;
	utf8_str.resize(utf8_size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
					utf16_str.length(), &utf8_str[0], utf8_size,
					nullptr, nullptr);
	return utf8_str;
}