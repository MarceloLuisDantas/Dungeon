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

typedef struct Room {
    size_t x1;
    size_t y1;
    size_t x2;
    size_t y2;
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

size_t random_range(size_t min, size_t max) {
    return rand() % (max - min + 1) + min;
}

Room generate_random_room() {
    size_t start_x = random_range(0, width);
    size_t start_y = random_range(0, height);
    Room room = {
        .x1 = start_x,
        .y1 = start_y,
        .x2 = start_x + random_range(MIN_ROO_W, MAX_ROO_W),
        .y2 = start_y + random_range(MIN_ROO_H, MAX_ROO_H)
    };
    return room;
}

bool check_if_point_in_map(size_t x, size_t y) {
    if (x < 0 || x >= width)
        return false;
    if (y < 0 || y >= height)
        return false;
    return true;
}

bool check_if_room_fits(Map *map, Room *room) {
    if (!check_if_point_in_map(room->x1, room->y1))
        return false;

    if (!check_if_point_in_map(room->x2, room->y2))
        return false;
        
    for (size_t x = room->x1; x <= room->x2; x++)
        if (map->map[x][room->y1] == '#' || map->map[x][room->y2] == '#')
            return false;

    for (size_t y = room->y1 + 1; y <= room->y2 - 1; y++)
        if (map->map[room->x1][y] == '#' || map->map[room->x2][y] == '#')
            return false;

    return true;
}

bool create_room(Map *map, Room *room) {
    for (size_t x = room->x1; x <= room->x2; x++) {
        map->map[x][room->y1] = '#';
        map->map[x][room->y2] = '#';
    }

    for (size_t y = room->y1 + 1; y <= room->y2 - 1; y++) {
        map->map[room->x1][y] = '#';
        map->map[room->x2][y] = '#';
    }

    return true;
}

int main() {
    srand(time(NULL));
    initscr();
    nodelay(stdscr, TRUE);
    noecho();

    getmaxyx(stdscr, height, width);
    height -= 1;
    width  -= 1;

    Map *map = new_map(width, height);

    Room room;
    size_t rooms_to_crete = 6;
    while (rooms_to_crete != 0) {
        room = generate_random_room();
        if (check_if_room_fits(map, &room)) {
            create_room(map, &room);
            
            for (size_t x = 0; x < width; x++)
                for (size_t y = 0; y < height; y++)
                    mvprintw(y + 1, x + 1, "%c", map->map[x][y]);

            rooms_to_crete -= 1;
        } 

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
