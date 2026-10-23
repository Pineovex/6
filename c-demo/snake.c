/*
 * 贪吃蛇 —— 第 1 周项目（C + Win32 控制台）
 *
 * 编译：gcc -g -Wall -Wextra -o snake.exe snake.c
 * 运行：snake.exe
 *
 * 操作：方向键 / WASD 转向，空格暂停，R 重开，ESC 退出
 *
 * 这个版本只用了三种东西：
 *   1. 二维数组当棋盘（严格说是用坐标点 + 环形队列存蛇身）
 *   2. SetConsoleCursorPosition 把光标挪回左上角，实现"原地重绘"
 *   3. GetAsyncKeyState 非阻塞读键盘（不用按回车）
 *
 * 先把它跑起来、玩一玩，再改：改速度、加障碍物、加穿墙、加音效、
 * 加计分板……每改一处都 git commit 一次。
 */

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

/* ---------- 常量 ---------- */

#define W 24          /* 棋盘内部宽（列） */
#define H 18          /* 棋盘内部高（行） */
#define CELLS (W * H) /* 棋盘总格子数，也就是蛇身最多占的格子数 */

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

/* ---------- 全局状态 ---------- */

typedef struct {
    int x;
    int y;
} Point;

static Point g_body[CELLS]; /* 环形队列：存蛇身每一节的坐标 */
static int g_head;          /* 队头下标（蛇头所在位置） */
static int g_len;           /* 蛇身长度 */
static int g_dir;           /* 当前方向 */
static int g_next_dir;      /* 下一个方向（按键先存这里，防止一帧内连按两次掉头） */
static Point g_food;
static int g_score;
static int g_speed_ms; /* 每帧间隔毫秒数 */
static int g_paused;
static int g_over;

/* ---------- 小工具 ---------- */

static void hide_cursor(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

static void move_cursor_home(void) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){0, 0});
}

/* 蛇身第 i 节在环形队列里的下标 */
static int body_index(int i) { return (g_head + i) % CELLS; }

static int body_contains(int x, int y) {
    int i;
    for (i = 0; i < g_len; i++) {
        Point p = g_body[body_index(i)];
        if (p.x == x && p.y == y) {
            return 1;
        }
    }
    return 0;
}

/* 在没被蛇占用的格子里随机放一个食物 */
static void place_food(void) {
    int free_cells = CELLS - g_len;
    int pick;
    int x, y;

    if (free_cells <= 0) { /* 蛇填满棋盘，直接算赢 */
        g_over = 1;
        return;
    }

    pick = rand() % free_cells;
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            if (body_contains(x, y)) {
                continue;
            }
            if (pick == 0) {
                g_food.x = x;
                g_food.y = y;
                return;
            }
            pick--;
        }
    }
}

static void reset_game(void) {
    g_head = 0;
    g_len = 3;
    g_dir = DIR_RIGHT;
    g_next_dir = DIR_RIGHT;
    g_score = 0;
    g_speed_ms = 150;
    g_paused = 0;
    g_over = 0;

    /* 初始蛇身横躺在中间：头在最右边 */
    g_body[0].x = W / 2;
    g_body[0].y = H / 2;
    g_body[1].x = W / 2 - 1;
    g_body[1].y = H / 2;
    g_body[2].x = W / 2 - 2;
    g_body[2].y = H / 2;

    place_food();
}

/* ---------- 绘制 ---------- */

static void draw(void) {
    /* 缓冲区：每行 (W+2) 个字符 + 换行，共 (H+2) 行 */
    static char buf[(W + 2 + 1) * (H + 2) + 1];
    int p = 0;
    int x, y, i;

    for (y = -1; y <= H; y++) {
        for (x = -1; x <= W; x++) {
            char c = ' ';

            if (x == -1 || x == W || y == -1 || y == H) {
                c = '#'; /* 墙 */
            } else if (x == g_food.x && y == g_food.y) {
                c = '*'; /* 食物 */
            } else {
                for (i = 0; i < g_len; i++) {
                    Point pt = g_body[body_index(i)];
                    if (pt.x == x && pt.y == y) {
                        c = (i == 0) ? '@' : 'o'; /* @ 蛇头，o 蛇身 */
                        break;
                    }
                }
            }
            buf[p++] = c;
        }
        buf[p++] = '\n';
    }

    move_cursor_home();
    fwrite(buf, 1, (size_t)p, stdout);
    fflush(stdout);
}

static void draw_hud(void) {
    printf("\r\n  得分: %-4d 长度: %-3d 速度: %dms   [方向键/WASD] 移动  [空格] 暂停  [R] 重开  "
           "[ESC] 退出\n",
           g_score, g_len, g_speed_ms);
    if (g_over) {
        printf("  游戏结束！按 R 重开，ESC 退出。\n");
    } else if (g_paused) {
        printf("  已暂停，按空格继续。\n");
    } else {
        printf("                                                                \n");
    }
    fflush(stdout);
}

/* ---------- 逻辑 ---------- */

/* 走一步。返回 0 表示撞死/撞墙，游戏结束 */
static int step(void) {
    Point head = g_body[g_head];
    int nx = head.x;
    int ny = head.y;

    /* 不允许 180 度掉头 */
    if ((g_next_dir + 2) % 4 != g_dir) {
        g_dir = g_next_dir;
    }

    switch (g_dir) {
    case DIR_UP:
        ny--;
        break;
    case DIR_RIGHT:
        nx++;
        break;
    case DIR_DOWN:
        ny++;
        break;
    case DIR_LEFT:
        nx--;
        break;
    default:
        break;
    }

    /* 撞墙 */
    if (nx < 0 || nx >= W || ny < 0 || ny >= H) {
        g_over = 1;
        return 0;
    }

    /* 撞自己（尾巴那一格马上会腾出来，所以不算） */
    {
        int i;
        for (i = 0; i < g_len - 1; i++) {
            Point pt = g_body[body_index(i)];
            if (pt.x == nx && pt.y == ny) {
                g_over = 1;
                return 0;
            }
        }
    }

    /* 移动：把新头放进队列，若是吃食物则长度 +1，否则丢掉尾巴 */
    g_head = (g_head - 1 + CELLS) % CELLS;
    g_body[g_head].x = nx;
    g_body[g_head].y = ny;
    g_len++;

    if (nx == g_food.x && ny == g_food.y) {
        g_score += 10;
        /* 吃得越多跑得越快，最快 60ms 一帧 */
        if (g_speed_ms > 60) {
            g_speed_ms -= 4;
        }
        place_food();
    } else {
        g_len--; /* 没吃到，长度回到原值（尾巴自然被覆盖） */
    }

    return 1;
}

/* 有没有按方向键（用来判定"开局"） */
static int dir_key_pressed(void) {
    static const int keys[8] = {VK_UP, 'W', VK_RIGHT, 'D', VK_DOWN, 'S', VK_LEFT, 'A'};
    int i;
    for (i = 0; i < 8; i++) {
        if (GetAsyncKeyState(keys[i]) & 0x8000) {
            return 1;
        }
    }
    return 0;
}

/* 非阻塞读键盘 */
static void poll_input(void) {
    if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
        g_next_dir = DIR_UP;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
        g_next_dir = DIR_RIGHT;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
        g_next_dir = DIR_DOWN;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
        g_next_dir = DIR_LEFT;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        g_paused = !g_paused;
        Sleep(180); /* 防止一按就切换好几次 */
    }
    if (GetAsyncKeyState('R') & 0x8000) {
        reset_game();
        Sleep(180);
    }
}

/* ---------- 自检模式（给自动化测试用，正常玩不会用到） ---------- */

static int self_test(void) {
    int frame;
    int alive = 1;
    int ok = 1;

    SetConsoleOutputCP(CP_UTF8);

    /* 测试 1：脚本化走一圈（右 6 -> 下 4 -> 左 5 -> 上 2），不该死 */
    reset_game();
    for (frame = 0; frame < 17 && alive; frame++) {
        if (frame == 6) {
            g_next_dir = DIR_DOWN;
        }
        if (frame == 10) {
            g_next_dir = DIR_LEFT;
        }
        if (frame == 15) {
            g_next_dir = DIR_UP;
        }
        alive = step();
    }
    printf("selftest 1 走位: 存活=%d（应为 1）长度=%d（应为 3）\n", alive, g_len);
    if (!alive || g_len != 3) {
        ok = 0;
    }

    /* 测试 2：吃掉前方的食物 -> 得分 +10、长度 +1 */
    reset_game();
    g_food.x = g_body[g_head].x + 1;
    g_food.y = g_body[g_head].y;
    g_next_dir = DIR_RIGHT;
    step();
    printf("selftest 2 吃食物: 得分=%d（应为 10）长度=%d（应为 4）\n", g_score, g_len);
    if (g_score != 10 || g_len != 4) {
        ok = 0;
    }

    /* 测试 3：一直往右撞墙 -> 应该 game over */
    reset_game();
    g_dir = DIR_RIGHT;
    g_next_dir = DIR_RIGHT;
    alive = 1;
    for (frame = 0; frame < 200 && alive; frame++) {
        alive = step();
    }
    printf("selftest 3 撞墙: over=%d（应为 1）\n", g_over);
    if (g_over != 1) {
        ok = 0;
    }

    /* 测试 4：180 度掉头应该被忽略（往右走时按左，仍往右） */
    reset_game();
    g_dir = DIR_RIGHT;
    g_next_dir = DIR_LEFT;
    {
        int x0 = g_body[g_head].x;
        step();
        printf("selftest 4 防掉头: 头 x %d -> %d（应 +1，即仍向右）\n", x0, g_body[g_head].x);
        if (g_body[g_head].x != x0 + 1) {
            ok = 0;
        }
    }

    printf("selftest 结果: %s\n", ok ? "全部通过" : "有失败项");
    return ok ? 0 : 1;
}

/* ---------- 主循环 ---------- */

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--selftest") == 0) {
        return self_test();
    }

    srand((unsigned)time(NULL));
    SetConsoleOutputCP(CP_UTF8); /* 让控制台认得中文 */
    hide_cursor();
    reset_game();

    {
        int started = 0;     /* 按过方向键才开始跑 */
        int over_drawn = 0;  /* 结束画面只画一次，避免刷屏 */

        draw();
        draw_hud();
        printf("\r\n  按 方向键 / WASD 开始\n");
        fflush(stdout);

        while (1) {
            poll_input();

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                break;
            }

            if (!started && dir_key_pressed()) {
                started = 1;
                draw();
                draw_hud();
            }

            if (!started) {
                Sleep(30); /* 等开局，别重绘 */
                continue;
            }

            if (!g_paused && !g_over && started) {
                step();
            }

            if (g_over) {
                if (!over_drawn) {
                    draw();
                    draw_hud();
                    over_drawn = 1;
                }
                Sleep(60); /* 别空转重绘：慢慢等 R / ESC */
                if (GetAsyncKeyState('R') & 0x8000) {
                    reset_game();
                    started = 0;
                    over_drawn = 0;
                    Sleep(180);
                    draw();
                    draw_hud();
                    printf("\r\n  按 方向键 / WASD 开始\n");
                    fflush(stdout);
                }
                continue;
            }

            over_drawn = 0;
            draw();
            draw_hud();
            Sleep(g_speed_ms);
        }
    }

    printf("\n");
    return 0;
}
