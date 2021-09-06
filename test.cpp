#include "Img.hpp"
#include <cstdio>

int main(int argc, char* argv[]){
    if (argc < 2) return 1;
    Img bitmap (argv[1]);
    if(bitmap.error()) return 1;
    bitmap.index(FULL_PALETTE);
    bitmap.print();
    //for (uint i=0; i<bitmap.size(); i++){
        /*cout << "Pixel: " << dec << i+1 << " of " << bitmap.size() << endl;
        cout << "\tR: 0x" << hex << bitmap.red(i);
        cout << "\tG: 0x" <<             bitmap.green(i);
        cout << "\tB: 0x" <<             bitmap.blue(i) << endl;
        cout << "\tColor: 0x" <<         bitmap.pixel(i) << std::dec;
        cout << "\tTransparent: " << (bool)bitmap.transparent(i) << endl;
        cout << hex << "Hash of: 0x" << bitmap.pixel(i) << " is 0x" << bitmap.askHash(bitmap.pixel(i)) << endl;*/
    //    cout << dec << "pixel: " << i << hex << " index 0x" << bitmap.pixel(i) << " was 0x" << bitmap.paletteEntry(bitmap.pixel(i)) << endl;
    //}
    /*cout << endl << dec << "Palette entries available: " << bitmap.paletteAvail() << hex << endl << "Palette entries used: " << endl;
    for(uint i=0; i<bitmap.paletteSize(); i++){
        cout << "0x" << bitmap.paletteEntry(i) << (((i+1)%5) ? "\t" : "\n");
    }
    cout << endl;*/
    
}