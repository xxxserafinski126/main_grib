#include "GribReader.h"
#include <iostream>

using namespace std;

int main() {
    try {
        GribReader reader("all.grib");
        reader.findGribHeader();
        reader.readMessageDetails();
        reader.readSection1();
        reader.readSection2();
        reader.readSection3();
    }
    catch (const exception& e) {
        cout << e.what() << "\n";
        return 1;
    }
    return 0;
}