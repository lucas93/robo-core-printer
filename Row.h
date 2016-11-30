#ifndef ROBOCOREPRINTERFRONT_ROW_H
#define ROBOCOREPRINTERFRONT_ROW_H

#include "Line.h"
#include <vector>
#include "SerialDisplay.h"


using Row = std::vector<Line>;

#ifndef __ISENSOR_H__

ostream & operator<<(ostream & ostr, const Row & row)
{
    ostr << row.size() << " ";
    for(const auto& line : row)
        ostr << line << " ";

    return ostr;
}

#endif // #ifndef __ISENSOR_H__



#ifdef __ISENSOR_H__

template<>
SerialDisplay& operator<< (SerialDisplay& serial, const Row & row)
{
    serial << row.size() << "\n";
    for(const auto& line : row)
    {
        serial << line << "\n";
    }
    return serial;
}

#endif // #ifdef __ISENSOR_H__


#endif //ROBOCOREPRINTERFRONT_ROW_H
