#include "hFramework.h"
#include "SerialDisplay.h"
#include "ConvertedImage.h"

#include "Printer.h"

void hMain()
{
    while (1)
    {
        LED1.toggle();

        Printer printer;

        printer.start();
    }
}
