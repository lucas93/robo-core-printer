#ifndef ROBOCOREPRINTERFRONT_PRINTER_H
#define ROBOCOREPRINTERFRONT_PRINTER_H

#include <algorithm>
#include <iterator>
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
    ConvertedImage image;
    String imageDataFileName = "image.txt";
    SDCardReader sdReader;

    RegulatedMotor<2> mX;
    RegulatedMotor<1> mY;
    RegulatedMotor<6> mZ;
    const int mXSpeed = 80;
    const int mYSpeed = 50;
    int mZSpeed = 100;
    Lego_Touch rTouch = Lego_Touch(hSens2);
    Lego_Touch lTouch = Lego_Touch(hSens1);

    int xCurrent = 0;
    int yCurrent = 0;
    int rowsLeft = 0;
    Row currentRow;

    const int WIDTH_MAX = 220;
    const int HEIGHT_MAX = 270;

    int zRotation = 600;
    int zCalibrationStep = 40;

    int xDistanceFromBoundry = 1000;    // TODO

    bool isXCalibrated = false;
    bool isYCalibrated = false;

    bool isPauseButtonPushed = false;

    int PIX_ROTATION = 80; // elemental servo rotation for 1 pixel

    enum class Direction { toRight, toLeft };
    Direction directionOfXMovement;
public:

    Printer()
    {
        mX.setSpeed(mXSpeed);
        mY.setSpeed(mYSpeed);
        mZ.setSpeed(mZSpeed);

        mX.setReversedPolarity(true);
    }

    void start()
    {
        console << newline << "Press any key to start: " << newline;
        ButtonManager::waitForAnyPress();

        preparePrinter();

        printImage();

        console << newline << "Done!";
    }

private:

    void preparePrinter()
    {
        calibrateY();
        calibrateX();
        calibratePen();

        prepareImage();

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

        console << newline << "Calibrated Y axis" << newline;
    }

    void calibrateX()
    {
        mX.start(true);
        while (lTouch.isPressed() == false);
        mX.stop();

        mX.rotate(xDistanceFromBoundry);

        directionOfXMovement = Direction::toRight;
        xCurrent = 0;

        console << newline << "Calibrated X axis" << newline;
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
        console << newline << "Calibrated pen Z axis" << newline;
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

    void calibratePenAxisIfKeyPressed()
    {
        if(Serial.available() > 0)
        {
            char button = ButtonManager::waitForAnyPress();

            if(button == 'c')
            {
                calibratePen();
            }
        }
    }

    void prepareImage()
    {
       sdReader.prepareSD(imageDataFileName);
       int widthMaxFromFile = sdReader.parseInt();
       int heightMaxFromFile = sdReader.parseInt();

       checkImageDimentions (widthMaxFromFile, heightMaxFromFile);

       rowsLeft = sdReader.parseInt();
    }

    void checkImageDimentions(int width, int height)
    {
        if(  widthMaxFromFile != WIDTH_MAX
           || heightMaxFromFile != HEIGHT_MAX )
        {
            while(1)
            {
                console << newline << "Image dimentions don't match!!";
                ButtonManager::waitForAnyPress();
            }
        }
    }

    void printImage()
    {
        while(getNextRowIfAvailable() == true)
        {
            printRow();

            moveY(1);

            calibratePenAxisIfKeyPressed();
        }

    }

    bool getNextRowIfAvailable()
    {
        if (rowsLeft > 0)
        {
            --rowsLeft;
            currentRow = sdReader.parseRow();
            return true;
        }
        return false;
    }

    void printRow()
    {
        directionOfXMovement = chooseDirection();
        Row rowToDraw = currentRow;

        if(directionOfXMovement == Direction::toLeft)
            rowToDraw = reverse(currentRow);

        for(auto & line : rowToDraw)
        {
            auto distance = distanceToLine(line, directionOfXMovement);
            auto lineLength = line.length();

            moveX(distance);
            PenDown();
            moveX(lineLength, directionOfXMovement);
            PenUp();
        }
    }

    Direction chooseDirection()
    {
        auto firstPointInRow = currentRow[0].a;
        auto lastPointInRow = currentRow[currentRow.size() - 1].b;

        auto distanceToFirstPointInRow = abs(xCurrent - firstPointInRow);
        auto distanceToLastPointInRow = abs(xCurrent - lastPointInRow);

        if(distanceToFirstPointInRow < distanceToLastPointInRow)
            return Direction::toRight;
        else
            return Direction::toLeft;
    }

    int distanceToLine(const Line line, Direction direction )
    {
        if(direction == Direction::toRight)
            return line.a - xCurrent;
        else if(direction == Direction::toLeft)
            return line.b - xCurrent;
    }

    void penTest()
    {
        PenDown();
        PenUp();
    }

    void PenDown()
    {
        mZ.rotate(zRotation);
        LED2.toggle();
    }

    void PenUp()
    {
        mZ.rotate(-zRotation);
        LED2.toggle();
    }

    bool moveX(int distance, Direction direction = Direction::toRight)
    {
        if (direction == Direction::toLeft)
            distance = -distance;

        if (xCurrent + distance > WIDTH_MAX || xCurrent + distance < 0)
        {
            console << "WIDTH_MAX reached!" << newline
                   << "xCurrent = " << xCurrent << newline
                   <<  "distance = " << distance << newline;

            syst.waitMS(2000);

            while(true);
        }

        mX.rotate(distance * PIX_ROTATION);

        xCurrent += distance;

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
        // TODO
    }

    void displayCalibrationParameters()
    {
        #define DISP(VAL) console << newline << #VAL " : " << VAL
        DISP(zRotation);
        DISP(xDistanceFromBoundry);
        DISP(PIX_ROTATION);
        #undef DISP
    }

    void showStats()
    {
        console << newline << "Rows left to print: " << rowsLeft;
    }
};

#endif //ROBOCOREPRINTERFRONT_PRINTER_H
