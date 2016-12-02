#ifndef ROBOCOREPRINTERFRONT_IMAGE_H
#define ROBOCOREPRINTERFRONT_IMAGE_H

#include "Row.h"


#include <fstream>

using ProcessedImage = std::vector<Row>;

#ifndef __ISENSOR_H__
ostream & operator<<(ostream & ostr, const ProcessedImage & image)
{
    ostr << image.size() << "\n";
    for(const auto& row : image)
    {
        ostr << row << "\n";
    }
    return ostr;
}
#endif // #ifndef __ISENSOR_H__

#ifdef __ISENSOR_H__

#include "SDCardReader.h"

#endif // #ifdef __ISENSOR_H__

// returns reversed vector
template<typename Vector>
Vector reverse(Vector vec)
{
    Vector result;
    auto size = vec.size();
    result.reserve(size);

    for(int k = size - 1; k >= 0; --k)
        result.push_back(vec[k]);
    return result;
}




class ProcessedImageManager
{
private:
    fstream textFile;
    #ifdef __ISENSOR_H__
    SDCardReader sdReader;
    #endif // #ifdef __ISENSOR_H__

public:

    ProcessedImage loadProcessedImageFromSD(const string &filename)
    {
        #ifdef __ISENSOR_H__
        int MAX_WIDTH = sdReader.parseInt();
        int MAX_HEIGHT = sdReader.parseInt();

        int height = sdReader.parseInt();
        ProcessedImage result(height);
        for(auto & row : result)
            row = getRowFromSD();

        return result;
        #endif // #ifdef __ISENSOR_H__
    }

    ProcessedImage loadProcessedImageFromTextFile(string &filename)
    {
        textFile.open(filename.c_str());

        int HEIGHT_MAX, WIDTH_MAX, numberOfRows;

//        int numberOfCharactersInFile;
//        textFile >> numberOfCharactersInFile;

        textFile >> HEIGHT_MAX;
        textFile >> WIDTH_MAX;

        textFile >> numberOfRows;
        auto img = ProcessedImage(numberOfRows);

        for (auto & row : img)
            row = getRow();

        textFile.close();
        return img;
    }

private:
    #ifdef __ISENSOR_H__
    Row getRowFromSD()
    {
        int numberOfLines = sdReader.parseInt();
        Row row(numberOfLines);

        for (auto & line : row)
            line = getLineFromSD();

        return row;
    }

    Line getLineFromSD()
    {
        Line::point_type a = sdReader.parseInt();
        Line::point_type b = sdReader.parseInt();

        return Line{ a, b };
    }
    #endif // #ifdef __ISENSOR_H__
    Row getRow()
    {
        int numberOfLines;
        textFile >> numberOfLines;

        auto row = Row(numberOfLines);

        for (auto & line : row)
        {
            line = getLine();
        }
        return row;
    }

    Line getLine()
    {
        Line::point_type a, b;

        textFile >> a;
        textFile >> b;

        return Line{ a, b };
    }
};


#endif //ROBOCOREPRINTERFRONT_IMAGE_H
