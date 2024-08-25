#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <ncurses.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

// Function to disable raw mode
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Function to enable raw mode
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Function to draw the file explorer
void drawFileExplorer(WINDOW *win, const char *path) {
    struct dirent *dp;
    DIR *dir = opendir(path);

    if (!dir) return;

    int y = 1;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_DIR) {
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, y++, 1, "📁 %s", dp->d_name);
            wattroff(win, COLOR_PAIR(2));
        } else {
            mvwprintw(win, y++, 1, "📄 %s", dp->d_name);
        }
    }

    closedir(dir);
}

// Function to draw the status bar
void drawStatusBar(WINDOW *win) {
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 0, 1, " File Explorer - q to quit | wq to save and quit ");
    wattroff(win, COLOR_PAIR(1));
}

// Function to draw the editor interface
void drawEditor(WINDOW *fileWin, WINDOW *editorWin, WINDOW *statusWin) {
    wclear(fileWin);
    wclear(editorWin);
    wclear(statusWin);

    box(fileWin, 0, 0);
    box(editorWin, 0, 0);
    box(statusWin, 0, 0);

    drawFileExplorer(fileWin, ".");
    drawStatusBar(statusWin);

    wrefresh(fileWin);
    wrefresh(editorWin);
    wrefresh(statusWin);
}

// Function to process commands
void processCommand(const char *command) {
    if (strcmp(command, ":q") == 0) {
        endwin();
        exit(0);
    } else if (strcmp(command, ":wq") == 0) {
        // Implement save functionality here
        endwin();
        exit(0);
    }
    // Add more commands as needed
}

// Function to process keypresses in the main editor loop
void editorProcessKeypress(WINDOW *cmdWin) {
    char command[80];
    mvwgetnstr(cmdWin, 1, 1, command, 79);
    processCommand(command);
}

// Main loop for the editor
void editorLoop(WINDOW *fileWin, WINDOW *editorWin, WINDOW *statusWin, WINDOW *cmdWin) {
    while (1) {
        drawEditor(fileWin, editorWin, statusWin);
        editorProcessKeypress(cmdWin);
    }
}

// Main function
int main() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *fileWin = newwin(height - 3, width / 4, 0, 0);
    WINDOW *editorWin = newwin(height - 3, (3 * width) / 4, 0, width / 4);
    WINDOW *statusWin = newwin(3, width, height - 3, 0);
    WINDOW *cmdWin = statusWin;

    editorLoop(fileWin, editorWin, statusWin, cmdWin);

    endwin();
    return 0;
}
