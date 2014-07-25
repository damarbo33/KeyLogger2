#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H


    #ifdef WIN
        #define WINVER 0x0501
        #include <windows.h>
    #endif

        #include <iostream>

        using namespace std;

        class PrintScreen
        {
            public:
                PrintScreen();
                virtual ~PrintScreen();
                int Print();
            protected:
            private:
        };


#endif // PRINTSCREEN_H
