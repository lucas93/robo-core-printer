#ifndef ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H
#define ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H

using Button = char;

namespace ButtonManager
{
#ifdef __HCLOUDCLIENT_H__
    hQueue<KeyCode> buttonQueue(10);

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
#endif //#ifdef __HCLOUDCLIENT_H__

    Button waitForAnyPress()
    {
        #ifdef __HCLOUDCLIENT_H__
        initializeIfNotInitializedAlready();

        KeyCode button = KeyCode::Key_A;
        buttonQueue.flush();
        // console << "Waiting to receive: \n";
        buttonQueue.receive(button);

        return [button]()
        {
            switch(button)
            {
            case KeyCode::Key_A: return 'a';
            case KeyCode::Key_B: return 'b';
            case KeyCode::Key_C: return 'c';
            case KeyCode::Key_D: return 'd';
            case KeyCode::Key_E: return 'e';
            case KeyCode::Key_F: return 'f';
            case KeyCode::Key_G: return 'g';
            case KeyCode::Key_H: return 'h';
            case KeyCode::Key_I: return 'i';
            case KeyCode::Key_J: return 'j';
            case KeyCode::Key_K: return 'k';
            case KeyCode::Key_L: return 'l';
            case KeyCode::Key_M: return 'm';
            case KeyCode::Key_N: return 'n';
            case KeyCode::Key_O: return 'o';
            case KeyCode::Key_P: return 'p';
            case KeyCode::Key_R: return 'r';
            case KeyCode::Key_S: return 's';
            case KeyCode::Key_T: return 't';
            case KeyCode::Key_U: return 'u';
            case KeyCode::Key_W: return 'w';
            case KeyCode::Key_Y: return 'y';
            case KeyCode::Key_Z: return 'z';
            case KeyCode::Key_0: return '0';
            case KeyCode::Key_1: return '1';
            case KeyCode::Key_2: return '2';
            case KeyCode::Key_3: return '3';
            case KeyCode::Key_4: return '4';
            case KeyCode::Key_5: return '5';
            case KeyCode::Key_6: return '6';
            case KeyCode::Key_7: return '7';
            case KeyCode::Key_8: return '8';
            case KeyCode::Key_9: return '9';
            }
        }();
        #endif //#ifdef __HCLOUDCLIENT_H__

        #ifndef __HCLOUDCLIENT_H__
        return Serial.getch();
        #endif //#ifndef __HCLOUDCLIENT_H__
    }
};


#endif //ROBOCOREPRINTERFRONT_CONTROLBUTTONS_H
