#ifndef ROBOCOREPRINTERFRONT_SDCARDREADER_H
#define ROBOCOREPRINTERFRONT_SDCARDREADER_H

#include <cstdlib>
#include <cctype>
#include <string>

#include "SerialDisplay.h"

using namespace std;



struct SDCardReader
{
    using Int = int16_t ;
    hFile file;
    bool prepared = false;

    void prepareSD(const string& RoboCoreImageFileName = "image.txt")
    {
        mountSD();
        openFile(RoboCoreImageFileName);


        prepared = true;
    }

    void prepareIfNotPrepared()
    {
        if( !prepared )
            prepareSD();
    }

    Int parseInt()
    {
        prepareIfNotPrepared();

        string strNum;
        char ch = getChar();
        while( isdigit(ch) )
        {
            strNum += ch;
            ch = getChar();
        }

        return std::atoi(strNum.c_str());
    }


    char getChar()
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


