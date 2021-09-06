#include "helper.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

#define HELP    argv[1]

#define REC_HSPLIT 4
#define REC_VSPLIT 4

#define IFNAME      argv[1]
#define OFNAME      argv[2]
#define INDEX_FORM  argv[3]
#define SPLIT       argv[4]
#define H_SPLIT     argv[5]
#define V_SPLIT     argv[6]

int main(int argc, char* argv[]){
    bool split = false;
    int hSplit = -1;
    int vSplit = -1;
    int bitDepth = 16;
    
    for(int i=0; i< argc; i++){
        cout << argv[i] << " ";
    }
    cout << endl;

    switch (argc) {
        case 7:
            vSplit = atoi(V_SPLIT);
            if(vSplit > REC_VSPLIT){
                cout << "WARNING: vSplit > " << REC_VSPLIT << ", this format is not commonly used" << endl;
            }
        case 6:
            if(string(SPLIT).compare("s")){
                cerr << "ERROR: unknown split command" << endl;
                return 1;
            }
            split=true;
            hSplit = atoi(H_SPLIT);
            if(hSplit > REC_HSPLIT){
                cout << "WARNING: hSplit > " << REC_HSPLIT << ", this format is not commonly used" << endl;
            }
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
            if(file_exists(OFNAME)){
                string backup_file_name = string(OFNAME);
                if(backup_file_name.back() == '/'){
                    cout << "ERROR: output file <" << OFNAME << "> is a directory" << endl;
                    return 1;
                }
                backup_file_name += ".bak";
                cout << "WARNING: <" << OFNAME << "> already exists, copying it to <" << OFNAME << ".bak>" << endl;
                ifstream src(OFNAME, ios::binary);
                if(!src.good()) {return 1; cerr << "ERROR: could not create backup file" << endl;}
                ofstream dst(backup_file_name, ios::binary);
                if(!dst.good()) {return 1; cerr << "ERROR: could not create backup file" << endl;}
            } if (!ofstream(OFNAME).good()){
                cout << "ERROR: unable to open file <" << OFNAME << ">" << endl;
                return 1;
            }

            break;
        default:
            printHelper();
            return 1;
    }
    cout << dec << "split: " << split << "\thSplit: " << hSplit << "\tvSplit: " << vSplit << "\tbitDepth: " << bitDepth << endl;

    return 0;
}
