#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include "cyntax.hpp"
#include "bitmasks.hpp"

#ifndef IMG_HPP
#define IMG_HPP

using namespace std;

class Img {
    struct{
        u16 entries[MAX_PALETTE_SIZE];
        u16 curr;
        uint size;
    }Palette;
    struct{
        u8* entries;
        u16 size;
    }HashTab;
    struct{
        uint offset;
        uint width;
        sint height;
        uint size;
        u16* entries;
    }Canvas;
    struct{
        uint error;
        uint file_size;
        uint img_size;
        uint info_header_size;
        u16  bit_depth;
        bool bottom_up;
        uint compression;
        uint index;
        uint bit_index;
        string file_name;
    }Meta;
    u16 probeHash(u16 color);
    u16 colorHash(u16 color);
public:
    Img(string fileName);
    ~Img();
    void index(uint palette_size);
    void print();
    inline u16 paletteEntry(uint x) {return Palette.entries[x];};
    inline u16 pixel(uint i){return Canvas.entries[i];}
    inline u16 red(uint i){return RED(Canvas.entries[i]);}
    inline u16 green(uint i){return GREEN(Canvas.entries[i]);}
    inline u16 blue(uint i){return BLUE(Canvas.entries[i]);}
    inline bool transparent(uint i){return !(Canvas.entries[i] & ALPHA_MASK);}
    inline uint size() {return Canvas.size;};
    inline bool topDown() {return !Meta.bottom_up;};
    inline bool bottomUp() {return Meta.bottom_up;};
    inline uint paletteMaxSize() {return Palette.size;};
    inline uint paletteSize() {return Palette.curr;};
    inline uint paletteAvail() {return Palette.size - Palette.curr;};
    inline u64  error() {return Meta.error&0xffff;};

    // testing
    u16 askHash(u16 color);
};

#endif