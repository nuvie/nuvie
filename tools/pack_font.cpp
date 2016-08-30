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

int main(int argc, char **argv)
{
    string name = "system.lzc";
    string output_file = "system.lzc.new";
    U6Lib_n library;
    U6Lib_n output;
    NuvieBmpFile bmpFile;
    unsigned char fontData[1024];

    if (argc != 2)
    {
        printf("Usage: %s <bmp font file>", argv[0]);
        exit(1);
    }

    if (!bmpFile.load(argv[1]))
        exit(1);

    if (library.open(name, 4, NUVIE_GAME_SE) == false)
        exit(1);

    output.create(output_file, 4, NUVIE_GAME_SE);

    for(int i=0;i<3;i++)
    {
        output.add_item(0, "");
        output.set_item_data(i, library.get_item(i, NULL), library.get_item_size(i));
    }

    packFontData(fontData, bmpFile.getRawIndexedData());
    output.add_item(0, "");
    output.set_item_data(3, fontData, sizeof(fontData));

    output.calc_item_offsets();
    output.write_header();
    output.write_index();
    output.write_items();
    output.close();
    library.close();
    return 0;
}


