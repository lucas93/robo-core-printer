#ifndef ROBOCOREPRINTERFRONT_LINE_H
#define ROBOCOREPRINTERFRONT_LINE_H

#include <string>
using std::ostream;
using String = std::string;

struct Line
{
    using Int = int16_t ;
    using point_type = Int;

    Int a;
    Int b;

    Line() = default;
    Line(Int a, Int b) : a(a), b(b) {}

    bool operator==(const Line& other) const
    {
        return a == other.a &&
               b == other.b;
    }

    bool isPoint()
    {
        return a == b;
    }

    Int length() const { return b - a; }

    String toString()  const
    {
        String str = "a = ";
        str += a;
        str += "  b = ";
        str += b;
        str += "  len = ";
        str += length();

        return str;
    }
};

#ifndef __ISENSOR_H__

ostream & operator<<(ostream & ostr, const Line & line)
{
    ostr << line.a << " "
         << line.b;
    return ostr;
}

#endif // #ifndef __ISENSOR_H__



#ifdef __ISENSOR_H__

#include "SerialDisplay.h"
template<>
SerialDisplay& operator<< (SerialDisplay& serial, const Line & line)
{
    serial << line.a << " "
           << line.b;
    return serial;
}

#endif // #ifdef __ISENSOR_H__

#endif //ROBOCOREPRINTERFRONT_LINE_H
