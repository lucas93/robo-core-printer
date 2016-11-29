#ifndef ROBOCOREPRINTERFRONT_SDCARDREADER_H
#define ROBOCOREPRINTERFRONT_SDCARDREADER_H

#include <string>
#include<cctype>
using namespace std;



namespace SDCardReader
{
    using Int = int16_t ;
    hFile file;
    bool prepared = false;

    void prepareImageFileToLoad(const string& RoboCoreImageFileName = "RoboCoreImage.txt")
    {
        mountSD();
        openFile(RoboCoreImageFileName);
        prepared = true;
    }

    void prepareIfNotPrepared()
    {
        if( !prepared )
            prepareImageFileToLoad();
    }

    Int parseInt()
    {
        prepareIfNotPrepared();

        string strNum;
        char ch = getSingleChar();
        while( isdigit(ch) )
        {
            strNum += ch;
        }

        return stoi(strNum);
    }

    char getSingleChar()
    {
        char d[1];
        r = file.read(d, 1);
        if(r != 0)
        {
            console << "\ngetchar error!";
            return 0;
        }

        return d[0];
    }

    void mountSD()
    {
        int r;
        r = SD.mount();
        console << "Mounted SD : " << r << newline;
    }

    void openFile(const string& RoboCoreImageFileName)
    {
        int r = SD.openFile(file, RoboCoreImageFileName.c_str(), hFile::MODE_OPEN_EXISTING | hFile::MODE_READ);
        console << "Opened file: " << RoboCoreImageFileName << "\twith code : " << r << newline;
    }
}
#endif //ROBOCOREPRINTERFRONT_SDCARDREADER_H


