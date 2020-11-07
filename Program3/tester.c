#include <stdlib.h>
#include <stdio.h>

int main() {
  int i;

  for (i=0;i<5;i++) {
    char str[20];
    sprintf(str, "p3 %d >> test_result", i);
    printf("executing: %s\n", str);
    system(str);
  }

  return 0;
}