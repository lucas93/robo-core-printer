#ifndef ROBOCOREPRINTERFRONT_TOUCHSENSOR_H
#define ROBOCOREPRINTERFRONT_TOUCHSENSOR_H

template<int portNumber>
struct TouchSensor
{
    static_assert( portNumber > 0 and portNumber <= 5  , "Invalid portNumber!");
    TouchSensor & operator=(const TouchSensor&) = delete;
    TouchSensor(const TouchSensor&) = delete;

    bool isPushed() const
    {
        // TODO
        return false;
    }
};

#endif //ROBOCOREPRINTERFRONT_TOUCHSENSOR_H
