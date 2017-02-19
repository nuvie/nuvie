#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../nuvieDefs.h"
#include "../files/U6Lzw.h"
#include "../files/U6Lib_n.h"
#include "../files/NuvieIOFile.h"
#include "../files/NuvieBmpFile.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,  LPSTR lpCmdLine, int iShowCmd) {
    SDL_SetModuleHandle(GetModuleHandle(NULL));
    return main(__argc, __argv);
}
#endif

void writeChar(unsigned char *font_data, unsigned char *buf, int x, int y)
{
    unsigned char *pixels = &buf[y*1024 + x*8];
    for(int i=0;i<8;i++)
    {
        for(int j=8;j>0;j--)
        {
            if(font_data[i] & (1<<(j-1)))
                pixels[8-j] = 1; // 0th palette entry should be black
        }

        pixels += 128;
    }
}

void writeFontToBuf(unsigned char *font_data, unsigned char *buf)
{
    int fidx = 0;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            writeChar(&font_data[fidx * 8], buf, x, y);
            fidx++;
        }
    }
}

void saveFontAsBmp(unsigned char *font_data, string bmp_filename) {
    NuvieBmpFile bmp;
    unsigned char pal[256 * 4];
    memset(pal, 0, sizeof(pal));

    pal[4] = 255;
    pal[5] = 255;
    pal[6] = 255;

    bmp.initNewBlankImage(128, 64, pal);
    writeFontToBuf(font_data, bmp.getRawIndexedData());
    bmp.save(bmp_filename);
}

int main(int argc, char **argv)
{
    U6Lib_n library;
    NuvieIOFileRead u6_ch;
    unsigned char *data;
    string name;
    char gametype[3]; // 'xx\0'

    if (argc != 2 || (strcmp(argv[1], "md") != 0 && strcmp(argv[1], "se") != 0 && strcmp(argv[1], "u6") != 0))
    {
        fprintf(stderr, "Usage: %s gametype\n\ngametype = either u6, md or se\n\n", argv[0]);
        exit(1);
    }

    strcpy(gametype, argv[1]);

    switch(gametype[0])
    {
        case 'u' :
            u6_ch.open("u6.ch");
            data = u6_ch.readAll();
            saveFontAsBmp(data, string(gametype) + "_en_font.bmp");
            saveFontAsBmp(&data[128 * 8], string(gametype) + "_garg_font.bmp");
            free(data);
            u6_ch.close();
            break;
        default :
            name = "system.lzc";

            if (!library.open(name, 4, NUVIE_GAME_SE))
                exit(1);

            fprintf(stderr, "size: %d\n", library.get_item_size(3));
            data = library.get_item(3, NULL);
            saveFontAsBmp(data, string(gametype) + "_font.bmp");
            library.close();
            free(data);
            break;
    }

    exit(0);
}
