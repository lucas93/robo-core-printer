#ifndef ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
#define ROBOCOREPRINTERFRONT_SERIALDISPLAY_H


#include <string>
#include <vector>
#include "ConvertedImage.h"

#ifdef __HCLOUDCLIENT_H__

#define PRINT platform.ui.console("cl1").printf

#endif // #ifdef __HCLOUDCLIENT_H__


#ifndef __HCLOUDCLIENT_H__ // #ifdef __ISENSOR_H__

#define PRINT Serial.printf

#endif // #ifndef __HCLOUDCLIENT_H__

struct SerialDisplay
{
    void print(const char * str)
    {
        PRINT("%s", str);
    }

    void print(const std::string & str)
    {
        PRINT("%s", str.c_str());
    }

    void print(int val)
    {
        PRINT("%d", val);
    }

    void print(long int val)
    {
        PRINT("%d", val);
    }

    void print(long unsigned int val)
    {
        PRINT("%u", val);
    }

    void print(unsigned int val)
    {
        PRINT("%u", val);
    }

    void print(double val)
    {
        PRINT("%f", val);
    }

    void print(ConvertedImage image)
    {
        print(image.size());
        print(" ");
        for(const auto& row : image)
            print(row);
    }

    void print(Row row)
    {
        print(row.size());
        print(" ");
        for(const auto& line : row)
            print(line);
    }

    void print(Line line)
    {
        print(line.a);
        print(" ");
        print(line.b);
        print(" ");
    }

    void clear()
    {

    }
} console;


template <typename T>
SerialDisplay& operator<< (SerialDisplay& serial, T&& string_to_print)
{
    serial.print(string_to_print);
    return serial;
}

const char newline[] = "\r\n";

#endif //ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
