#include "Img.hpp"
#include <cstring>
#include <errors.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <type_traits>

#define OCCURRED(ERROR) (Meta.error & ERROR)
#define ERROR(CODE) Meta.error |= (CODE); input_file.close(); return
#define CLEAR(CODE) Meta.error &= (~(CODE))

Img::Img(string fileName) {
    Meta.file_name = fileName;
    Meta.error = CANVAS_ALLOC_ERROR | HASHTAB_ALLOC_ERROR;
    Palette.curr = 1;
    Palette.entries[0] = 0x0000;
    Palette.size = PALETTE_SIZE;
    HashTab.size = HASHTAB_SIZE;
    HashTab.indexTransparent = false;

    // Reading file header

    ifstream input_file(fileName, ios::in | ios::binary);
    if (input_file.fail()) {
        ERROR(OPEN_IMAGE_ERROR);
    }
    if (!input_file.good()) {
        ERROR(NO_DATA_ERROR);
    }
    u16 magic_bytes;
    input_file.read((char *)&magic_bytes, 2);     // 0-1      OK
    if (magic_bytes != 0x4d42) {
        ERROR(MAGIC_BYTES_ERROR);
    }
    input_file.read((char *)&Meta.file_size, 4);  // 2-5      OK
    input_file.seekg(4, input_file.cur);          // 6-9
    input_file.read((char *)&Canvas.offset, 4);   // 10-13    OK (?)
    if (!input_file.good()) {
        ERROR(BMFH_ERROR);
    }

    // Reading info header

    input_file.read((char *)&Meta.info_header_size, 4);   // 14-17    OK
    input_file.read((char *)&Canvas.width, 4);            // 18-21    OK
    input_file.read((char *)&Canvas.height, 4);           // 22-25    OK
    if (Canvas.height < 0) {
        Meta.bottom_up = false;
        Canvas.height = -Canvas.height;
    } else {
        Meta.bottom_up = true;
    }
    Canvas.size = Canvas.width * Canvas.height;
    input_file.seekg(2, input_file.cur);                   // 26-27
    input_file.read((char *)&Meta.bit_depth, 2);          // 28-29    OK
    if (Meta.bit_depth != 16) {
        ERROR(BITDEPTH_ERROR);
    }
    input_file.read((char *)&Meta.compression, 4);        // 30-33    
    if (Meta.compression != 0 && Meta.compression != 3) {
        ERROR(COMPRESSION_ERROR);
    }
    input_file.read((char*)&Meta.img_size, 4);
    if (Meta.img_size != (Canvas.size<<1)){
        ERROR(SIZE_ERROR);
    }
    if (!input_file.good()) {
        ERROR(BMIH_ERROR);
    }

    //memset(Palette.entries, 0, PALETTE_SIZE); probably not needed
    Canvas.entries = new u16[Canvas.size];
    if (!Canvas.entries) {
        ERROR(CANVAS_ALLOC_ERROR);
    } CLEAR(CANVAS_ALLOC_ERROR);

    input_file.seekg((sint)Canvas.offset);
    input_file.read((char*)Canvas.entries, Meta.img_size); //TODO: check for endian
    if(!input_file.good()) {ERROR(PARSING_DATA_ERROR);};

    // Done reading file
    input_file.close();

    HashTab.entries = new u8[HashTab.size];
    if (!HashTab.entries) {
        ERROR(HASHTAB_ALLOC_ERROR);
    } CLEAR(HASHTAB_ALLOC_ERROR);
}

Img::~Img(){
    if(!OCCURRED(CANVAS_ALLOC_ERROR)){
        delete[] Canvas.entries;
    } else if(!OCCURRED(HASHTAB_ALLOC_ERROR)){
        delete[] HashTab.entries;
    }
}

uint Img::probeHash(u16 color){
    uint base_pos = colorHash(color);
    uint final_pos = base_pos;
    for(uint i=0; i<HashTab.size; i++){
        final_pos = (base_pos + ((i+i*i)>>1))%HashTab.size;
        if(Palette.entries[HashTab.entries[final_pos]] == color || HashTab.entries[final_pos] == EMPTY){
            return final_pos;
        }
    }
    return HashTab.size; // out of bound
}

uint Img::colorHash(u16 color){
    uint ret =  RED(color);
    ret     ^= (GREEN(color)<<3);
    ret     ^= ((BLUE(color)<<6) | (BLUE(color)>>3)) &  0x1ff;
    return ret;
}

void Img::index(bool indexTransparent){
    memset(HashTab.entries, EMPTY, HashTab.size);
    for (uint i=0; i<Canvas.size; i++){
        if((!transparent(i)) || indexTransparent){
            uint buck = probeHash(Canvas.entries[i]);
            if(!(buck < HashTab.size)) {Meta.error |= PALETTE_ERROR; return;}
            if(HashTab.entries[buck] == EMPTY){
                if(!paletteAvail()) {Meta.error |= PALETTE_ERROR; return;}
                Palette.entries[Palette.curr] = Canvas.entries[i];
                HashTab.entries[buck] = Palette.curr;
                Palette.curr++;
            }
            Canvas.entries[i] = HashTab.entries[buck];
        } else Canvas.entries[i] = EMPTY;
    }
}

void Img::print(){
    cout << Meta.file_name << ": " << endl;
    cout << "\tpixel size: " << Canvas.width << "*" << Canvas.height << " - " << Canvas.size << endl;
    cout << "\timage size: " << Meta.img_size << endl;
    cout << "\tdata offset: 0x" << hex << Canvas.offset << dec << endl;
    cout << "\tfile size: " << Meta.file_size << endl;
    cout << "\t" << Meta.bit_depth << "bpp" << endl;
    cout << "\tcompression type: " << Meta.compression << endl;
    cout << "\tdirection: "<< (Meta.bottom_up ? "bottom up" : "top down") << endl;
    cout << "\theader size: " << Meta.info_header_size << endl;
    cout << "\tpalette size: " << Palette.size << endl;
    cout << "\thashtab size: " << HashTab.size << endl;
    cout << "\terror: " << hex << Meta.error << endl << endl;
}