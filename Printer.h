#ifndef ROBOCOREPRINTERFRONT_PRINTER_H
#define ROBOCOREPRINTERFRONT_PRINTER_H

#include "ProcessedImage.h"
#include "Motor.h"
//#include "TouchSensor.h"
#include <hFramework.h>
#include <Lego_Touch.h>
#include "SerialDisplay.h"
#include "ControlButtons.h"
#include "System.h"

using namespace hSensors;

class Printer
{
private:
    ProcessedImage image;
    String imageDataFileName = "imageData.img";

    RegulatedMotor<2> mX;
    RegulatedMotor<1> mY;
    RegulatedMotor<6> mZ;
    const int mXSpeed = 200;
    const int mYSpeed = 80;
    const int mZSpeed = 600;
    Lego_Touch lTouch = Lego_Touch(hSens2);
    Lego_Touch rTouch = Lego_Touch(hSens1);

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

    const int PIX_ROTATION = 20; // elemental servo rotation for 1 pixel

public:

    Printer()
    {
        mX.setSpeed(mXSpeed);
        mY.setSpeed(mYSpeed);
        mZ.setSpeed(mZSpeed);
        // TODO optional - prepare menu handling


    }

    void start()
    {
        prepareEV3();

        printImage();

        console << "\nDone!\n";
    }

private:

    void prepareEV3()
    {
        calibrateY();
        calibrateX();
        calibratePen();

        displayCalibrationParameters();
    }

    void printImage()
    {
        for (auto & r : image)
        {
            showStats();

            for(auto & p : r)
            {

            }

            //moveY(1, true);
        }
    }

    void calibratePen(bool shouldCalibrateX = true)
    {
        Button button;

        do
        {
            console << "A - amplitude calibrate" << newline
                   << "P - position calibrate" << newline
                   << "T - try" << newline
                   << "ENTER - accept";

            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
            case Button::Key_A:
                calibratePenAmplitude();
                break;

            case Button::Key_P:
                calibratePenPosition();
                break;

            case Button::Key_T:
                penTest();
                break;
            }
        } while(button != Button::Enter);
    }

    void calibratePenAmplitude()
    {
        Button button;

        do
        {
            console << "Q - top higher" << newline
                   << "A - top lower" << newline
                   << "E - down higher" << newline
                   << "D - down lower" << newline
                   << "T - try" << newline
                   << "ENTER - accept";

            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
                case Button::Key_Q:
                    zRotation += zCalibrationStep;
                    mZ.rotate(-zCalibrationStep);
                    break;
                case Button::Key_A:
                    zRotation -= zCalibrationStep;
                    mZ.rotate(zCalibrationStep);
                    break;

                case Button::Key_E:
                    zRotation -= zCalibrationStep;
                    break;

                case Button::Key_D:
                    zRotation += zCalibrationStep;
                    break;

                case Button::Key_T:
                    penTest();
                    break;

                default:
                    break;
            }
        } while(button != Button::Enter);
    }

    void calibratePenPosition()
    {
        Button button;

        do
        {
            console << "UP - pen higher" << newline
                   << "DOWN - pen lower" << newline
                   << "ENTER - accept" << newline
                   << "T - try";

            switch (button)
            {
                case Button::Up:
                    mZ.rotate(-zCalibrationStep);
                    break;

                case Button::Down:
                    mZ.rotate(zCalibrationStep);
                    break;

            case Button::Key_T:
                penTest();

            default:
                break;
            }
        } while(button != Button::Enter);
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
            console << "WIDTH_MAX reached!" << newline
                   << "xCurrent = " << xCurrent << newline
                   <<  "distance = " << distance << newline;

            syst.waitMS(2000);

            while(true);
        }

        mX.setSpeed((int) (mXSpeed * speedMultiper));
        mX.rotate(distance * PIX_ROTATION, true);

        while (mX.isMoving())
        {
            if(rTouch.isPressed() or lTouch.isPressed())
            {
                mX.stop();
                console << "OUT OF BOUNDRIES!";
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
            console << "WIDTH_MAX reached!" << newline
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
            if(rTouch.isPressed())
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

    void displayCalibrationParameters()
    {
        console << "zRotation = " << zRotation;
    }

    void showStats()
    {

    }

    void loadImage()
    {
        console << "Loaded image data";
        // TODO
    }
};

#endif //ROBOCOREPRINTERFRONT_PRINTER_H
