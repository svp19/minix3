#include <stdio.h>

typedef struct {
    int* r, g, b;
    int w, h;
} Image;

Image readFile(FILE *fp) {
    char buff[255];
    Image image;

    // Skip first line
    fgets(buff, 255, (FILE*)fp);

    // Read w, h
    fgets(buff, 255, (FILE*)fp);
    char dim[100];

    return image;
}

int main(int argc, char *argv[]) {

    // Read PPM file
    FILE* image_p = fopen("mussorie.ppm", "r");
    int pixel;

    while (fscanf(image_p, "%d", &pixel) != EOF) {
        printf("%d", pixel);
    }

    return 0;
}