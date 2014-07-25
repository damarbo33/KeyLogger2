#ifndef HOTKEY_H
#define HOTKEY_H

#include <iostream>
#include <fstream>
#include "constantes.h"
#include "fileio.h"
#include "Constant.h"


using namespace std;

class BufferKeys{
    public:
        BufferKeys(){size = 0;}
        ~BufferKeys(){};
        const static int MAX_KEYS = 20;

        char * getData(){return data;}

        int getSize(){
            return size;
        }

        void setSize(int var){
            size = var;
        }

        void inc(){
            size++;
        }

        void add(DWORD var){
            if (size < MAX_KEYS){
                data[size] = (char)var;
                inc();
            }
        }

        void clear(){
            memset(data, '\0', MAX_KEYS);
            size = 0;
        }
    private:
        int size;
        char data[MAX_KEYS];
};


class HotKey{
    public:
        DWORD  init();
        bool salir;

        static HotKey *m_pThis;

        HotKey(){
            m_pThis = this;
            buff = new BufferKeys();
        };

        ~HotKey(){
            #ifdef WIN
            UnhookWindowsHookEx(hookKB);
            UnhookWindowsHookEx(hookMOUSE);
            #endif
            archivo.writeToFile(DATAOUT, buff->getData(), buff->getSize(), true);
            delete buff;
            std::cout << "Hotkey Destructor" << std::endl; // Show us when the key has been pushed
        };

        #ifdef WIN
        static HHOOK hookKB;
        static HHOOK hookMOUSE;
        #endif

        DWORD capture(void *ptr);



    protected:

    private:
        BufferKeys *buff;
        Fileio archivo;

        #ifdef WIN
            LRESULT LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );
            LRESULT LowLevelMouseProc( int nCode, WPARAM wParam, LPARAM lParam );
            // Trampoline
            static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam){
                return m_pThis->LowLevelKeyboardProc(code, wParam, lParam);
            }

            // Trampoline
            static LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam){
                return m_pThis->LowLevelMouseProc(code, wParam, lParam);
            }
            /* NOT static */
            bool SetKeyboardHook(){

                HINSTANCE appInstance = GetModuleHandle(NULL);
                hookKB = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, appInstance, 0);
                return true;
            }

            /* NOT static */
            bool SetMouseHook(){
                HINSTANCE appInstance = GetModuleHandle(NULL);
                hookMOUSE = ::SetWindowsHookEx(WH_MOUSE_LL, MouseProc, appInstance, 0);
                return true;
            }
        #endif
};




#endif // HOTKEY_H
