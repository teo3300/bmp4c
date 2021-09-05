#include "Img.hpp"
#include <cstdio>

int main(int argc, char* argv[]){
    if (argc < 2) return 1;
    Img bitmap (argv[1]),
        bitmap2(argv[1]);
    //bitmap.print();
    //bitmap2.print();
    bitmap.index();


    //checking for integrity

    for(uint i = 0; i<bitmap.size(); i++){
        if(bitmap.paletteEntry(bitmap.pixel(i)) != bitmap2.pixel(i)){
            cout << "data loss: 0x" << hex << bitmap.paletteEntry(bitmap.pixel(i)) << " should be 0x" << bitmap2.pixel(i) << endl;
        }
    }
    cout << "Palette entries uses: " << dec << bitmap.paletteSize() << endl;
}