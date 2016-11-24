#ifndef ROBOCOREPRINTERFRONT_SYSTEM_H
#define ROBOCOREPRINTERFRONT_SYSTEM_H


struct System
{
    using millisecond = int;
    void waitMS(int ms)
    {

    }

    millisecond now()
    {
        return 1;
    }

} syst;

#endif //ROBOCOREPRINTERFRONT_SYSTEM_H
