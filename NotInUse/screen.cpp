/*** includes ***/
#include <iostream>
#include <sys/ioctl.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
void setCursorPosition(int XPos, int YPos) {
  printf("\033[%d;%dH",YPos+1,XPos+1);
}
void getCursor(int* x, int* y) {
  printf("\033[6n");  /* This escape sequence !writes! the current
                         coordinates to the terminal.
                         We then have to read it from there, see [4,5].
                         Needs <termios.h>,<unistd.h> and some others */
  scanf("\033[%d;%dR", x, y);
}

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main() {
  int cols, rows;
  //enableRawMode();
  struct winsize ts;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
  cols = ts.ws_col;
  rows = ts.ws_row;

  for(int i = 0; i < 5; i++){
    int x, y;
    getCursor(&x, &y);
    std::cout << x << " " <<y<< std::endl;
    setCursorPosition(x+5, y + 4);
  }


  return 0;
}
