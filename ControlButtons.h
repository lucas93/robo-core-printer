#ifndef ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H
#define ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H

#ifdef __ISENSOR_H__
using Button = KeyCode;

namespace ButtonManager
{
    hQueue<Button> buttonQueue(10);

    void init()
    {
        platform.ui.onKeyEvent = [](KeyEventType type, KeyCode code)
        {
            if (type == KeyEventType::Pressed)
            {
                // console << "flushing\n";
                buttonQueue.flush();
                // console << "sending to front\n";
                ButtonManager::buttonQueue.sendToBack(code);
            }

        };
    }

    void initializeIfNotInitializedAlready()
    {
        static bool initialized = false;
        if(!initialized)
        {
            init();
            initialized = true;
        }
    }

    Button waitForAnyPress()
    {
        initializeIfNotInitializedAlready();

        Button button = Button::Key_A;
        buttonQueue.flush();
        // console << "Waiting to receive: \n";
        buttonQueue.receive(button);

        return button;
    }
};
#else

enum class Button
{
    Backspace = 8, Tab = 9, Enter = 13, Shift = 16,
    Ctrl = 17, Alt = 18, PauseBreak = 19, CapsLock = 20,
    Esc = 27, Space = 32, PageUp = 33, PageDown = 34,
    End = 35, Home = 36, Left = 37, Up = 38,
    Right = 39, Down = 40, Insert = 45, Delete = 46,
    Key_0 = 48, Key_1 = 49, Key_2 = 50, Key_3 = 51,
    Key_4 = 52, Key_5 = 53, Key_6 = 54, Key_7 = 55,
    Key_8 = 56, Key_9 = 57, Key_A = 65, Key_B = 66,
    Key_C = 67, Key_D = 68, Key_E = 69, Key_F = 70,
    Key_G = 71, Key_H = 72, Key_I = 73, Key_J = 74,
    Key_K = 75, Key_L = 76, Key_M = 77, Key_N = 78,
    Key_O = 79, Key_P = 80, Key_Q = 81, Key_R = 82,
    Key_S = 83, Key_T = 84, Key_U = 85, Key_V = 86,
    Key_W = 87, Key_X = 88, Key_Y = 89, Key_Z = 90,
    LeftWinkey = 91, RightWinkey = 92, Select = 93, Numpad_0 = 96,
    Numpad_1 = 97, Numpad_2 = 98, Numpad_3 = 99, Numpad_4 = 100,
    Numpad_5 = 101, Numpad_6 = 102, Numpad_7 = 103, Numpad_8 = 104,
    Numpad_9 = 105, NumpadStar = 106, NumpadAdd = 107, NumpadMinus = 109,
    NumpadDot = 110, NumpadUnderscore = 111, F1 = 112, F2 = 113,
    F3 = 114, F4 = 115, F5 = 116, F6 = 117,
    F7 = 118, F8 = 119, F9 = 120, F10 = 121,
    F11 = 122, F12 = 123, NumLock = 144, ScrollLock = 145,
    Semicolon = 186, Equals = 187, Comma = 188, Minus = 189,
    Dot = 190, Underscore = 191, Grave = 192, RightBracket = 219,
    Backslash = 220, LeftBracket = 221, Apostrope = 222, NUMBER_OF_KEYS
};

namespace ButtonManager
{
    void init() {}
    void initializeIfNotInitializedAlready(){}
    Button waitForAnyPress()
    {
        return Button::Key_A;
    }
}


#endif // #ifndef __ISENSOR_H__

#endif //ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H
