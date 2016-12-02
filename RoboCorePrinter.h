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

            displayCalibrationParameters();
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
            p->printer.PenDown();
            p->printer.PenUp();
        }

        void displayCalibrationParameters()
        {
            #define DISP(VAL) console << newline << #VAL " : " << VAL
            DISP(p->zRotation);
            DISP(p->xDistanceFromBoundryAterCalibration);
            DISP(p->ROTATION_PER_PIXEL);
            #undef DISP
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

    class Printer
    {
        RoboCorePrinter *p;

    public:
        Printer(RoboCorePrinter* parent) : p(parent) {}

        void calibratePenAxisIfKeyPressed()
        {
            if(Serial.available() > 0)
            {
                char key = Serial.getch();

                if(key == 'c')
                {
                    p->calibrator.calibratePen();
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
            if (p->rowsLeft > 0)
            {
                --(p->rowsLeft);
                p->currentRow = p->sdReader.parseRow();
                return true;
            }
            return false;
        }

        void printRow()
        {
            p->directionOfXMovement = chooseDirection();
            Row rowToDraw = p->currentRow;

            if(p->directionOfXMovement == Direction::toLeft)
                rowToDraw = reverse(p->currentRow);

            for(auto & line : rowToDraw)
            {
                auto distance = getDistanceToLineFromCurrentPosition(line, p->directionOfXMovement);
                auto lineLength = line.length();

                moveX(distance);
                PenDown();
                moveX(lineLength, p->directionOfXMovement);
                PenUp();
            }
        }

        Direction chooseDirection()
        {
            auto firstPointInRow = p->currentRow[0].a;
            auto lastPointInRow = p->currentRow[p->currentRow.size() - 1].b;

            auto distanceToFirstPointInRow = abs(p->xCurrent - firstPointInRow);
            auto distanceToLastPointInRow = abs(p->xCurrent - lastPointInRow);

            if(distanceToFirstPointInRow < distanceToLastPointInRow)
                return Direction::toRight;
            else
                return Direction::toLeft;
        }

        int getDistanceToLineFromCurrentPosition(const Line line, Direction direction )
        {
            if(direction == Direction::toRight)
                return line.a - p->xCurrent;
            else if(direction == Direction::toLeft)
                return line.b - p->xCurrent;
        }

        void PenDown()
        {
            p->mZ.rotate(p->zRotation);
            LED2.toggle();
        }

        void PenUp()
        {
            p->mZ.rotate(-(p->zRotation));
            LED2.toggle();
        }

        void moveX(int distance, Direction direction = Direction::toRight)
        {
            if (direction == Direction::toLeft)
                distance = -distance;

            p->mX.rotate(distance * p->ROTATION_PER_PIXEL);
            p->xCurrent += distance;
        }

        void moveY(int distance, bool immediateReturn = false)
        {
            p->yCurrent += distance;
            p->mY.rotate(-distance * p->ROTATION_PER_PIXEL, immediateReturn);
        }

        void showStats()
        {
            console << newline << "Rows left to print: " << p->rowsLeft;
        }
    };

    Calibrator calibrator{this};
    ImagePreparer imagePreparer{this};
    Printer printer{this};

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

        calibrator.calibratePrinter();
        imagePreparer.prepareImage();
        printer.printImage();

        console << newline << "Done!";
    }





};

#endif //ROBOCOREPRINTERFRONT_ROBOCOREPRINTER_H
