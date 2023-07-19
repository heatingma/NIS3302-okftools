#include <stdio.h>

int main() {
    FILE *fp;
    fp = fopen("log.txt", "w");
    fclose(fp);
    return 0;
}
