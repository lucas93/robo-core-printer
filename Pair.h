#ifndef ROBOCOREPRINTERFRONT_PAIR_H
#define ROBOCOREPRINTERFRONT_PAIR_H

#include <string>
using String = std::string;

struct Pair
{
    int a;
    int b;
    int aNext;
    int bPrevious;

    Pair(int a, int b) : a(a), b(b) {}

    int length()
	{
		return b - a;
	}

    int spaceAfter()
    {
        return aNext - b;
    }

    int spaceBefore()
    {
        return a - bPrevious;
    }

    String toString()
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

#endif //ROBOCOREPRINTERFRONT_PAIR_H
