#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

const size_t MAX_ROO_H = 10;
const size_t MIN_ROO_H = 5;
const size_t MAX_ROO_W = 20;
const size_t MIN_ROO_W = 10;
      size_t height    = 100;
      size_t width     = 200;

typedef struct Map {
    char **map;
    size_t width;
    size_t height;
} Map;

typedef struct Point {
    size_t x;
    size_t y;
} Point;

typedef struct Room {
    Point *from;
    Point *to;
} Room;

Map *new_map(size_t x, size_t y) {
    if (x <= 0 || y <= 0)
        return NULL;

    Map *m = malloc(sizeof(Map));
    m->map = malloc(sizeof(char*) * x);
    for (size_t i = 0; i < x; i++) {
        m->map[i] = malloc(sizeof(char) * y);
        for (size_t j = 0; j < y; j++) {
            m->map[i][j] = '-';
        }
    }

    m->width = x;
    m->height = y;
    return m;
}

Point *new_point(size_t x, size_t y) {
    Point *p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    return p;
}

Room *new_room(Point *from, Point *to) {
    Room *room = malloc(sizeof(Room));
    room->from = new_point(from->x, from->y);
    room->to = new_point(to->x, to->y);
    return room;
}

size_t random_range(size_t min, size_t max) {
    srand(time(NULL));
    return rand() % (max - min + 1) + min;
}

bool check_if_point_in_map(Point *p) {
    if (p->x < 0 || p->x >= width)
        return false;
    if (p->y < 0 || p->y >= height) 
        return false;
    return true;
}

Room *generate_random_room() {
    size_t room_w = random_range(MIN_ROO_W, MAX_ROO_W);
    size_t room_h = random_range(MIN_ROO_H, MAX_ROO_H);
    size_t start_x = random_range(0, width);
    size_t start_y = random_range(0, height);
    Point *from = new_point(start_x, start_y);
    Point *to = new_point(start_x + room_w, start_y + room_h);
    return new_room(from, to);
}


bool check_if_room_fits(Map *map, Room *room) {
    if (!check_if_point_in_map(room->from))
        return false;

    if (!check_if_point_in_map(room->to))
        return false;
        
    for (size_t x = room->from->x; x <= room->to->x; x++)
        if (map->map[x][room->from->y] == '#' || map->map[x][room->to->y] == '#')
            return false;

    for (size_t y = room->from->y + 1; y <= room->to->y - 1; y++)
        if (map->map[room->from->x][y] == '#' || map->map[room->to->x][y] == '#')
            return false;

    return true;
}

bool create_room(Map *map, Room *room) {
    for (size_t x = room->from->x; x <= room->to->x; x++) {
        map->map[x][room->from->y] = '#';
        map->map[x][room->to->y] = '#';
    }

    for (size_t y = room->from->y + 1; y <= room->to->y - 1; y++) {
        map->map[room->from->x][y] = '#';
        map->map[room->to->x][y] = '#';
    }

    return true;
}

int main() {
    initscr();
    nodelay(stdscr, TRUE);
    noecho();

    getmaxyx(stdscr, height, width);
    height -= 1;
    width  -= 1;

    Map *map = new_map(width, height);

    size_t rooms_to_crete = 3;
    while (rooms_to_crete != 0) {
        Room *room = generate_random_room();
        if (check_if_room_fits(map, room)) {
            create_room(map, room);
            
            for (size_t x = 0; x < width; x++)
                for (size_t y = 0; y < height; y++)
                    mvprintw(y + 1, x + 1, "%c", map->map[x][y]);

            rooms_to_crete -= 1;
        } 
        free(room);
        refresh();
    }


    while (true) {
        int ch = getch();
        if (ch == 'q')
            break;
    }

    endwin();
    return 0;
}
