#ifndef ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
#define ROBOCOREPRINTERFRONT_SERIALDISPLAY_H

struct SerialDisplay
{
    template <typename T>
    void print(T str)
    {

    }
} Serial;

template <typename T>
SerialDisplay& operator<< (SerialDisplay& serial, T str)
{
    serial.print(str);
    return serial;
}

#endif //ROBOCOREPRINTERFRONT_SERIALDISPLAY_H
