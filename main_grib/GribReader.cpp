#include "GribReader.h"
#include <iostream>
#include <cstdint>
#include <stdexcept>

using namespace std;

GribReader::GribReader(const string& filename) {
    gribFile.open(filename, ios::binary | ios::in);
    if (!gribFile.is_open()) {
        throw runtime_error("Nie udalo sie otworzyc pliku!");
    }
    startSection0 = 0;
}

GribReader::~GribReader() {
    if (gribFile.is_open()) {
        gribFile.close();
    }
}

void GribReader::findGribHeader() {
    char buffer[4];
    while (!gribFile.eof()) {
        gribFile.read(buffer, 1);
        if (buffer[0] == 'G') {
            gribFile.read(buffer + 1, 3);
            if (buffer[1] == 'R' && buffer[2] == 'I' && buffer[3] == 'B') {
                startSection0 = static_cast<uint32_t>(gribFile.tellg()) - 4;
                return;
            }
        }
    }
    if (gribFile.eof()) {
        throw runtime_error("Nie znaleziono naglowka GRIB.");
    }
}


void  GribReader::readMessageDetails() {
    char buffer[3];
   

    // Sekcja 1
    gribFile.seekg(startSection0 + 8);
    gribFile.read(buffer, 3);
    section1Length = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);
    cout << "Dlugosc sekcji 1: " << section1Length << "\n";

    // Sekcja 2
    gribFile.seekg(startSection0 + 8 + section1Length);
    gribFile.read(buffer, 3);
    section2Length = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);
    cout << "Dlugosc sekcji 2: " << section2Length << "\n";

    // Sekcja 3
    gribFile.seekg(startSection0 + 8 + section1Length + section2Length);
    gribFile.read(buffer, 3);
    section3Length = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);
    cout << "Dlugosc sekcji 3: " << section3Length << "\n";

    // Sekcja 4
    gribFile.seekg(startSection0 + 8 + section1Length + section2Length + section3Length);
    gribFile.read(buffer, 3);
    section4Length = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);
    cout << "Dlugosc sekcji 4: " << section4Length << "\n";

    // Sekcja 5
    gribFile.seekg(startSection0 + 8 + section1Length + section2Length + section3Length + section4Length);
    gribFile.read(buffer, 3);
    section5Length = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);
    cout << "Dlugosc sekcji 5: " << section5Length << "\n";


    gribFile.seekg(startSection0);
    string endMarker;
    uint32_t position7777 = 0;
    while (!gribFile.eof()) {
        gribFile.read(buffer, 1);
        endMarker.push_back(buffer[0]);
        if (endMarker.size() > 4) {
            endMarker.erase(0, 1);
        }
        if (endMarker == "7777") {
            position7777 = static_cast<uint32_t>(gribFile.tellg()) - 4;
            break;
        }
    }

    if (position7777 == 0) {
        cout << "Nie znaleziono znacznika '7777'.\n";
        return;
    }

    cout << "==========================" << endl;
    cout << "==  General Message Info  ==" << endl;
    cout << "==========================" << endl;

    cout << "Grib Finda at pos: " << startSection0 << "\n";

    cout << "7777 Find at position: " << position7777 << "\n";

    // Obliczanie odleg³oœci
    uint32_t distanceBetweenEndOfGribAnd7777 = position7777 - (startSection0 + 8);
    cout << "Distance between end of GRIB and start of 7777: " << distanceBetweenEndOfGribAnd7777 << "\n";

    // Ca³kowita d³ugoœæ wiadomoœci
    uint32_t wholeMessageLength = position7777 - startSection0;
    cout << "Whole Message length: " << wholeMessageLength << "\n";

}





void GribReader::readSection1() {


    cout << "==========================" << endl;
    cout << "==  Section 1 Data  ==" << endl;
    cout << "==========================" << endl;
    cout << "Section length: " << section1Length << endl;


    unsigned char year, month, day, hour, minute, high_byte, low_byte, level_or_layer, unit_parameters, GDS_And_BMS, grid_Identification, process_id_number, 
        identification_of_Centre, parameter_Table_Version, time_Unit, p1_Period_Of_Time, p2_Period_Of_Time, time_Range_Indicator, missing_Averages_Or_Occumulations, 
        century_Of_Initial, identification_Of_Sub_Center, decimal_Scale_factor_1, decimal_Scale_factor_2;

    gribFile.seekg(startSection0 + 11);




    gribFile.read(reinterpret_cast<char*>(&parameter_Table_Version), 1);

    gribFile.read(reinterpret_cast<char*>(&identification_of_Centre), 1);

    gribFile.read(reinterpret_cast<char*>(&process_id_number), 1);

    gribFile.read(reinterpret_cast<char*>(&grid_Identification), 1);

    gribFile.read(reinterpret_cast<char*>(&GDS_And_BMS), 1);

    gribFile.read(reinterpret_cast<char*>(&unit_parameters), 1);

    gribFile.read(reinterpret_cast<char*>(&level_or_layer), 1);


    gribFile.read(reinterpret_cast<char*>(&high_byte), 1);
    // Czytanie ni¿szego bajtu
    gribFile.read(reinterpret_cast<char*>(&low_byte), 1);

    // £¹czenie bajtów w wartoœæ 16-bitow¹, uwzglêdniaj¹c big-endian
    uint16_t height_pressure_etc = (high_byte << 8) | low_byte;

    

    gribFile.read(reinterpret_cast<char*>(&year), 1);
    gribFile.read(reinterpret_cast<char*>(&month), 1);
    gribFile.read(reinterpret_cast<char*>(&day), 1);
    gribFile.read(reinterpret_cast<char*>(&hour), 1);
    gribFile.read(reinterpret_cast<char*>(&minute), 1);



    gribFile.read(reinterpret_cast<char*>(&time_Unit), 1);
    gribFile.read(reinterpret_cast<char*>(&p1_Period_Of_Time), 1);
    gribFile.read(reinterpret_cast<char*>(&p2_Period_Of_Time), 1);
    gribFile.read(reinterpret_cast<char*>(&time_Range_Indicator), 1);

    gribFile.seekg(startSection0 + 31);


    gribFile.read(reinterpret_cast<char*>(&missing_Averages_Or_Occumulations), 1);
    gribFile.read(reinterpret_cast<char*>(&century_Of_Initial), 1);
    gribFile.read(reinterpret_cast<char*>(&identification_Of_Sub_Center), 1);
    gribFile.read(reinterpret_cast<char*>(&decimal_Scale_factor_1), 1);
    gribFile.read(reinterpret_cast<char*>(&decimal_Scale_factor_2), 1);

    uint16_t decimal_Scale_factor = (decimal_Scale_factor_1 << 8) | decimal_Scale_factor_2;




    cout << "Parameter Table Version: " << static_cast<int>(parameter_Table_Version) << endl;
    cout << "Identification of Centre: " << static_cast<int>(identification_of_Centre) << endl;
    cout << "process id number: " << static_cast<int>(process_id_number) << endl;
    cout << "Grid Identification: " << static_cast<int>(grid_Identification) << endl;
    cout << "GDS and BMS " << static_cast<int>(GDS_And_BMS) << endl;
    cout << "Unit parameters: " << static_cast<int>(unit_parameters) << endl;
    cout << "Indicator of type of level or layer: " << static_cast<int>(level_or_layer) << endl;
    cout << "Height, pressure, etc. of the level or layer: " << height_pressure_etc << endl;
    cout << "Rok: " << 1900 + year << "\n";
    cout << "Miesiac: " << static_cast<int>(month) << "\n";
    cout << "Dzien: " << static_cast<int>(day) << "\n";
    cout << "Godzina: " << static_cast<int>(hour) << "\n";
    cout << "Minuta: " << static_cast<int>(minute) << "\n";


    cout << "Time unit: " << static_cast<int>(time_Unit) << "\n";
    cout << "P1 - Period of time: " << static_cast<int>(p1_Period_Of_Time) << "\n";
    cout << "P2 - Period of time: " << static_cast<int>(p2_Period_Of_Time) << "\n";
    cout << "Time range indicator : " << static_cast<int>(time_Range_Indicator) << "\n";
    cout << "Number Missing from averages or accumulations: " << static_cast<int>(missing_Averages_Or_Occumulations) << "\n";
    cout << "Reference century: " << static_cast<int>(century_Of_Initial) << "\n";
    cout << "Identification of sub Center: " << static_cast<int>(identification_Of_Sub_Center) << "\n";
    cout << "Decimal Scale factor: " << static_cast<int>(decimal_Scale_factor) << "\n";




    cout << "///" << endl;
}
void GribReader::readSection2() {


    uint16_t latitude_Of_Last_1, Longtitude, Latitude, Latitude_show, Longtitude_show;

    cout << "==========================" << endl;
    cout << "==  Section 2 Data  ==" << endl;
    cout << "==========================" << endl;
    cout << "Section length: " << section2Length << endl;

    unsigned char vertical_Coordinates, octet_number, data_Representation_Type, latitude_Circle_1, latitude_Circle_2, longitude_Meridian_1, longitude_Meridian_2, resolution_And_Component_Flags,
        where_1, where_2;

    char buffer_3[3];
    char buffer_2[2];


    // Sekcja 
    gribFile.seekg(startSection0 + 8 + section1Length + 3);
    //gribFile.read(buffer, 3);



   /*costam = (static_cast<unsigned char>(buffer[0]) << 16) |
        (static_cast<unsigned char>(buffer[1]) << 8) |
        static_cast<unsigned char>(buffer[2]);

        */


        //gribFile.read(reinterpret_cast<char*>(&cos), 1);

        //cout << "!!!!!!!!!!!!!!!!!Dlugosc powinno byc 178 2: !!!!!!!!!!!!!!!!!" << costam << "!!!!!" << "\n";


    gribFile.read(reinterpret_cast<char*>(&vertical_Coordinates), 1);
    gribFile.read(reinterpret_cast<char*>(&octet_number), 1);
    gribFile.read(reinterpret_cast<char*>(&data_Representation_Type), 1);
    gribFile.read(reinterpret_cast<char*>(&latitude_Circle_1), 1);
    gribFile.read(reinterpret_cast<char*>(&latitude_Circle_2), 1);
    gribFile.read(reinterpret_cast<char*>(&longitude_Meridian_1), 1);
    gribFile.read(reinterpret_cast<char*>(&longitude_Meridian_2), 1);





    uint16_t latitude_Circle = (latitude_Circle_1 << 8) | latitude_Circle_2;
    uint16_t longitude_Meridian = (longitude_Meridian_1 << 8) | longitude_Meridian_2;

    gribFile.read(reinterpret_cast<char*>(&where_1), 1);

    gribFile.read(buffer_2, 2);

    Latitude = (static_cast<unsigned char>(buffer_2[0]) << 8) |
        static_cast<unsigned char>(buffer_2[1]);


    gribFile.read(reinterpret_cast<char*>(&where_2), 1);

    gribFile.read(buffer_2, 2);

    Longtitude = (static_cast<unsigned char>(buffer_2[0]) << 8) |
        static_cast<unsigned char>(buffer_2[1]);

    gribFile.read(reinterpret_cast<char*>(&resolution_And_Component_Flags), 1);


    gribFile.read(buffer_2, 2);

    latitude_Of_Last_1 = (static_cast<unsigned char>(buffer_2[0]) << 8) |
        static_cast<unsigned char>(buffer_2[1]);


    cout << "Number of Vertical Coordinates: " << static_cast<int>(vertical_Coordinates) << "\n";
    cout << "Octet number: " << static_cast<int>(octet_number) << "\n";
    cout << "Data representation type " << static_cast<int>(data_Representation_Type) << "\n";
    cout << "umbers of points along a latitude circle:" << static_cast<int>(latitude_Circle) << "\n";
    cout << "umbers of points along a latitude circle:" << static_cast<int>(longitude_Meridian) << "\n";

    if (Latitude != 0)
    {
        Latitude_show = Latitude / 1000;
    }
    else
    {
        Latitude_show = Latitude;
    }

    if (static_cast<int>(where_1) & 128) {
        cout << "Latitude: -" << Latitude_show << " degrees" << "\n";
    }
    else {
        cout << "Latitude: " << Latitude_show << " degrees" << "\n";
    }

    


    if (Longtitude != 0)
    {
        Longtitude_show = Longtitude / 1000;
    }
    else
    {
        Longtitude_show = Longtitude;
    }


    if (static_cast<int>(where_2) & 128) {
        cout << "Longtitude: -" << Longtitude_show << " degrees" << "\n";
    }
    else {
        cout << "Longtitude: " << "huj" << Longtitude_show << " degrees" << "\n";
    }

    
    //cout << "Resolution and component flags: " << static_cast<int>(resolution_And_Component_Flags) << "\n";
    

    bool direction_Increments_Given = resolution_And_Component_Flags & 0x01; // Bit 1
    bool earth_Spheroid = resolution_And_Component_Flags & 0x02; // Bit 2
    bool vector_Components = resolution_And_Component_Flags & 0x20; // Bit 5 (zaczynamy liczenie od 0, wiêc bit 5 to maska 0x20)

    cout << "Direction increments given: " << (direction_Increments_Given ? "Yes" : "No") << "\n";
    cout << "Earth model: " << (earth_Spheroid ? "Oblate spheroid defined by IAU in 1965" : "Spherical, radius = 6367.47 km") << "\n";
    cout << "Vector components relative to: " << (vector_Components ? "a specified grid in the direction of increasing x and y (or i and j) coordinates" : "east and north directions") << "\n";

    //cout << "Latitude of last grid point: " << static_cast<int>(latitude_Of_Last_1) << "\n";

}

void GribReader::readSection3() {
    cout << "Dlugosc sekcji 3: " << section3Length << "\n";
}