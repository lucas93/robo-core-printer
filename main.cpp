#include "hFramework.h"
#include "SerialDisplay.h"

#include "Printer.h"

void hMain()
{
    while (1)
    {
        RoboCorePrinter printer;

        printer.start();
    }
}
