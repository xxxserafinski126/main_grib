#ifndef GRIBREADER_H
#define GRIBREADER_H

#include <fstream>
#include <string>

class GribReader {
private:
    std::fstream gribFile;
    uint32_t startSection0;
    uint32_t section1Length, section2Length, section3Length, section4Length, section5Length;

public:
    GribReader(const std::string& filename);
    ~GribReader();

    void findGribHeader();
    void readMessageDetails();
    void readSection1();
    void readSection2();
    void readSection3();
};

#endif // GRIBREADER_H