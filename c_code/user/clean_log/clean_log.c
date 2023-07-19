#include <stdio.h>

int main() {
    FILE *fp;
    fp = fopen("log", "w");
    fclose(fp);
    return 0;
}
