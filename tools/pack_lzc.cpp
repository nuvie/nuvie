#include <stdio.h>
#include <stdlib.h>
#include <nuvieDefs.h>
#include "../files/NuvieIOFile.h"

#include "../files/U6Lib_n.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,  LPSTR lpCmdLine, int iShowCmd) {
    //SDL_SetModuleHandle(GetModuleHandle(NULL));
    return main(__argc, __argv);
}
#endif

unsigned char *readFile(const char *filename, uint32 *filesize) {
  NuvieIOFileRead file;
  if(!file.open(filename)) {
    printf("Error opening file '%s'", filename);
    return NULL;
  }

  *filesize = file.get_size();
  return file.readAll();
}

int main(int argc, char **argv) {
  U6Lib_n output;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s file.lzc first.dat [otherfiles ...]\n", argv[0]);
    exit(1);
  }

  std::string path(argv[1]);

  if(!output.create(path, 4, NUVIE_GAME_SE))
    exit(1);

  for(uint8 i=2;i<argc;i++) {
    output.add_item(0, "");
    uint32 filesize = 0;
    unsigned char *data = readFile(argv[i], &filesize);
    if(!data) {
      printf("Error reading file '%s'\n", argv[i]);
      return 1;
    }
    printf("[%02d] Adding %s Size: %d\n",i-2, argv[i], filesize);
    output.set_item_data(i-2, data, filesize);
    free(data);
  }

  output.calc_item_offsets();
  output.write_header();
  output.write_index();
  output.write_items();
  output.close();
  return 0;
}


