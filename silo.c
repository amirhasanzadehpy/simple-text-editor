/*** include ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios org_termios;

/*** terminal ***/
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios) == -1)
    die("tcsetattr");
}

void enableRawMode(void) {
  if (tcgetattr(STDIN_FILENO, &org_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = org_termios;

  raw.c_iflag &= ~(IXON | ICRNL);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VTIME] = 1;
  raw.c_cc[VMIN] = 0;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

/*** init ***/
int main(void) {
  enableRawMode();

  while (1) {
    char c = '\0';

    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");

    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }

    if (c == 'q')
      break;
  };

  return 0;
}
