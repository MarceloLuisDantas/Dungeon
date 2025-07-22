#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

size_t height = 100;
size_t width  = 200;
size_t MAX_ROOM_H = 10;
size_t MIN_ROOM_H = 5;
size_t MAX_ROOM_W = 20;
size_t MIN_ROOM_W = 10;
size_t PADDING = 4; 

typedef struct Point {
    size_t x;
    size_t y;
} Point;

typedef struct Map {
    char **map;
    size_t width;
    size_t height;
    size_t *centers; // [x1, y1, x2, y2, ... xn, yn]
    size_t rooms;
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
            m->map[i][j] = ' ';
        }
    }

    m->width = x;
    m->height = y;
    m->rooms = 0;
    return m;
}

size_t random_range(size_t min, size_t max) {
    return rand() % (max - min + 1) + min;
}

Room generate_random_room() {
    size_t start_x = random_range(PADDING, (width - PADDING));
    size_t start_y = random_range(PADDING, (height - PADDING));
    Room room = {
        .x1 = start_x,
        .y1 = start_y,
        .x2 = start_x + random_range(MIN_ROOM_W, MAX_ROOM_W),
        .y2 = start_y + random_range(MIN_ROOM_H, MAX_ROOM_H)
    };
    return room;
}

bool check_if_point_in_map(size_t x, size_t y) {
    if (x < 3 || x >= (width - 3))
        return false;
    if (y < 3 || y >= (height - 3))
        return false;
    return true;
}

bool check_if_room_fits(Map *map, Room *room) {
    if (!check_if_point_in_map(room->x1, room->y1)) return false;
    if (!check_if_point_in_map(room->x2, room->y2)) return false;
    
    // Detects if the room is at least 3 tiles from another
    for (size_t x = room->x1 - 3; x < room->x2 + 3; x++)
        if (map->map[x][room->y1 - 3] != ' ' || map->map[x][room->y2 + 3] != ' ')
            return false;
    
    for (size_t y = room->y1 - 3; y < room->y2 + 3; y++)
        if (map->map[room->x1 - 3][y] != ' ' || map->map[room->x2 + 3][y] != ' ')
            return false;

    // Detects if the room is covering another smaller room
    for (size_t x = room->x1 + 3; x < room->x2 - 3; x++)
        if (map->map[x][room->y1 + 3] != ' ' || map->map[x][room->y2 - 3] != ' ')
            return false;
    
    for (size_t y = room->y1 + 3; y < room->y2 - 3; y++)
        if (map->map[room->x1 + 3][y] != ' ' || map->map[room->x2 - 3][y] != ' ')
            return false;

    return true;
}

bool create_room(Map *map, Room *room) {
    // Walls
    for (size_t x = room->x1; x <= room->x2; x++) {
        map->map[x][room->y1] = '#';
        map->map[x][room->y2] = '#';
    }

    // Walls
    for (size_t y = room->y1 + 1; y <= room->y2 - 1; y++) {
        map->map[room->x1][y] = '#';
        map->map[room->x2][y] = '#';
    }

    // Floor
    for (size_t x = room->x1 + 1; x <= room->x2 - 1; x++) 
        for (size_t y = room->y1 + 1; y <= room->y2 - 1; y++) 
            map->map[x][y] = '.';

    map->centers[map->rooms] = room->x2 - ((room->x2 - room->x1) / 2);
    map->centers[map->rooms + 1] = room->y2 - ((room->y2 - room->y1) / 2);
    map->rooms += 2;
    
    return true;
}

void draw_path(size_t x1, size_t y1, size_t x2, size_t y2) {
    size_t temp;
    if (x1 > x2) {
        for (; x2 != x1; x1--) 
            mvprintw(y1, x1, "O");
    } else {
        for (; x1 != x2; x1++)
            mvprintw(y1, x1, "O");
    }

    if (y1 > y2) {
        for (; y2 != y1; y1--) 
            mvprintw(y1, x1, "O");
    } else {
        for (; y1 != y2; y1++) 
            mvprintw(y1, x1, "O");
    }
}

size_t distance(size_t x1, size_t x2, size_t y1, size_t y2) {
    size_t dx = abs(x2 - x1);
    size_t dy = abs(y2 - y1);
    return sqrt(dx*dy + dy*dy);
}

Point find_closer_center(Map *map, size_t i_center) {
    size_t x1 = map->centers[i_center];
    size_t y1 = map->centers[i_center + 1];
    
    size_t closest_x = 0;
    size_t closest_y = 0;
    size_t last_distance = -1;
    for (size_t p = 0; p < map->rooms - 2; p += 2) {
        size_t x2 = map->centers[p];
        size_t y2 = map->centers[p + 1];
        if (x2 != x1 && y2 != y1) {
            size_t d = distance(x1, y1, x2, y2);
            if (last_distance == -1) {
                last_distance = d;
            } else {
                if (last_distance > d) {
                    last_distance = d;
                    closest_x = x2;
                    closest_y = y2;
                }
            }
        }
    }

    Point p = {
        .x = closest_x, 
        .y = closest_y
    };

    return p;
}

int main() {
    srand(time(NULL));
    initscr();
    nodelay(stdscr, TRUE);
    noecho();

    getmaxyx(stdscr, height, width);
    height -= 1;
    width  -= 1;

    MAX_ROOM_H = (25.0 / 100.0) * height;
    MAX_ROOM_W = (20.0 / 100.0) * width;
    MIN_ROOM_W = 20;

    Map *map = new_map(width, height);

    Room room;
    size_t trys = 0;
    size_t rooms_to_create = 30;
    map->centers = malloc(sizeof(size_t) * rooms_to_create * 2);
    while (rooms_to_create != 0) {
        room = generate_random_room();
        if (check_if_room_fits(map, &room)) {
            create_room(map, &room);
            
            for (size_t x = 0; x < width; x++)
                for (size_t y = 0; y < height; y++)
                    mvprintw(y + 1, x + 1, "%c", map->map[x][y]);

            refresh();
            while (true) {
            int ch = getch();
                if (ch == 'q')
                    break;
            }
            rooms_to_create -= 1;
        } 

        trys += 1;
        if (trys >= 500)
            break;
    }

    for (size_t i = 0; i < map->rooms; i += 2)
        mvprintw(map->centers[i + 1], map->centers[i], "c");
        
    for (size_t i = 0; i < map->rooms - 2; i += 2) {
        Point next = find_closer_center(map, i);
        draw_path(map->centers[i], map->centers[i + 1], next.x, next.y);
        refresh();
        while (true) {
            int ch = getch();
            if (ch == 'q')
                break;
        }
    }
    
    
    endwin();
    return 0;
}
