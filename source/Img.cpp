#include "Img.hpp"
#include "cyntax.hpp"
#include "errors.hpp"
#include "bitmasks.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>

typedef u16 ChunkLine[LINE_SIZE];
typedef u16 Chunk[CHUNK_SIZE];

#define OCCURRED(ERROR) (Meta.error & ERROR)
#define ERROR(CODE) Meta.error |= (CODE); input_file.close(); return
#define CLEAR(CODE) Meta.error &= (~(CODE))

#define HASHTAB_MASK    (MAX_HASHTAB_SIZE-1)
#define COLORHASH(color) ((u16)( (RED(color) ^ (GREEN(color)<<3) ^ ((BLUE(color)<<6) | (BLUE(color)>>3)))&HASHTAB_MASK ))

#define DUMP_DEFINE(var_name, value)   "#define " << base_name << "_" << #var_name  << " " << (value) << endl
#define CEIL4(num) ((num)+(((num%4)+4)%4))

#define TRISWAP(a,b,tmp,size)\
{\
    memcpy(tmp, a, size);\
    memcpy(a, b, size);\
    memcpy(b, tmp, size);\
}

u16 Img::colorHash(u16 color){
    u16 ret =  GREEN(color);
    ret     ^= (u16)(BLUE(color)<<3);
    ret     ^= (u16)(((RED(color)<<6) | (RED(color)>>3)) &  HASHTAB_MASK);
    return ret;
}

Img::Img(string fileName) {
    Meta.name = fileName.substr(fileName.find_last_of("/")+1);
    Meta.name = Meta.name.substr(0, Meta.name.find_first_of("."));
    Meta.index = false;
    Meta.split = false;
    Meta.vSplit = 0;
    Meta.hSplit = 0;
    Meta.error = CANVAS_ALLOC_ERROR | HASHTAB_ALLOC_ERROR;
    Palette.size = 0;
    Palette.curr = 0;
    HashTab.collisions = 0;
    HashTab.size = MAX_HASHTAB_SIZE;

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
    if(Meta.bottom_up){
        for(sint i=Canvas.size-Canvas.width; i>(-1); i-= Canvas.width){
            input_file.read((char*)&Canvas.entries[i], Canvas.width<<1);
        }
    } else input_file.read((char*)Canvas.entries, Meta.img_size); //TODO: check for endian
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
    } if(!OCCURRED(HASHTAB_ALLOC_ERROR)){
        delete[] HashTab.entries;
    }
}

u16 Img::probeHash(u16 color){
    u16 base_pos = COLORHASH(color);
    for(uint i=0; i<HashTab.size; i++){
        u16 final_pos = (base_pos + ((i+i*i)>>1))&HASHTAB_MASK;   // quad probe
        //final_pos = (base_pos + i)&HASHTAB_MASK;              // linear probing
        if(Palette.entries[HashTab.entries[final_pos]] == color || HashTab.entries[final_pos] == EMPTY){
            return final_pos;
        }
        HashTab.collisions ++;
    }
    return HashTab.size; // out of bound
}

void Img::index(uint palette_size){
    if(Meta.index) return;
    if     (palette_size == SMALL_PALETTE) Meta.bit_depth = 4;
    else if(palette_size == FULL_PALETTE)  Meta.bit_depth = 8;
    else return;
    Palette.curr = 1;
    Palette.entries[0] = 0x7fff;
    Palette.size = palette_size;
    memset(HashTab.entries, EMPTY, HashTab.size);
    for (uint i=0; i<Canvas.size; i++){
        if((!transparent(i))){
            uint buck = probeHash(Canvas.entries[i]);
            if(!(buck < HashTab.size)) {Meta.error |= PALETTE_ERROR; return;}
            if(HashTab.entries[buck] == EMPTY){
                if(!paletteAvail()) {Meta.error |= PALETTE_ERROR; return;}
                Palette.entries[Palette.curr] = Canvas.entries[i];
                HashTab.entries[buck] = (u8)Palette.curr;
                Palette.curr++;
            }
            Canvas.entries[i] = HashTab.entries[buck];
        } else Canvas.entries[i] = 0x00;
    }
    Meta.index = true;
}

void Img::split(uint width, uint height){
    if(Meta.split) return; 
    if(!height) {if(!width) height = width = 1; else height = width;}
    if(!width) {Meta.error = ILLEGAL_SPLIT_REQ_ERROR; return;}
    if((Canvas.height % (width<<3)) || (Canvas.height % (height<<3))){
        Meta.error = SPLIT_ALIGN_ERROR;
        return;
    }

    // phase 1: convert from array of pixels to array of chunks
    uint buff_pos = 0;
    uint line_buff_width = Canvas.width>>LINE_SHIFT;
    uint line_buff_size = Canvas.size>>LINE_SHIFT;
    uint v_step = Canvas.width;
    // /\ ok
    ChunkLine   *line_canvas =    (ChunkLine*)Canvas.entries,
                *line_buffer = new ChunkLine[line_buff_size];
    for(uint i=0; i<line_buff_size; i+=v_step)                                      // hopefully it's equal to i++, j++, l++ and
        for(uint j=0; j<line_buff_width; j++)                              // [chunk_width * l + chunk_width*(i<<3) + j]
            for(uint l=0; l<v_step; l+=line_buff_width, buff_pos++)
                memcpy(&line_buffer[buff_pos], &line_canvas[i+j+l], sizeof(ChunkLine));

    // phase 2: rearrange chunks following split criteria
    uint chunk_buff_width = line_buff_width;
    uint chunk_buff_size = Canvas.size>>CHUNK_SHIFT;
    v_step = chunk_buff_width*height;
    buff_pos = 0;
    Chunk   *chunk_canvas = (Chunk*)Canvas.entries,
            *chunk_buffer = (Chunk*)line_buffer;
    for(uint i=0; i<chunk_buff_size; i+=v_step)
        for(uint j=0; j<chunk_buff_width; j+=width)
            for(uint r=0; r<v_step; r+=chunk_buff_width)
                for(uint c=0; c<width; c++, buff_pos++)
                    memcpy(&chunk_canvas[buff_pos], &chunk_buffer[i+j+r+c], sizeof(Chunk));

    delete[] line_buffer;
    Meta.hSplit = width;
    Meta.vSplit = height;
    Meta.split = true;
}

void Img::dump(string fileName){
    if(fileName.find(' ') != string::npos) {Meta.error |= OUTPUT_FILENAME_ERROR;return;}
    string full_path = fileName.substr(0, fileName.find_last_of("."));
    string base_name = full_path.substr(full_path.find_last_of("/")+1);
    ofstream output_file(fileName);
    ofstream output_header(full_path + ".h");
    if(!output_file.good()){Meta.error |= OUTPUT_EROOR; return;}
    if(!output_header.good()){Meta.error |= OUTPUT_EROOR; return;}
    const uint canvas_byte_size = ((Meta.bit_depth == 16) ? (Canvas.size<<1) : ((Meta.bit_depth == 8) ? (Canvas.size) : (Canvas.size>>1)));
    output_header <<DUMP_DEFINE(canvas_entries_size, Canvas.size) <<
                    DUMP_DEFINE(canvas_byte_size, canvas_byte_size) <<
                    DUMP_DEFINE(canvas_word_size, CEIL4((canvas_byte_size))>>2) <<
                    DUMP_DEFINE(bit_depth, Meta.bit_depth) <<
                    DUMP_DEFINE(index, Meta.index) <<
                    DUMP_DEFINE(palette_entries_size, Palette.curr) <<
                    DUMP_DEFINE(palette_byte_size, Palette.curr<<1) <<
                    DUMP_DEFINE(palette_word_size, CEIL4(Palette.curr<<1)>>2) <<
                    DUMP_DEFINE(split, Meta.split) <<
                    DUMP_DEFINE(hSplit, Meta.hSplit) <<
                    DUMP_DEFINE(vSplit, Meta.vSplit);
    uint arr_len = CEIL4(Palette.curr<<1)>>2;
    if(Meta.index) {
        output_file << "#include \"" << base_name << ".h\"" << endl;
        output_file << endl << "const unsigned int " << base_name << "_palette_data [" << dec << arr_len << "] __attribute__ ((aligned (4))) = { " << endl << hex;
        // dump palette
        for(uint i=0; i<Palette.curr; i+=2){
            uint pair = Palette.entries[i];
            char sep = ',';
            if(i+1 < Palette.curr){
                pair |= (Palette.entries[i+1] << 16);
            }
            if(i == (uint)(Palette.curr -1) || i == (uint)(Palette.curr -2)) sep = '\n';
            output_file << hex << "0x" << pair << sep << " ";
        }
        output_file << "};" << endl;
    }
    arr_len = canvas_byte_size>>2;
    output_file << "const unsigned int " << base_name << "_canvas_data [" << dec << arr_len << "] __attribute__ ((aligned (4))) = { " << endl << hex;
    if(Meta.bit_depth == 16){
        for(uint i=0; i<Canvas.size; i+=2){
            uint pair = Canvas.entries[i];
            char sep = ',';
            if(i+1 < Canvas.size){
                pair |= Canvas.entries[i+1] << 16;
            } else sep = '\n';
            output_file << hex << "0x" << pair << sep;
        }
    }
    else if(Meta.bit_depth == 8){
        for(uint i=0; i<Canvas.size; i+=4){
            uint pair = Canvas.entries[i];
                pair |= Canvas.entries[i+1]<<8;
                pair |= Canvas.entries[i+2]<<16;
                pair |= Canvas.entries[i+3]<<24;
            output_file << hex << "0x" << pair << ((i+4 < Canvas.size) ? ", " : "\n");
        }
    }
    else if(Meta.bit_depth == 4){
        for(uint i=0; i<Canvas.size; i+=8){
            uint pair = Canvas.entries[i];
                pair |= Canvas.entries[i+1]<<4;
                pair |= Canvas.entries[i+2]<<8;
                pair |= Canvas.entries[i+3]<<12;
                pair |= Canvas.entries[i+4]<<16;
                pair |= Canvas.entries[i+5]<<20;
                pair |= Canvas.entries[i+6]<<24;
                pair |= Canvas.entries[i+7]<<28;
            output_file << hex << "0x" << pair << ((i+8 < Canvas.size) ? ", " : "\n");
        }
    }
    output_file << "};" << endl;
    // dump canvas
}

void Img::print(){
    cout << Meta.name << ": " << endl;
    cout << "\tpixel size: " << dec << Canvas.width << "*" << Canvas.height << " - " << Canvas.size << endl;
    cout << "\timage size: " << Meta.img_size << endl;
    cout << "\tdata offset: 0x" << hex << Canvas.offset << dec << endl;
    cout << "\tfile size: " << Meta.file_size << endl;
    cout << "\t" << Meta.bit_depth << "bpp" << endl;
    cout << "\tcompression type: " << Meta.compression << endl;
    cout << "\tdirection: "<< (Meta.bottom_up ? "bottom up" : "top down") << endl;
    cout << "\theader size: " << Meta.info_header_size << endl;
    cout << "\tpalette size: " << Palette.size << endl;
    cout << "\tpalette entries used: " << Palette.curr << endl;
    cout << "\tpalette entries avail: " << paletteAvail() << endl;
    cout << "\thashtab size: " << HashTab.size << endl;
    cout << "\thashtab collisions: " << HashTab.collisions << endl;
    cout << "\terror: 0x" << hex << Meta.error << endl << endl;
}


u16 askHash(u16 color){
    return COLORHASH(color);
}