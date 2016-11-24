#ifndef ROBOCOREPRINTERFRONT_PRINTER_H
#define ROBOCOREPRINTERFRONT_PRINTER_H

#include "Image.h"
#include "Motor.h"
#include "TouchSensor.h"
#include "SerialDisplay.h"
#include "ControlButtons.h"
#include "System.h"

class Printer
{
private:
    Image image;
    String imageDataFileName = "imageData.img";

    RegulatedMotor mX, mY, mZ;
    const int mXSpeed = 200;
    const int mYSpeed = 80;
    const int mZSpeed = 600;
    const TouchSensor lTouch;
    const TouchSensor rTouch;

    int xCurrent = 0;
    int yCurrent = 0;
    const int WIDTH_MAX = 310;
    const int HEIGHT_MAX = 730;
    int zRotation = 135;
    int zCalibrationStep = 5;
    const int xReverseConstant = 46;

    bool isXCalibrated = false;
    bool isYCalibrated = false;

    bool isPauseButtonPushed = false;

    const PIX_ROTATION = 20; // elemental servo rotation for 1 pixel

public:

    Printer()
    {
        mX.setSpeed(mXSpeed);
        mY.setSpeed(mYSpeed);
        mZ.setSpeed(mZSpeed);
        // TODO optional - prepare menu handling

        loadImage();

        Serial << "Loaded image data";
    }

    void start()
    {
        prepareEV3();

        printImage();

        Serial << "\nDone!\n";
    }

private:

    void prepareEV3()
    {
        calibratePen();
        calibrateX();
        calibrateY();
    }

    void printImage()
    {
        bool ok = true;

        for (auto & r : image)
        {
            if (isPauseButtonPushed)
                pauseButtonPushed();

            isPauseButtonPushed = false;

            showStats();

            for(auto & p : r)
            {
                PenDown();
                ok = moveX(p.length());

                if (!ok)
                {
                    PenUp();
                    break;
                }

                PenUp();
                ok = moveX(-p.spaceBefore(), 1.3);
                if (!ok)
                    break;
            }
            if (!ok)
                break;
            moveY(1, true);
        }
    }

    void calibratePen(bool shouldCalibrateX = true)
    {
        Button button;

        do {
            Serial << "UP - amplitude calibrate" << newline
                   << "DOWN - position calibrate" << newline
                   << "LEFT - step calibrate" << newline
                   << "ENTER - accept" << newline
                   << "ESCAPE - try and accept";

            button = waitForAnyPress();

            switch (button) {
            case Button::ID_UP:
                calibratePenAmplitude();
                break;
            case Button::ID_DOWN:
                calibratePenPosition();
                break;
            case Button::ID_LEFT:
                calibratePenCalibrationStep();
                break;
            case Button::ID_ENTER:
                break;
            case Button::ID_ESCAPE:
                penTest();
                break;
            }
        } while(button != Button::ID_ENTER);

        Serial.clear();

        if(shouldCalibrateX)
            calibrateX();
    }

    void calibratePenAmplitude()
    {
        Button button;

        do {
            Serial << "UP - top higher" << newline
                   << "LEFT - top lower" << newline
                   << "DOWN - down lower" << newline
                   << "RIGHT - down higher" << newline
                   << "ENTER - accept" << newline
                   << "ESCAPE - try";

            button = waitForAnyPress();

            switch (button)
            {
                case Button::ID_UP:
                    zRotation += zCalibrationStep;
                    mZ.rotate(-zCalibrationStep);
                    break;
                case Button::ID_LEFT:
                    zRotation -= zCalibrationStep;
                    mZ.rotate(zCalibrationStep);
                    break;
                case Button::ID_DOWN:
                    zRotation += zCalibrationStep;
                    break;
                case Button::ID_RIGHT:
                    zRotation -= zCalibrationStep;
                    break;
                case Button::ID_ESCAPE:
                    penTest();
                    break;
                default:
                    break;
            }
        } while(button != Button::ID_ENTER);
    }

    void calibratePenPosition()
    {
        Button button;

        do {
            Serial << "UP - pen higher" << newline
                   << "LEFT - pen much higher" << newline
                   << "DOWN - pen lower" << newline
                   << "RIGHT - pen much lower" << newline
                   << "ENTER - accept" << newline
                   << "ESCAPE - try";

            button = waitForAnyPress();

            switch (button)
            {
                case Button::ID_UP:
                    mZ.rotate(-zCalibrationStep);
                    break;
                case Button::ID_LEFT:
                    mZ.rotate(-4 * zCalibrationStep);
                    break;
                case Button::ID_DOWN:
                    mZ.rotate(zCalibrationStep);
                    break;
                case Button::ID_RIGHT:
                    mZ.rotate(4 * zCalibrationStep);
                    break;
                case Button::ID_ESCAPE:
                    penTest();
                default:
                    break;
            }
        } while(button != Button::ID_ENTER);
    }

    void calibratePenCalibrationStep()
    {
        // TODO optional
    }

    void penTest()
    {
        PenDown();
        PenUp();
    }

    void PenDown()
    {
        mZ.rotate(zRotation);
    }

    void PenUp()
    {
        mZ.rotate(-zRotation);
    }

    bool moveX(int distance, double speedMultiper = 1.0)
    {
        if(speedMultiper < 0.0)
            speedMultiper = -speedMultiper;

        if (xCurrent + distance > WIDTH_MAX || xCurrent + distance < 0)
        {
            Serial << "WIDTH_MAX reached!" << newline
                   << "xCurrent = " << xCurrent << newline
                   <<  "distance = " << distance << newline;

            syst.waitMS(2000);

            while(true);
        }

        mX.setSpeed((int) (mXSpeed * speedMultiper));
        mX.rotate(distance * PIX_ROTATION, true);

        while (mX.isMoving())
        {
            if(rTouch.isPushed() or lTouch.isPushed())
            {
                mX.stop();
                Serial << "OUT OF BOUNDRIES!";
                while(true);
            }
        }

        xCurrent += distance;
        mX.setSpeed(mXSpeed);
        return true;
    }

    void moveY(int distance, bool immediateReturn = false)
    {
        if (yCurrent + distance > HEIGHT_MAX || yCurrent + distance < 0)
        {
            Serial << "WIDTH_MAX reached!" << newline
                   << "xCurrent = " << xCurrent << newline
                   <<  "distance = " << distance << newline;

            syst.waitMS(2000);

            while(true);
        }

        yCurrent += distance;
        mY.rotate(-distance * PIX_ROTATION, immediateReturn);
    }

    void calibrateX()
    {
        // TODO
    }

    void calibrateY()
    {
        const int distance = 10000;

        mY.setSpeed(mYSpeed / 2);
        mY.rotate(distance * PIX_ROTATION, true);

        while (mY.isMoving())
        {
            if(rTouch.isPushed())
            {
                mY.stop();
                mY.setSpeed(mYSpeed);
                yCurrent = 0;
            }
        }
    }

    void pauseButtonPushed()
    {
        // TODO
    }

    void showStats()
    {

    }

    void loadImage()
    {
        // TODO
    }

};

#endif //ROBOCOREPRINTERFRONT_PRINTER_H
