#ifndef ROBOCOREPRINTERFRONT_MOTOR_H
#define ROBOCOREPRINTERFRONT_MOTOR_H

template <int motorNumber>
struct RegulatedMotor
{
    static_assert( motorNumber > 0 and motorNumber <= 6  , "Invalid Motor!");
private:
    using speed_unit = int;
    using rotation_unit = int32_t;

    const speed_unit maxSpeed = 100;
    const speed_unit minSpeed = 0;

    mutable speed_unit speed = 50; // 0 - 100

    decltype(&hMot1) motor;

    bool reversed = false;

public:
    RegulatedMotor & operator=(const RegulatedMotor&) = delete;
    RegulatedMotor(const RegulatedMotor&) = delete;


    RegulatedMotor()
    {
        switch(motorNumber)
        {
        case 1: motor = &hMot1;
            break;
        case 2: motor = &hMot2;
            break;
        case 3: motor = &hMot3;
            break;
        case 4: motor = &hMot4;
            break;
        case 5: motor = &hMot5;
            break;
        case 6: motor = &hMot6;
            break;
        }
    }

    void setSpeed(const int speed_) const
    {
        if (speed_ > maxSpeed)
            speed = maxSpeed;
        else if (speed_ < minSpeed)
            speed = minSpeed;
        else
            speed = speed_;
    }

    int getSpeed() const { return speed; }

    void rotate(rotation_unit rotation, bool immediateReturn = false) const
    {
        if(reversed)
            rotation = -rotation;

        motor->rotRel(rotation, speed * 10, !immediateReturn);
    }

    bool isMoving(const int testDelayMS = 20) const
    {
        auto count1 = getEncoderCnt();
        sys.delay(testDelayMS);

        auto count2 = getEncoderCnt();

        return count1 != count2;
    }

    auto getEncoderCnt() const
    {
        return motor->getEncoderCnt();
    }

    void start(bool clockwise = true) const
    {
        motor->stopRegulation();
        sys.delay(10);

        if(reversed)
            clockwise = !clockwise;

        if(clockwise)
            motor->setPower(-speed * 10);
        else
            motor->setPower(speed * 10);
    }
    void stop() const
    {
        motor->stop();
    }

    void inversePolarity()
    {
        reversed = !reversed;
    }

    void setReversedPolarity(bool reversed_)
    {
        reversed = reversed_;
    }
};

#endif //ROBOCOREPRINTERFRONT_MOTOR_H
