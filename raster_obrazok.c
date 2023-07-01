#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#define NEPODARILO_SA_ZAPISAT 2
#define SUBOR_SA_NEOTVORIL 1
#define PIX(im, x, y) im->px[(y) * im->width + x]

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned char *px;
} GSI;

void gsi_destroy(GSI *img)
{
    if (img != NULL)
    {
        if (img->px != NULL)
        {
            free(img->px);
            img->px = NULL;
        }
        free(img);
    }
}

GSI *gsi_create_empty(void)
{
    GSI *newGSI = (GSI *)malloc(sizeof(GSI));
    if (newGSI == NULL)
    {
        return NULL;
    }

    newGSI->width = 0;
    newGSI->height = 0;
    newGSI->px = NULL;

    return newGSI;
}

GSI *gsi_create_with_geometry(unsigned int m, unsigned int n)
{
    GSI *newGSI = (GSI *)malloc(sizeof(GSI));
    if (newGSI == NULL)
    {
        return NULL;
    }

    newGSI->width = (unsigned int)m;
    newGSI->height = (unsigned int)n;
    newGSI->px = (unsigned char *)malloc(m * n * sizeof(unsigned char));
    if (newGSI->px == NULL)
    {
        free(newGSI);
        return NULL;
    }

    return newGSI;
}

GSI *gsi_create_with_geometry_and_color(unsigned int m, unsigned int n, unsigned char color)
{
    GSI *newGSI = gsi_create_with_geometry(m, n);
    if (newGSI == NULL)
    {
        free(newGSI);
        return NULL;
    }

    unsigned int totalPixels = m * n;
    for (unsigned int i = 0; i < totalPixels; i++)
    {
        newGSI->px[i] = color;
    }

    return newGSI;
}

/**
 * @return 1 - meotvoril subor
 * 2 - nepodarilo sa zapisat
 */
char gsi_save_as_pgm5(GSI *img, char *file_name, char *comment)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        return SUBOR_SA_NEOTVORIL;
    }

    if (fprintf(file, "P5\n") < 0)
    {
        // failed to write
        fclose(file);
        return NEPODARILO_SA_ZAPISAT;
    }
    if (comment != NULL)
    {
        fprintf(file, "# %s\n", comment);
        if (ferror(file)) {
            fclose(file);
            return NEPODARILO_SA_ZAPISAT;
        }        
    }
    if (fprintf(file, "%d %d 255 ", img->width, img->height) < 0)
    {
        // failed to write
        fclose(file);
        return NEPODARILO_SA_ZAPISAT;
    }

    fwrite(img->px,img->height*img->width,1,file);
    if (ferror(file)) {
        fclose(file);
        return NEPODARILO_SA_ZAPISAT;
    }
    fclose(file);
    return 0;
}

/**
 * @return 1 - meotvoril subor
 */
GSI *gsi_create_by_pgm5(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        return NULL;
    }

    char str[256];
    if ((fscanf(file, "%s\n", &str)) == 0 || strcmp("P5",str) != 0)
    {
        fclose(file);
        return NULL;
    }

    char *line;
    size_t len;
    
    line = fgets(str, sizeof(str), file);
    if (line == NULL)
    {
        fclose(file);
        return NULL;
    }
    int width = 0;
    int height = 0;
    int maxGray = 0;

    if (str[0] == '#')
    {
        // komentar odignorujeme
        if(fscanf(file, "%d %d %d ", &width, &height, &maxGray) ==0) {
            fclose(file);
            return NULL;
        }
    }
    else
    {
        if(sscanf(line, "%d %d %d ", &width, &height, &maxGray) == 0) {
            fclose(file);
            return NULL;
        }
    }

    GSI *img = gsi_create_with_geometry(width, height);
    fread(img->px, img->height*img->width,1, file);
    if (ferror(file)) {
        gsi_destroy(img);
        fclose(file);
        return NULL;
    }
    fclose(file);
    return img;
}


GSI* gsi_create_right_rotation(GSI *to_rotate)
{
    
    GSI *rotatedGSI = gsi_create_with_geometry(to_rotate->height, to_rotate->width);
    if (rotatedGSI == NULL) {
        return NULL;
    }
    
    for (int y = 0; y < to_rotate->height; y++) {
        for (int x = 0; x < to_rotate->width; x++) {
            PIX(rotatedGSI, rotatedGSI->width-y-1, x) = PIX(to_rotate, x, y);
        }
    }
    
    return rotatedGSI;
}


GSI* gsi_create_left_rotation(GSI *to_rotate)
{  
    GSI *rotatedGSI = gsi_create_with_geometry(to_rotate->height, to_rotate->width);
    if (rotatedGSI == NULL) {
        return NULL;
    }
    
    for (int y = 0; y < to_rotate->height; y++) {
        for (int x = 0; x < to_rotate->width; x++) {
            int pos = (rotatedGSI->height-x-1) * rotatedGSI->width + y;
            PIX(rotatedGSI, y, rotatedGSI->height-x-1) = PIX(to_rotate, x,y);
        }
    }
    
    return rotatedGSI;
}




int main()
{
    GSI *obrazok2 = gsi_create_by_pgm5("raster_obrazok3.pgm");
    if(obrazok2 == NULL) {
        fprintf(stderr, "Obrazok sa nenacital!");
        return 1;
    }
    if (PIX(obrazok2, 0, 0) == 255)
    {
        printf("nacital som obrazok");
    }
    //GSI *obrazok2_otoceny = gsi_create_right_rotation(obrazok2);
    GSI *obrazok2_otoceny = gsi_create_left_rotation(obrazok2);
    gsi_save_as_pgm5(obrazok2_otoceny,"obrazok2_otoceny.pgm","skuska");
    gsi_destroy(obrazok2_otoceny);
    gsi_destroy(obrazok2);

    GSI *gsi = gsi_create_with_geometry_and_color(2, 2, 0);
    if (PIX(gsi, 0, 0) == 0)
    {
        printf("is empty");
    }

    char res = gsi_save_as_pgm5(gsi, "raster_obrazok.pgm", "skuska");
  

    gsi_destroy(gsi);
    

    return 0;
}
