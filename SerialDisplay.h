#ifndef ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
#define ROBOCOREPRINTERFRONT_SERIALDISPLAY_H

#include "hCloudClient.h"
#include <string>


struct SerialDisplay
{
    void print(const char * str)
    {
        platform.ui.console("cl1").printf("%s", str);
    }

    void print(const std::string & str)
    {
        platform.ui.console("cl1").printf("%s", str.c_str());
    }

    void print(int val)
    {
        platform.ui.console("cl1").printf("%d", val);
    }

    void print(long unsigned int val)
    {
        platform.ui.console("cl1").printf("%u", val);
    }

    void print(double val)
    {
        platform.ui.console("cl1").printf("%f", val);
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

const char newline[] = "\n";

#endif //ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
