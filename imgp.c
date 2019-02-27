#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"

int main(int argc, char *argv[]) {

    char *namebw, *namef1, *namef2, *namef3;

    int i, j, grey, sum;
    int g1, g2, g3, gray;

    struct bmp_fileheader bmpf;
    struct bmp_infoheader bmpi;

    struct pixel {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    };

    // citire bmp intrare

    FILE *bmpin = fopen(argv[1], "rb");

    fread(&bmpf, sizeof(struct bmp_fileheader), 1, bmpin);

    fread(&bmpi, sizeof(struct bmp_infoheader), 1, bmpin);

    fseek(bmpin, bmpf.imageDataOffset, SEEK_SET);

    struct pixel *pix;
    pix = calloc((bmpi.height + 2) * (bmpi.width + 2), sizeof(struct pixel));

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {
            fread(&pix[i * bmpi.width + j], sizeof(struct pixel), 1, bmpin);
        }
    }

    fclose(bmpin);

    namebw = (char*) malloc(strlen(argv[1]) + strlen("_black_white"));
    namef1 = (char*) malloc(strlen(argv[1]) + strlen("_f1"));
    namef2 = (char*) malloc(strlen(argv[1]) + strlen("_f2"));
    namef3 = (char*) malloc(strlen(argv[1]) + strlen("_f3"));

    strncpy(namebw, argv[1], strlen(argv[1]) - 4);
    strncpy(namef1, argv[1], strlen(argv[1]) - 4);
    strncpy(namef2, argv[1], strlen(argv[1]) - 4);
    strncpy(namef3, argv[1], strlen(argv[1]) - 4);

    strcat(namebw, "_black_white.bmp");
    strcat(namef1, "_f1.bmp");
    strcat(namef2, "_f2.bmp");
    strcat(namef3, "_f3.bmp");

    FILE *bmpout = fopen(namebw, "wb");

    fwrite(&bmpf, sizeof(bmpf), 1, bmpout);
    fwrite(&bmpi, sizeof(bmpi), 1, bmpout);

    fseek(bmpout, bmpf.imageDataOffset, SEEK_SET);

    // calcul bmp black and white

    struct pixel *pixbw;
    pixbw = calloc((bmpi.height + 2) * (bmpi.width + 2), sizeof(struct pixel));

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {
            sum =
                pix[i * bmpi.width + j].g +
                pix[i * bmpi.width + j].b +
                pix[i * bmpi.width + j].r;

            grey = sum / 3;

            pixbw[i * bmpi.width + j].g = grey;
            pixbw[i * bmpi.width + j].b = grey;
            pixbw[i * bmpi.width + j].r = grey;

            fwrite(&pixbw[i * bmpi.width + j], sizeof(struct pixel), 1, bmpout);
        }
    }

    fclose(bmpout);

    // calcul si scriere edge detection pt kernel
    /*
       -1	-1	-1
       -1	 8	-1
       -1	-1	-1
     */

    FILE *bmpout2 = fopen(namef1, "wb");

    fwrite(&bmpf, sizeof(bmpf), 1, bmpout2);
    fwrite(&bmpi, sizeof(bmpi), 1, bmpout2);

    fseek(bmpout2, bmpf.imageDataOffset, SEEK_SET);

    struct pixel *pixf;
    pixf = calloc((bmpi.height + 2) * (bmpi.width + 2), sizeof(struct pixel));

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {

            g1 =
                pixbw[(i - 1) * bmpi.width + (j - 1)].g * (-1) +
                pixbw[(i - 1) * bmpi.width + j].g * (-1) +
                pixbw[(i - 1) * bmpi.width + (j + 1)].g * (-1);
            g2 =
                pixbw[i * bmpi.width + (j - 1)].g * (-1) +
                pixbw[i * bmpi.width + j].g * 8 +
                pixbw[i * bmpi.width + (j + 1)].g * (-1);
            g3 =
                pixbw[(i + 1) * bmpi.width + (j - 1)].g * (-1) +
                pixbw[(i + 1) * bmpi.width + j].g * (-1) +
                pixbw[(i + 1) * bmpi.width + (j + 1)].g * (-1);

            gray = g1 + g2 + g3;

            if (gray > 255) {
                gray = 255;
            }

            if (gray < 0) {
                gray = 0;
            }

            pixf[i * bmpi.width + j].g = gray;
            pixf[i * bmpi.width + j].b = gray;
            pixf[i * bmpi.width + j].r = gray;

            //marginea stanga
            if ((j == 1 && i != bmpi.height) ||
                (j == 1 && i != 1)) {

                g1 =
                    pixbw[(i - 1) * bmpi.width + j].g * (-1) +
                    pixbw[(i - 1) * bmpi.width + (j + 1)].g * (-1);
                g2 =
                    pixbw[i * bmpi.width + j].g * 8 +
                    pixbw[i * bmpi.width + (j + 1)].g * (-1);
                g3 =
                    pixbw[(i + 1) * bmpi.width + j].g * (-1) +
                    pixbw[(i + 1) * bmpi.width + (j + 1)].g * (-1);

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }

            //marginea dreapta
            if ((j == bmpi.width && i != bmpi.height) ||
                (j == bmpi.width && i != 1)) {

                g1 =
                    pixbw[(i - 1) * bmpi.width + (j - 1)].g * (-1) +
                    pixbw[(i - 1) * bmpi.width + j].g * (-1);
                g2 =
                    pixbw[i * bmpi.width + (j - 1)].g * (-1) +
                    pixbw[i * bmpi.width + j].g * 8;
                g3 =
                    pixbw[(i + 1) * bmpi.width + (j - 1)].g * (-1) +
                    pixbw[(i + 1) * bmpi.width + j].g * (-1);

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }
        }
    }

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {
            fwrite(&pixf[i * bmpi.width + j], sizeof(struct pixel), 1, bmpout2);
        }
    }

    fclose(bmpout2);

    // calcul si scriere edge detection pt kernel
    /*
       0	 1	 0
       1	-4	 1
       0	 1	 0
     */

    FILE *bmpout3 = fopen(namef2, "wb");

    fwrite(&bmpf, sizeof(bmpf), 1, bmpout3);
    fwrite(&bmpi, sizeof(bmpi), 1, bmpout3);

    fseek(bmpout3, bmpf.imageDataOffset, SEEK_SET);

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {

            g1 =
                pixbw[(i - 1) * bmpi.width + j].g;
            g2 =
                pixbw[i * bmpi.width + (j - 1)].g +
                pixbw[i * bmpi.width + j].g * (-4) +
                pixbw[i * bmpi.width + (j + 1)].g;
            g3 =
                pixbw[(i + 1) * bmpi.width + j].g;

            gray = g1 + g2 + g3;

            if (gray > 255) {
                gray = 255;
            }

            if (gray < 0) {
                gray = 0;
            }

            pixf[i * bmpi.width + j].g = gray;
            pixf[i * bmpi.width + j].b = gray;
            pixf[i * bmpi.width + j].r = gray;

            //marginea stanga
            if ((j == 1 && i != bmpi.height) ||
                (j == 1 && i != 1)) {

                g1 =
                    pixbw[(i - 1) * bmpi.width + j].g;
                g2 =
                    pixbw[i * bmpi.width + j].g * (-4) +
                    pixbw[i * bmpi.width + (j + 1)].g;
                g3 =
                    pixbw[(i + 1) * bmpi.width + j].g;

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }

            //marginea dreapta
            if ((j == bmpi.width && i != bmpi.height) ||
                (j == bmpi.width && i != 1)) {

                g1 =
                    pixbw[(i - 1) * bmpi.width + j].g;
                g2 =
                    pixbw[i * bmpi.width + (j - 1)].g +
                    pixbw[i * bmpi.width + j].g * (-4);
                g3 =
                    pixbw[(i + 1) * bmpi.width + j].g;

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }
        }
    }

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {
            fwrite(&pixf[i * bmpi.width + j], sizeof(struct pixel), 1, bmpout3);
        }
    }

    fclose(bmpout3);

    // calcul si scriere edge detection pt kernel
    /*
       1	 0	-1
       0	 0	 0
       -1	 0	 1
     */

    FILE *bmpout4 = fopen(namef3, "wb");

    fwrite(&bmpf, sizeof(bmpf), 1, bmpout4);
    fwrite(&bmpi, sizeof(bmpi), 1, bmpout4);

    fseek(bmpout4, bmpf.imageDataOffset, SEEK_SET);

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {

            g1 =
                pixbw[(i - 1) * bmpi.width + (j - 1)].g +
                pixbw[(i - 1) * bmpi.width + (j + 1)].g * (-1);
            g2 = 0;
            g3 =
                pixbw[(i + 1) * bmpi.width + (j - 1)].g * (-1) +
                pixbw[(i + 1) * bmpi.width + (j + 1)].g;

            gray = g1 + g2 + g3;

            if (gray > 255) {
                gray = 255;
            }

            if (gray < 0) {
                gray = 0;
            }

            pixf[i * bmpi.width + j].g = gray;
            pixf[i * bmpi.width + j].b = gray;
            pixf[i * bmpi.width + j].r = gray;

            //marginea stanga
            if ((j == 1 && i != bmpi.height) ||
                (j == 1 && i != 1)) {
                g1 =
                    pixbw[(i - 1) * bmpi.width + (j + 1)].g * (-1);
                g2 = 0;
                g3 =
                    pixbw[(i + 1) * bmpi.width + (j + 1)].g;

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }

            //marginea dreapta
            if ((j == bmpi.width && i != bmpi.height) ||
                (j == bmpi.width && i != 1)) {
                g1 =
                    pixbw[(i - 1) * bmpi.width + (j - 1)].g;
                g2 = 0;
                g3 =
                    pixbw[(i + 1) * bmpi.width + (j - 1)].g * (-1);

                gray = g1 + g2 + g3;

                if (gray > 255) {
                    gray = 255;
                }

                if (gray < 0) {
                    gray = 0;
                }

                pixf[i * bmpi.width + j].g = gray;
                pixf[i * bmpi.width + j].b = gray;
                pixf[i * bmpi.width + j].r = gray;
            }
        }
    }

    for (i = bmpi.height; i >= 1; i--) {
        for (j = 1; j < bmpi.width + 1; j++) {
            fwrite(&pixf[i * bmpi.width + j], sizeof(struct pixel), 1, bmpout4);
        }
    }

    fclose(bmpout4);
    free(pix);
    free(pixbw);
    free(pixf);

    return 0;
}
