#ifndef ROBOCOREPRINTERFRONT_ROBOCOREPRINTER_H
#define ROBOCOREPRINTERFRONT_ROBOCOREPRINTER_H

#include <hFramework.h>
#include <algorithm>
#include <iterator>
#include "ConvertedImage.h"
#include "Motor.h"
#include <Lego_Touch.h>
#include "SerialDisplay.h"
#include "SDCardReader.h"

using namespace hSensors;

class RoboCorePrinter
{
private:
    ConvertedImage image;
    String imageDataFileName = "image.txt";
    SDCardReader sdReader;

    RegulatedMotor<2> mX;
    RegulatedMotor<1> mY;
    RegulatedMotor<6> mZ;
    const int mXSpeed = 80;
    const int mYSpeed = 30;
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
    int zCalibrationStep = 20;

    int xDistanceFromBoundryAterCalibration = 1000;

    bool isPauseButtonPushed = false;

    int ROTATION_PER_PIXEL = 80; // elemental servo rotation for 1 pixel

    enum class Direction { toRight, toLeft };
    Direction directionOfXMovement;

    class Calibrator
    {
        RoboCorePrinter* p;
    public:
        Calibrator(RoboCorePrinter* parent) : p(parent) {}

        void calibratePrinter()
        {
            calibrateY();
            calibrateX();
            calibratePen();
        }

        void calibratePen()
        {
            char key;
            do
            {
                console << newline << "Calibrating pen:" << newline
                       << "A - amplitude calibration" << newline
                       << "P - position calibration" << newline
                       << "S - speed calibration" << newline
                       << "T - try" << newline
                       << "C - accept" << newline;

                key = Serial.getch();

                switch (key)
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
            } while(key != 'c');
            console << newline << "Calibrated pen Z axis" << newline;
        }

    private:
        void calibrateY()
        {
            p->mY.setSpeed(p->mYSpeed / 2);
            p->mY.start();
            console << "Press any key when Y i calibrated: ";

            Serial.getch();

            p->mY.stop();
            p->mY.setSpeed(p->mYSpeed);
            p->yCurrent = 0;

            console << newline << "Calibrated Y axis" << newline;
        }

        void calibrateX()
        {
            p->mX.start(true);
            while (p->lTouch.isPressed() == false);
            p->mX.stop();

            p->mX.rotate(p->xDistanceFromBoundryAterCalibration);

            p->directionOfXMovement = Direction::toRight;
            p->xCurrent = 0;

            console << newline << "Calibrated X axis" << newline;
        }


        void calibratePenAmplitude()
        {
            char key;
            console << newline << "Calibrating pen amplitude:" << newline
                   << "Q - top higher" << newline
                   << "A - top lower" << newline
                   << "E - down higher" << newline
                   << "D - down lower" << newline
                   << "T - try" << newline
                   << "C - accept" << newline << newline;
            do
            {
                key = Serial.getch();

                switch (key)
                {
                    case 'q':
                        p->zRotation += p->zCalibrationStep;
                        p->mZ.rotate(-p->zCalibrationStep);
                        break;
                    case 'a':
                        p->zRotation -= p->zCalibrationStep;
                        p->mZ.rotate(p->zCalibrationStep);
                        break;

                    case 'e':
                        p->zRotation -= p->zCalibrationStep;
                        break;

                    case 'd':
                        p->zRotation += p->zCalibrationStep;
                        break;

                    case 't':
                        penTest();
                        break;

                    default:
                        break;
                }
            } while(key != 'c');
        }

        void calibratePenPosition()
        {
            char key;
            console << newline << "Calibrating pen position:" << newline
                   << "W - pen higher" << newline
                   << "S - pen lower" << newline
                   << "T - try" << newline
                   << "C - accept" << newline;

            do
            {
                key = Serial.getch();

                switch (key)
                {
                case 'w':
                    p->mZ.rotate(-p->zCalibrationStep);
                    break;

                case 's':
                   p-> mZ.rotate(p->zCalibrationStep);
                    break;

                case 't':
                    penTest();

                default:
                    break;
                }
            } while(key != 'c');
        }
        void calibratePenSpeed()
        {
            auto constrain = [](int val, int min, int max) -> bool
            {
                return (val < min ? min : (val > max ? max : val));
            };

            char key;
            console << newline << "Calibrating pen speed:" << newline
                   << "W - faster" << newline
                   << "S - slower" << newline
                   << "T - try" << newline
                   << "C - accept" << newline;

            do
            {
                key = Serial.getch();

                switch (key)
                {
                case 'w':
                    p->mZSpeed = constrain(p->mZSpeed + 5, 15, 100);
                    p->mZ.setSpeed(p->mZSpeed);
                    break;

                case 's':
                    p->mZSpeed = constrain(p->mZSpeed - 5, 15, 100);
                    p->mZ.setSpeed(p->mZSpeed);
                    break;

                case 't':
                    penTest();

                default:
                    break;
                }
            } while(key != 'c');
        }

        void penTest()
        {
            p->PenDown();
            p->PenUp();
        }
    };

    class ImagePreparer
    {
        RoboCorePrinter *p;

    public:
        ImagePreparer(RoboCorePrinter* parent) : p(parent) {}

        void prepareImage()
        {
           p->sdReader.prepareSD(p->imageDataFileName);
           int widthMaxFromFile = p->sdReader.parseInt();
           int heightMaxFromFile = p->sdReader.parseInt();

           checkImageDimentions (widthMaxFromFile, heightMaxFromFile);

           p->rowsLeft = p->sdReader.parseInt();
        }

    private:
        void checkImageDimentions(int widthMaxFromFile, int heightMaxFromFile)
        {
            if(  widthMaxFromFile != p->WIDTH_MAX
               || heightMaxFromFile != p->HEIGHT_MAX )
            {
                while(1)
                {
                    console << newline << "Image dimentions don't match!!";
                    Serial.getch();
                }
            }
        }
    };

    Calibrator calibrator{this};
    ImagePreparer imagePreparer{this};

public:

    RoboCorePrinter()
    {
        mX.setSpeed(mXSpeed);
        mY.setSpeed(mYSpeed);
        mZ.setSpeed(mZSpeed);

        mX.setReversedPolarity(true); // zależne od konstrukcji
    }

    void start()
    {
        console << newline << "Press s key to start: " << newline;
        Serial.getch();

        preparePrinter();

        printImage();

        console << newline << "Done!";
    }

private:

    void preparePrinter()
    {
        calibrator.calibratePrinter();
        imagePreparer.prepareImage();

        displayCalibrationParameters();
    }

    void calibratePenAxisIfKeyPressed()
    {
        if(Serial.available() > 0)
        {
            char key = Serial.getch();

            if(key == 'c')
            {
                calibrator.calibratePen();
            }
        }
    }

    void printImage()
    {
        while(getNextRowIfAvailable() == true)
        {
            printRow();
            moveY(1);

            showStats();
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
            auto distance = getDistanceToLineFromCurrentPosition(line, directionOfXMovement);
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

    int getDistanceToLineFromCurrentPosition(const Line line, Direction direction )
    {
        if(direction == Direction::toRight)
            return line.a - xCurrent;
        else if(direction == Direction::toLeft)
            return line.b - xCurrent;
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

            sys.delay(2000);

            while(true);
        }

        mX.rotate(distance * ROTATION_PER_PIXEL);
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

            sys.delay(2000);

            while(true);
        }

        yCurrent += distance;
        mY.rotate(-distance * ROTATION_PER_PIXEL, immediateReturn);
    }


    void displayCalibrationParameters()
    {
        #define DISP(VAL) console << newline << #VAL " : " << VAL
        DISP(zRotation);
        DISP(xDistanceFromBoundryAterCalibration);
        DISP(ROTATION_PER_PIXEL);
        #undef DISP
    }

    void showStats()
    {
        console << newline << "Rows left to print: " << rowsLeft;
    }
};

#endif //ROBOCOREPRINTERFRONT_ROBOCOREPRINTER_H
