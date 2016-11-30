#ifndef ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
#define ROBOCOREPRINTERFRONT_SERIALDISPLAY_H


#include <string>

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
