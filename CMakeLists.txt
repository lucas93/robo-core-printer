cmake_minimum_required(VERSION 3.6)
project(RoboCorePrinterFront)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp Motor.h Printer.h Pair.h Row.h TouchSensor.h SerialDisplay.h ControlButtons.h
System.h Line.h SDCardReader.h ConvertedImage.h)
add_executable(RoboCorePrinterFront ${SOURCE_FILES})
