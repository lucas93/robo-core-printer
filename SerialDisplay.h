#ifndef ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
#define ROBOCOREPRINTERFRONT_SERIALDISPLAY_H

struct SerialDisplay
{
    template <typename T>
    void print(T str)
    {

    }

    void clear()
    {

    }
} Serial;

template <typename T>
SerialDisplay& operator<< (SerialDisplay& serial, T str)
{
    serial.print(str);
    return serial;
}

const char newline[] = "\n";

#endif //ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
