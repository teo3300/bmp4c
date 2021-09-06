#include "helper.hpp"

#define BUILD_NAME  "main"

using namespace std;

void printHelper(){
    cout << "Accepted commands:" << endl;
    cout << BUILD_NAME << " input output "<< endl <<
            BUILD_NAME << " input output bitDepth" << endl <<
            BUILD_NAME << " input output bitDepth s" << endl <<
            BUILD_NAME << " input output bitdepth s splitSize" << endl <<
            BUILD_NAME << " input output bitdepth s hSplit vSplit" << endl <<
            endl <<
            "bitDepth:\tmust be 16 for direct pixel color, 4 or 8 for index color" << endl <<
            "s:\t\tsplit image in contiguous 8*8 chunk" << endl <<
            "splitSize:\tgroup chunks in squares of the specified size" << endl <<
            "hSplit:\t\tgroup chunks in rectangles of the specified width, maximum is " << MAX_HSPLIT << endl <<
            "vSplit:\t\tgroup chunks in rectangles of the specified height, maximum is " << MAX_VSPLIT << endl;
}

bool file_exists(string file_name){
    ifstream f(file_name); return f.good();
}