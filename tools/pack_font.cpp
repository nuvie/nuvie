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

void writeChar(unsigned char *dest, unsigned char *bmp_data)
{
    for(int y=0;y<8;y++)
    {
        dest[y] = 0;
        for(int x=0;x<8;x++)
        {
            if(bmp_data[x])
                dest[y] |= (uint8)(1 << (7-x));
        }
        bmp_data += 128;
    }
}

void packFontData(unsigned char *dest, unsigned char *bmp_data)
{
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<16;x++)
        {
            writeChar(dest, &bmp_data[y*16*8*8 + x*8]);
            dest += 8;
        }
    }
}

void saveU6Font(NuvieBmpFile *bmpFile)
{
    unsigned char fontData[2048];
    NuvieIOFileWrite u6ch;
    unsigned char *pixels = bmpFile->getRawIndexedData();

    if (bmpFile->getWidth() != 128 || bmpFile->getHeight() != 128)
    {
        printf("bmp file must be 128x128 pixels\n");
        return;
    }

    u6ch.open("u6.ch");
    u6ch.seekStart();

    packFontData(fontData, pixels);

    pixels += (16 * 8 * 8 * 8);
    packFontData(&fontData[1024], pixels);

    u6ch.writeBuf(fontData, sizeof(fontData));

    u6ch.close();
}

void saveWOUFont(NuvieBmpFile *bmpFile)
{
    string name = "system.lzc";
    string outputFile = "system.lzc.new";
    unsigned char fontData[1024];
    U6Lib_n library;
    U6Lib_n output;

    if (bmpFile->getWidth() != 128 || bmpFile->getHeight() != 64)
    {
        printf("bmp file must be 128x64 pixels\n");
        return;
    }

    if (library.open(name, 4, NUVIE_GAME_SE) == false)
        return;

    output.create(outputFile, 4, NUVIE_GAME_SE);

    for(uint32 i=0;i<3;i++)
    {
        output.add_item(0, "");
        output.set_item_data(i, library.get_item(i, NULL), library.get_item_size(i));
    }

    packFontData(fontData, bmpFile->getRawIndexedData());
    output.add_item(0, "");
    output.set_item_data(3, fontData, sizeof(fontData));

    output.calc_item_offsets();
    output.write_header();
    output.write_index();
    output.write_items();
    output.close();
    library.close();
}

int main(int argc, char **argv)
{
    NuvieBmpFile bmpFile;

    if (argc != 3 || (strcmp(argv[1], "md") != 0 && strcmp(argv[1], "se") != 0 && strcmp(argv[1], "u6") != 0))
    {
        printf("Usage: %s [u6,se,md] bmp_font_file", argv[0]);
        exit(1);
    }

    if (!bmpFile.load(argv[2]))
    {
        printf("Failed to open BMP file: '%s'\n", argv[2]);
        exit(1);
    }

    if (argv[1][0] == 'u')
    {
        saveU6Font(&bmpFile);
    } else
    {
        saveWOUFont(&bmpFile);
    }

    return 0;
}


