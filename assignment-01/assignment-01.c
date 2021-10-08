/* Richard Davis
 * CSCI 4100
 * kittycat
 * This program reads and displays input streams
 */

#include <stdio.h>
#include <stdlib.h>

void display_stream(FILE *stream);

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int i;
    FILE *file;
    for (i = 1; i < argc; i = i + 1) {
      file = fopen(argv[i], "r");
      display_stream(file);
      fclose(file);
    }
  } else {
    display_stream(stdin);
  }

  return 0;
}

void display_stream(FILE *stream) {
  if (stream == NULL) {
    perror("error");
    exit(EXIT_FAILURE);
  }

  while(1) {
    char character = fgetc(stream);
    if (character == EOF) {
      break;
    }
    putchar(character);
  }
}
