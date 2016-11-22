#ifndef ROBOCOREPRINTERFRONT_PRINTER_H
#define ROBOCOREPRINTERFRONT_PRINTER_H

#include "Image.h"
#include "Motor.h"
#include "TouchSensor.h"
#include "SerialDisplay.h"

class Printer
{
private:
    Image image;
    String imageDataFileName = "imageData.img";

    const RegulatedMotor mX, mY, mZ;
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
        for (auto & r : image)
        {
            if (isPauseButtonPushed)
            {
                pauseButtonPushed();
            }
            isPauseButtonPushed = false;

            showStats();
        }
    }

    void calibratePen()
    {

    }

    void calibrateX()
    {

    }

    void calibrateY()
    {

    }

    void pauseButtonPushed()
    {

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
