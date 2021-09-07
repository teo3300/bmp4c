#include "helper.hpp"
#include "Img.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

#define HELP    argv[1]
#define IFNAME      argv[1]
#define OFNAME      argv[2]
#define INDEX_FORM  argv[3]
#define SPLIT       argv[4]
#define H_SPLIT     argv[5]
#define V_SPLIT     argv[6]

// TODO: add x/y offset (?)

int main(int argc, char* argv[]){
    bool split = false;
    uint hSplit = 0;
    uint vSplit = 0;
    int bitDepth = 16;

    switch (argc) {
        case 7:
            vSplit = atoi(V_SPLIT);
            if(vSplit)
            if(vSplit > MAX_VSPLIT){
                cerr << "ERROR: vSplit too big, maximum is " << MAX_VSPLIT << endl;
                return 1;
            }
        case 6:
            hSplit = atoi(H_SPLIT);
            if(hSplit > MAX_HSPLIT){
                cerr << "ERROR: hSplit too big, maximum is " << MAX_VSPLIT << endl;
                return 1;
            }
            if(vSplit && !hSplit){
                cerr << "ERROR: hSplit must be non-zero" << endl;
                return 1;
            }
        case 5:
            if(string(SPLIT).compare("s")){
                cerr << "ERROR: unknown split command" << endl;
                return 1;
            }
            split=true;
        case 4:
            bitDepth = atoi(INDEX_FORM);
            if(bitDepth != 4 && bitDepth!= 8 && bitDepth!= 16){
                cout << "ERROR: impossible to generate an image with a bitdepth of " << bitDepth << endl;
                return 1;
            }
        case 3:
            if(!file_exists(IFNAME)){
                cerr << "ERROR: <" << IFNAME << "> does not exists, use 'bmp4c' or 'bmp4c h' to get help" << endl;
                return 1;
            }
            break;
        default:
            printHelper();
            return 1;
    }

    Img image(IFNAME);
    if(image.error()) {cerr << "Error occurred: 0x" << hex << image.error() << endl; return 1;}
    if(bitDepth == 8){
        image.index(FULL_PALETTE);
    } else if (bitDepth == 4){
        image.index(SMALL_PALETTE);
    }
    if(image.error()) {cerr << "Error occurred: 0x" << hex << image.error() << endl; return 1;}
    if(split) image.split(hSplit, vSplit);
    if(image.error()) {cerr << "Error occurred: 0x" << hex << image.error() << endl; return 1;}
    image.print();
    image.dump(OFNAME);
    cout << dec << "split: " << split << "\thSplit: " << hSplit << "\tvSplit: " << vSplit << "\tbitDepth: " << bitDepth << endl;

    return 0;
}
