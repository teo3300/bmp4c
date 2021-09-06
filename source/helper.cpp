#include "helper.hpp"

using namespace std;

void printHelper(){
    cout << "this should be the helper" << endl;
}

bool file_exists(string file_name){
    ifstream f(file_name); return f.good();
}