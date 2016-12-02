#ifndef ROBOCOREPRINTERFRONT_SDCARDREADER_H
#define ROBOCOREPRINTERFRONT_SDCARDREADER_H

#include <cstdlib>
#include <cctype>
#include <string>

#include "SerialDisplay.h"
#include "Row.h"

using namespace std;



class SDCardReader
{
    using Int = int16_t ;
    hFile file;
    bool prepared = false;

public:
    SDCardReader() = default;
    ~SDCardReader()
    {
        closeFile();
        unmountSD();
    }

    void prepareSD(const string& RoboCoreImageFileName = "image.txt")
    {
        mountSD();
        openFile(RoboCoreImageFileName);


        prepared = true;
    }

    Row parseRow()
    {
        int numberOfLines = parseInt();
        Row row(numberOfLines);

        for(auto & line : row)
            line = parseLine();

        return row;
    }

    Int parseInt()
    {
        prepareIfNotPrepared();

        string strNum;
        char ch = getSingleCharFromSD();
        while( isdigit(ch) )
        {
            strNum += ch;
            ch = getSingleCharFromSD();
        }

        return std::atoi(strNum.c_str());
    }

private:

    void prepareIfNotPrepared()
    {
        if( !prepared )
            prepareSD();
    }

    Line parseLine()
    {
        Line::point_type a = parseInt();
        Line::point_type b = parseInt();

        return Line{ a, b };
    }

    char getSingleCharFromSD()
    {
        char d[1];
        int r = file.read(d, 1);
        if (r == 1)
            return d[0];
        else
            return 0;
    }

    void mountSD()
    {
        int r;
        r = SD.mount();
        Serial.printf("Mounted SD : %d\r\n", r);
        //console << "Mounted SD : " << r << newline;
    }

    void unmountSD()
    {
        SD.unmount();
    }

    void openFile(const string& RoboCoreImageFileName)
    {
        int r = SD.openFile(file, RoboCoreImageFileName.c_str(), hFile::MODE_OPEN_EXISTING | hFile::MODE_READ);
        //console << "Opened file: " << RoboCoreImageFileName << "\twith code : " << r << newline;
        Serial.printf("Opened file: %s\t with code: %d\r\n", RoboCoreImageFileName.c_str(), r);
    }

    void closeFile()
    {
        file.close();
    }
};
#endif //ROBOCOREPRINTERFRONT_SDCARDREADER_H


