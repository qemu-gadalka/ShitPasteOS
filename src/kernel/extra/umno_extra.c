#include "../includes/umno_extra.h"

// ========== NikoClicker ==========
typedef struct {
    int niko_count;
    int niko_per_click;
    int niko_per_second;
    int upgrade1_level;
    int upgrade2_level;
    int upgrade3_level;
    int upgrade1_cost;
    int upgrade2_cost;
    int upgrade3_cost;
} GameState;

static GameState game;

void UmnoClicker(void) {
    game.niko_count = 0;
    game.niko_per_click = 1;
    game.niko_per_second = 0;
    game.upgrade1_level = 0;
    game.upgrade2_level = 0;
    game.upgrade3_level = 0;
    game.upgrade1_cost = 10;
    game.upgrade2_cost = 50;
    game.upgrade3_cost = 100;
    cls();

    print("=== NikoClicker ===\n");
    print("\n\n");

    printf("Nikos: %d\n", game.niko_count);
    printf("Per Click: %d | Per Second: %d\n\n", game.niko_per_click, game.niko_per_second);
    printf("1. Better Clicker (Level %d) - Cost: %d\n", game.upgrade1_level, game.upgrade1_cost);
    printf("2. Auto Clicker (Level %d) - Cost: %d\n", game.upgrade2_level, game.upgrade2_cost);
    printf("3. Mega Clicker (Level %d) - Cost: %d\n\n", game.upgrade3_level, game.upgrade3_cost);
    print("Press SPACE to click, 1-3 to buy upgrades, Q to quit\n");
    int needs_redraw = 0;
    int auto_click_counter = 0;

    int safe_multiply(int value, float multiplier) {
        if (value > 1000000000) return 2000000000;
        
        long long result = (long long)value * multiplier;
        if (result > 2000000000) return 2000000000;
        if (result < value) return value + 1;
        
        return (int)result;
    }

    while (1) {
        int c = get_char_nonblocking();
        
        if (c > 0) {
            if (c == ' ') {
                if (game.niko_count <= 2000000000 - game.niko_per_click) {
                    game.niko_count += game.niko_per_click;
                } else {
                    game.niko_count = 2000000000;
                }
                needs_redraw = 1;
            }
            else if (c == '1') {
                if (game.upgrade1_cost >= 1000000000) {
                    print("Max level reached for Better Clicker!\n");
                    needs_redraw = 1;
                } else if (game.niko_count >= game.upgrade1_cost) {
                    game.niko_count -= game.upgrade1_cost;
                    game.upgrade1_level++;
                    
                    if (game.niko_per_click < 1000000000) {
                        game.niko_per_click += 1;
                    }
                    
                    game.upgrade1_cost = safe_multiply(game.upgrade1_cost, 1.5f);
                    needs_redraw = 1;
                }
            }
            else if (c == '2') {
                if (game.upgrade2_cost >= 1000000000) {
                    print("Max level reached for Auto Clicker!\n");
                    needs_redraw = 1;
                } else if (game.niko_count >= game.upgrade2_cost) {
                    game.niko_count -= game.upgrade2_cost;
                    game.upgrade2_level++;
                    
                    if (game.niko_per_second < 1000000000) {
                        game.niko_per_second += 1;
                    }
                    
                    game.upgrade2_cost = safe_multiply(game.upgrade2_cost, 1.5f);
                    needs_redraw = 1;
                }
            }
            else if (c == '3') {
                if (game.upgrade3_cost >= 1000000000) {
                    print("Max level reached for Mega Clicker!\n");
                    needs_redraw = 1;
                } else if (game.niko_count >= game.upgrade3_cost) {
                    game.niko_count -= game.upgrade3_cost;
                    game.upgrade3_level++;
                    
                    if (game.niko_per_click <= 1000000000 - 5) {
                        game.niko_per_click += 5;
                    } else {
                        game.niko_per_click = 1000000000;
                    }
                    
                    game.upgrade3_cost = safe_multiply(game.upgrade3_cost, 2.0f);
                    needs_redraw = 1;
                }
            }
            else if (c == '\t') {
                kernel_panic();
            }
            else if (c == 'q' || c == 'Q') {
                break;
            }
        }

        if (game.niko_per_second > 0) {
            auto_click_counter++;
            if (auto_click_counter >= 15) {
                if (game.niko_count <= 2000000000 - game.niko_per_second) {
                    game.niko_count += game.niko_per_second;
                } else {
                    game.niko_count = 2000000000;
                }
                auto_click_counter = 0;
                needs_redraw = 1;
            }
        }

        if (needs_redraw) {
            cls();
            
            printf("Nikos: %d\n", game.niko_count);
            printf("Per Click: %d | Per Second: %d\n\n", game.niko_per_click, game.niko_per_second);
            printf("1. Better Clicker (Level %d) - Cost: %d\n", game.upgrade1_level, game.upgrade1_cost);
            printf("2. Auto Clicker (Level %d) - Cost: %d\n", game.upgrade2_level, game.upgrade2_cost);
            printf("3. Mega Clicker (Level %d) - Cost: %d\n\n", game.upgrade3_level, game.upgrade3_cost);
            print("Press SPACE to click, 1-3 to buy upgrades, Q to quit\n");
            needs_redraw = 0;
        }

        for (volatile int i = 0; i < 50000; i++);
    }
    
    cls();
    printf("Game ended! Final score: %d Nikos\n", game.niko_count);
    print("Press any key to continue...\n");
    get_char_blocking();
}

// ========== Tetris ==========
#define TETRIS_W 10
#define TETRIS_H 20

static int tetris_board[TETRIS_H][TETRIS_W] = {0};
static int tetris_cx, tetris_cy, tetris_id;
static int tetris_score = 0;
static int game_over = 0;
static int fall_delay = 30;
static int fall_counter = 0;

static int TetrisShapes[7][4][4] = {
    {{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}
};

static void TetrisDrawBoard(void);
static int TetrisValid(int px, int py);
static void TetrisLock(void);
static void TetrisClearLines(void);
static void TetrisSpawn(void);
static int TetrisTryOffset(int ox, int oy);
static void TetrisRotate(void);

static void TetrisDrawBoard(void) {
    cls();
    
    print("#");
    for (int i = 0; i < TETRIS_W * 3; i++) print("#");
    print("#\n");

    for (int y = 0; y < TETRIS_H; y++) {
        print("#");
        for (int x = 0; x < TETRIS_W; x++) {
            int solid = tetris_board[y][x];
            int current = 0;

            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 4; xx++) {
                    if (TetrisShapes[tetris_id][yy][xx]) {
                        if (x == tetris_cx + xx && y == tetris_cy + yy) {
                            current = 1;
                            break;
                        }
                    }
                }
                if (current) break;
            }

            if (current) print("[ ]");
            else if (solid) print("[#]");
            else print("   ");
        }
        print("#\n");
    }

    print("#");
    for (int i = 0; i < TETRIS_W * 3; i++) print("#");
    print("#\n");
    
    printf("Score: %d\n", tetris_score);
    print("Controls: A-left, D-right, S-down, W-rotate, SPACE-hard drop, Q-quit\n");
    
    if (game_over) {
        print("\nGAME OVER! Press any key to continue...\n");
    }
}

static int TetrisValid(int px, int py) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (TetrisShapes[tetris_id][y][x]) {
                int bx = px + x;
                int by = py + y;
                if (bx < 0 || bx >= TETRIS_W || by < 0 || by >= TETRIS_H) return 0;
                if (tetris_board[by][bx]) return 0;
            }
        }
    }
    return 1;
}

static void TetrisLock(void) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (TetrisShapes[tetris_id][y][x]) {
                tetris_board[tetris_cy + y][tetris_cx + x] = 1;
            }
        }
    }
}

static void TetrisClearLines(void) {
    for (int y = TETRIS_H - 1; y >= 0; y--) {
        int full = 1;
        for (int x = 0; x < TETRIS_W; x++) {
            if (!tetris_board[y][x]) {
                full = 0;
                break;
            }
        }
        if (full) {
            tetris_score += 100;

            if (tetris_score % 500 == 0 && fall_delay > 10) {
                fall_delay -= 5;
            }

            for (int yy = y; yy > 0; yy--) {
                for (int x = 0; x < TETRIS_W; x++) {
                    tetris_board[yy][x] = tetris_board[yy - 1][x];
                }
            }
            for (int x = 0; x < TETRIS_W; x++) {
                tetris_board[0][x] = 0;
            }
            y++;
        }
    }
}

static void TetrisSpawn(void) {
    static unsigned int seed = 12345;
    seed = seed * 1103515245 + 12345;
    tetris_id = (seed >> 16) % 7;
    
    tetris_cx = TETRIS_W / 2 - 2;
    tetris_cy = 0;

    if (!TetrisValid(tetris_cx, tetris_cy)) {
        game_over = 1;
    }
}

static int TetrisTryOffset(int ox, int oy) {
    if (TetrisValid(tetris_cx + ox, tetris_cy + oy)) {
        tetris_cx += ox;
        tetris_cy += oy;
        return 1;
    }
    return 0;
}

static void TetrisRotate(void) {
    int temp[4][4];
    int original[4][4];
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            original[y][x] = TetrisShapes[tetris_id][y][x];
            temp[y][x] = 0;
        }
    }
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            temp[x][3 - y] = original[y][x];
        }
    }
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            TetrisShapes[tetris_id][y][x] = temp[y][x];
        }
    }
    
    if (!TetrisValid(tetris_cx, tetris_cy)) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                TetrisShapes[tetris_id][y][x] = original[y][x];
            }
        }
    }
}

void UmnoTetris(void) {
    cls();
    print("Welcome to UmnoTetris!\n");
    
    for (int y = 0; y < TETRIS_H; y++) {
        for (int x = 0; x < TETRIS_W; x++) {
            tetris_board[y][x] = 0;
        }
    }
    tetris_score = 0;
    game_over = 0;
    fall_delay = 30;
    fall_counter = 0;
    
    TetrisSpawn();
    
    int needs_redraw = 1;

    while (!game_over) {
        int c = get_char_nonblocking();
        if (c > 0) {
            int moved = 0;
            if (c == 'a' || c == 'A') moved = TetrisTryOffset(-1, 0);
            else if (c == 'd' || c == 'D') moved = TetrisTryOffset(1, 0);
            else if (c == 's' || c == 'S') { 
                moved = TetrisTryOffset(0, 1);
                if (moved) tetris_score++; 
            }
            else if (c == 'w' || c == 'W') {
                TetrisRotate();
                moved = 1;
            }
            else if (c == ' ') {
                while (TetrisTryOffset(0, 1)) {
                    tetris_score++;
                }
                TetrisLock();
                TetrisClearLines();
                TetrisSpawn();
                moved = 1;
            }
            else if (c == 'q' || c == 'Q') {
                break;
            }
            
            if (moved) {
                needs_redraw = 1;
            }
        }

        fall_counter++;
        if (fall_counter >= fall_delay) {
            fall_counter = 0;
            if (!TetrisTryOffset(0, 1)) {
                TetrisLock();
                TetrisClearLines();
                TetrisSpawn();
            }
            needs_redraw = 1;
        }

        if (needs_redraw) {
            TetrisDrawBoard();
            needs_redraw = 0;
        }
        
        for (volatile int i = 0; i < 5000000; i++);
    }
    
    if (game_over) {
        TetrisDrawBoard();
        get_char_blocking();
    }
}

// ========== Launcher ==========
void UmnoLauncher(void) {
    cls();
    print("Welcome to UmnoLauncher!\n\n");
    char buf[64];

    while (1) {
        print("UmnoLauncher> ");
        size_t pos = 0;

        // Чтение команды
        while (1) {
            int c = get_char_blocking();
            
            if (c > 0) {
                if (c == '\r' || c == '\n') {
                    print("\n");
                    break;
                } else if (c == '\b' || c == 127) {
                    if (pos > 0) {
                        pos--;
                        print("\b \b");
                    }
                } else {
                    if (pos < sizeof(buf) - 1) {
                        buf[pos++] = (char)c;
                        char s[2] = { (char)c, 0 };
                        print(s);
                    }
                }
            }
        }

        buf[pos] = 0;
        
        if (pos == 0) {
            continue;
        }

        if (strcmp(buf, "1") == 0) {
            UmnoClicker();
        }
        else if (strcmp(buf, "2") == 0) {
            UmnoTetris();
        }
        else if (strcmp(buf, "help") == 0) {
            print("1 - NikoClicker\n2 - NikoTetris\nexit - Exit from UmnoLauncher\n\n");
        }
        else if (strcmp(buf, "exit") == 0) {
            cls();
            break;
        }
        else {
            printf("Unknown command: '%s'\n", buf);
            print("Type 'help' for available commands.\n");
        }
        
    }
}