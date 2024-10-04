#include <iostream>
#include <cmath>
#include <omp.h>
#include <chrono>

using namespace std;

int main() {
    const char *inputFileName = "lena_std.ppm";
    const char *outputFileName = "rabbit_kuwahara.ppm";

    unsigned char *color;
    int width, height;

    FILE *file = fopen(inputFileName, "rb");

    fscanf(file, "P6\n%d %d\n255\n", &width, &height);

    int imageSize = width * height * 3;
    color = new unsigned char[imageSize];

    fread(color, 1, imageSize, file);

    fclose(file);

    auto start = chrono::high_resolution_clock::now();

    unsigned char *kuwaharaImage = color;
    int filterSize = 6;
    int filterRegion = filterSize / 2;
    int x, y;

    #pragma omp parallel private(x, y)
    {
        #pragma omp for
        for (y = filterRegion; y < height - filterRegion; y++) {
            for (x = filterRegion; x < width - filterRegion; x++) {
                for (int rgb = 0; rgb < 3; rgb++) {
                    double variances[4];

                    for (int i = 0; i < 4; i++) {
                        int row = x + (i % 2) * filterRegion;
                        int col = y + (i / 2) * filterRegion;

                        double s = 0.0;
                        double variance = 0.0;

                        for (int j = 0; j < filterRegion; j++) {
                            for (int k = 0; k < filterRegion; k++) {
                                int rowPix = row + k;
                                int colPix = col + j;

                                int index = (colPix * width + rowPix) * 3 + rgb;
                                unsigned char pixelValue = color[index];
                                s += pixelValue;
                            }
                        }

                        for (int j = 0; j < filterRegion; j++) {
                            for (int k = 0; k < filterRegion; k++) {
                                int rowPix = row + k;
                                int colPix = col + j;

                                int index = (colPix * width + rowPix) * 3 + rgb;
                                unsigned char pixelValue = color[index];
                                variance += pow(pixelValue - (s / pow(filterRegion, 2)), 2);
                            }
                        }

                        variances[i] = variance;
                    }

                    int regionWithMinVariance = 0;
                    for (int i = 1; i < 4; i++) {
                        if (variances[i] < variances[regionWithMinVariance]) {
                            regionWithMinVariance = i;
                        }
                    }

                    int row = x + (regionWithMinVariance % 2) * filterRegion;
                    int col = y + (regionWithMinVariance / 2) * filterRegion;

                    double newValue = 0.0;
                    for (int j = 0; j < filterRegion; j++) {
                        for (int k = 0; k < filterRegion; k++) {
                            int rowPix = row + k;
                            int colPix = col + j;

                            int index = (colPix * width + rowPix) * 3 + rgb;
                            newValue += color[index];
                        }
                    }

                    int kuwaharaIndex = (y * width + x) * 3 + rgb;
                    kuwaharaImage[kuwaharaIndex] = (newValue / pow(filterRegion, 2));
                }
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    printf("Czas wykonania wynosi: %f sekundy\n", duration.count());

    FILE *outputFile = fopen(outputFileName, "wb");

    fprintf(outputFile, "P6\n%d %d\n255\n", width, height);

    int outputImageSize = imageSize;
    fwrite(kuwaharaImage, 1, outputImageSize, outputFile);

    fclose(outputFile);

    delete[] color;
    delete[] kuwaharaImage;

    return 0;
}
