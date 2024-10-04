#include <iostream>
#include <cstdio>
#include <omp.h>
#include <chrono>

using namespace std;

int main() {
    const char *leftEyeFileName = "lion-chicago-l.ppm";
    const char *rightEyeFileName = "lion-chicago-r.ppm";
    const char *anaglyphFileName = "anaglyph.ppm";

    FILE *leftEyeFile = fopen(leftEyeFileName, "rb");
    FILE *rightEyeFile = fopen(rightEyeFileName, "rb");

    int width, height;
    fscanf(leftEyeFile, "P6\n%d %d\n255\n", &width, &height);
    
    int imageSize = width * height * 3;

    unsigned char* leftImage = new unsigned char[imageSize];
    fread(leftImage, 1, imageSize, leftEyeFile);
    fclose(leftEyeFile);

    fscanf(rightEyeFile, "P6\n%d %d\n255\n", &width, &height);

    imageSize = width * height * 3;

    unsigned char* rightImage = new unsigned char[imageSize];
    fread(rightImage, 1, imageSize, rightEyeFile);
    fclose(rightEyeFile);

    auto start = chrono::high_resolution_clock::now();

    unsigned char* anaglyph = new unsigned char[imageSize];

    int x, y;
    #pragma omp parallel for private(x, y)
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int index = (y * width + x) * 3;

            //anaglyph[index + 1] = (leftImage[index + 1] + rightImage[index + 1]) / 2; // Gfinal = (Gleft + Gright) / 2
            anaglyph[index] = leftImage[index];
            anaglyph[index + 1] = rightImage[index + 1];
            anaglyph[index + 2] = rightImage[index + 2];
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    printf("Czas wykonania wynosi: %f sekundy\n", duration.count());

    FILE *anaglyphFile = fopen(anaglyphFileName, "wb");

    fprintf(anaglyphFile, "P6\n%d %d\n255\n", width, height);
    fwrite(anaglyph, 1, imageSize, anaglyphFile);

    fclose(anaglyphFile);

    delete[] leftImage;
    delete[] rightImage;
    delete[] anaglyph;

    return 0;
}
