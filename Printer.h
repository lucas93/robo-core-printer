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
#include "SDCardReader.h"

using namespace hSensors;

class Printer
{
private:
    ProcessedImage image;
    String imageDataFileName = "image.txt";
    SDCardReader sdReader;

    RegulatedMotor<2> mX;
    RegulatedMotor<1> mY;
    RegulatedMotor<6> mZ;
    const int mXSpeed = 50;
    const int mYSpeed = 50;
    int mZSpeed = 100;
    Lego_Touch lTouch = Lego_Touch(hSens2);
    Lego_Touch rTouch = Lego_Touch(hSens1);

    int xCurrent = 0;
    int yCurrent = 0;
    const int WIDTH_MAX = 310;
    const int HEIGHT_MAX = 730;
    int zRotation = 600;
    int zCalibrationStep = 40;
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
        preparePrinter();

        printImage();

        console << "\nDone!\n";
    }

private:

    void preparePrinter()
    {
        calibrateY();
        calibrateX();
        calibratePen();

        displayCalibrationParameters();
    }


    void calibrateY()
    {
        mY.setSpeed(mYSpeed / 2);
        mY.start();
        console << "Press any button when Y i calibrated: ";

        ButtonManager::waitForAnyPress();

        mY.stop();
        mY.setSpeed(mYSpeed);
        yCurrent = 0;
    }

    void calibrateX()
    {
        // TODO
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

    void calibratePen()
    {
        Button button;

        do
        {
            console << newline << "Calibrating pen:" << newline
                   << "A - amplitude calibration" << newline
                   << "P - position calibration" << newline
                   << "S - speed calibration" << newline
                   << "T - try" << newline
                   << "C - accept" << newline;

            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
            case 'a':
                calibratePenAmplitude();
                break;

            case 'p':
                calibratePenPosition();
                break;
            case 's':
                calibratePenSpeed();
                break;

            case 't':
                penTest();
                break;
            }
        } while(button != 'c');
    }

    void calibratePenAmplitude()
    {
        Button button;
        console << newline << "Calibrating pen amplitude:" << newline
               << "Q - top higher" << newline
               << "A - top lower" << newline
               << "E - down higher" << newline
               << "D - down lower" << newline
               << "T - try" << newline
               << "C - accept" << newline << newline;
        do
        {
            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
                case 'q':
                    zRotation += zCalibrationStep;
                    mZ.rotate(-zCalibrationStep);
                    break;
                case 'a':
                    zRotation -= zCalibrationStep;
                    mZ.rotate(zCalibrationStep);
                    break;

                case 'e':
                    zRotation -= zCalibrationStep;
                    break;

                case 'd':
                    zRotation += zCalibrationStep;
                    break;

                case 't':
                    penTest();
                    break;

                default:
                    break;
            }
        } while(button != 'c');
    }

    void calibratePenPosition()
    {
        Button button;
        console << newline << "Calibrating pen position:" << newline
               << "W - pen higher" << newline
               << "S - pen lower" << newline
               << "T - try" << newline
               << "C - accept" << newline;

        do
        {
            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
            case 'w':
                mZ.rotate(-zCalibrationStep);
                break;

            case 's':
                mZ.rotate(zCalibrationStep);
                break;

            case 't':
                penTest();

            default:
                break;
            }
        } while(button != 'c');
    }
    void calibratePenSpeed()
    {
        auto constrain = [](int val, int min, int max) -> bool
        {
            return (val < min ? min : (val > max ? max : val));
        };

        Button button;
        console << newline << "Calibrating pen speed:" << newline
               << "W - faster" << newline
               << "S - slower" << newline
               << "T - try" << newline
               << "C - accept" << newline;

        do
        {
            button = ButtonManager::waitForAnyPress();

            switch (button)
            {
            case 'w':
                mZSpeed = constrain(mZSpeed + 5, 15, 100);
                mZ.setSpeed(mZSpeed);
                break;

            case 's':
                mZSpeed = constrain(mZSpeed - 5, 15, 100);
                mZ.setSpeed(mZSpeed);
                break;

            case 't':
                penTest();

            default:
                break;
            }
        } while(button != 'c');
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


    void pauseButtonPushed()
    {
        // TODO
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
