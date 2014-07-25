// Thread.h
#ifndef __THREAD_H__
#define __THREAD_H__
// #############################################################################
    #ifdef WIN
        #define WIN32_LEAN_AND_MEAN
        #include <Windows.h>
    #else
        #include <pthread.h>

        #define THREAD_STILL_ACTIVE 0x103
        #define THREAD_NOT_INITIATED 0x102
        #define THREAD_FINISHED 0x101
    #endif

#include "constant.h"
#include "gmutex.h"


// =============================================================================
template<class T>
class Thread
{
    // new type Method: pointer to a object's method (this call)
    typedef DWORD (T::* Method)(void);
// -----------------------------------------------------------------------------
protected:
    #ifdef WIN
        HANDLE  hThread;      // unique handle to the thread
    #else
        pthread_t hThread;
        pthread_attr_t attr;
        void *threadReturn;
    #endif
private:
    DWORD   threadID;     // thread id - 0 until started
    T*      object;       // the object which owns the method
    Method  method;       // the method of the object
    static volatile DWORD status; //Para controlar el estado en UNIX
    #ifdef WIN
        HANDLE  hInterrupt;   // mutex to signal an interrupt via ReleaseSemaphore()
        HANDLE  hSingleStart; // only one thread allowed to call start() mutex
    #else
        GMutex *hSingleStart;
    #endif
// -----------------------------------------------------------------------------
private:
    // This function gets executed by a concurrent thread.
    #ifdef WIN
    static DWORD run(LPVOID thread_obj)
    #else
    static void *run(void * thread_obj)
    #endif
    {
//        std::cout << "Thread::running"<< std::endl;
        Thread<T>* thread = (Thread<T>*)thread_obj;
//        std::cout << "Thread::running. Launching method"<< std::endl;
        DWORD retorno = (thread->object->*thread->method) ();
        //No podemos decir que el thread ha terminado porque este es un metodo estatico
        //que no puede acceder a las variables no estaticas
//        thread->setStatus(THREAD_FINISHED);
//        std::cout << "Thread::running. Returning result"<< std::endl;
        #ifdef WIN
            return retorno;
        #else
            status = THREAD_FINISHED;
            cout << "exiting thread now" << endl;
            pthread_exit((void *)retorno);
            return (void *)retorno;
        #endif
    }
    // Prevent copying of threads: No sensible implementation!
    Thread(const Thread<T>& other) {}
    // Prevent assignment of threads: No sensible implementation!
    Thread<T>& operator =(const Thread<T>& other) {}
// -----------------------------------------------------------------------------
public:
    /* Creates a new Thread object. object: the one which method should be
    executed. method: pointer to the object's method. */
    explicit Thread(T* object, DWORD ( T::* method)(void))
    {

        this->object        = object;
        this->method        = method;
        this->threadID      = 0;

        #ifdef WIN
            this->hThread       = NULL;
            this->hInterrupt    = CreateSemaphore(NULL, 1, 1, NULL);
            this->hSingleStart  = CreateMutex(NULL, FALSE, NULL);
        #else
            this->status = THREAD_NOT_INITIATED;
            hSingleStart = new GMutex();
        #endif
    }
// -----------------------------------------------------------------------------
    ~Thread(void)
    {
        #ifdef WIN
            if (hInterrupt)
                CloseHandle(hInterrupt);
            if (hThread)
                CloseHandle(hThread);
        #else
            std::cout << "Thread::Destructor"<<std::endl;
            pthread_attr_destroy(&attr);
            delete hSingleStart;
        #endif
    }
// -----------------------------------------------------------------------------
    /* Starts executing the objects method in a concurrent thread. True if the
    thread was started successfully; otherwise false. */
    bool start()
    {
        #ifdef WIN
            try {
                if (WaitForSingleObject(hSingleStart, 0) != WAIT_OBJECT_0)
                    return false;
                if (hThread)    // Thread had been started sometime in the past
                {
                    if (WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT)
                    {   // if thread's still running deny new start
                        return false;
                    }
                    CloseHandle(hThread);
                }
                // (Re-)Set not interrupted semaphore state
                WaitForSingleObject(hInterrupt, 0);

                hThread = CreateThread(
                    NULL,
                    0,
                    (LPTHREAD_START_ROUTINE) Thread<T>::run,
                    this,
                    0,
                    &this->threadID
                );
                if (hThread)
                    return true;
                return false;
            }
            catch(...)
            {

            }
            ReleaseMutex(hSingleStart);
            return false;
        #else
            if (this->status != THREAD_STILL_ACTIVE){
                int rc;
                hSingleStart->Lock();
                // Initialize and set thread joinable
                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
                //El cuarto parametro le pasa argumentos a la funcion que llamamos
                rc = pthread_create(&hThread, &attr, Thread<T>::run, this);
                if (rc != 0){
                    std::cout << "Error:unable to create thread," << rc << std::endl;
                    return false;
                }
                status = THREAD_STILL_ACTIVE;
                hSingleStart->Unlock();
                return true;
            } else {
                return false;
            }

        #endif


    }
// -----------------------------------------------------------------------------
    // Blocks the calling thread until this thread has stopped.
    inline int join()
    {
        #ifdef WIN
        WaitForSingleObject(hThread, INFINITE);
        return 1;
        #else
            int rc;
            rc = pthread_join(hThread, &threadReturn);

            if (rc != 0){
                std::cout << "Error:unable to join," << rc << std::endl;
                return -1;
            }
            status = THREAD_FINISHED;
            pthread_attr_destroy(&attr);
            std::cout << "join: completed thread";
            std::cout << "  exiting with threadReturn :" << threadReturn << std::endl;
            return (int)threadReturn;
        #endif
    }
// -----------------------------------------------------------------------------
    /* Asks the thread to exit nicely. Thread function must implement checks.
    return value indicates if the interrupt could be placed not if the thread
    reacts on the interrupt. true indicates success, false an error. */
    inline bool interrupt()
    {
         #ifdef WIN
        if (hInterrupt)
        {
            return ((ReleaseSemaphore(hInterrupt, 1, NULL) == FALSE) ?
                false : true);
        }
        return false;
        #else
        return false;
        #endif
    }
// -----------------------------------------------------------------------------
    /* True if an interrupt request was set, otherwise false. */
    inline bool isInterrupted()
    {
        #ifdef WIN
        return this->isInterrupted(0);
        #else
        return false;
        #endif
    }
// -----------------------------------------------------------------------------
    /* True if an interrupt request was set, otherwise false. Waits for millisec
    milliseconds for the interrupt to take place. */
    inline bool isInterrupted(DWORD millisec)
    {
        #ifdef WIN
        if (WaitForSingleObject(hInterrupt, millisec) == WAIT_TIMEOUT)
        {
            return false;
        }
        ReleaseSemaphore(hInterrupt, 1, NULL);  // keep interrupted state
        return true;
        #else
        return false;
        #endif
    }
// -----------------------------------------------------------------------------
    inline bool isRunning()
    {
        #ifdef WIN
        DWORD exitCode = 0;
        if (hThread)
            GetExitCodeThread(hThread, &exitCode);
        if (exitCode == STILL_ACTIVE)
            return true;
        return false;
        #else
        if (status == THREAD_STILL_ACTIVE)
            return true;

        return false;
        #endif
    }
// -----------------------------------------------------------------------------
    // Getter & Setter
// -----------------------------------------------------------------------------
    #ifdef WIN
    inline HANDLE getThreadHandle()
    #else
    inline pthread_t getThreadHandle()
    #endif
    {
        return hThread;
    }
// -----------------------------------------------------------------------------
    inline DWORD getThreadID()
    {
        #ifdef WIN
            return threadID;
        #else
            //return pthread_self();
            return hThread;
//            return pthread_getthreadid_np() ;
//            return 0;
        #endif
    }

    inline void setStatus(DWORD var)
    {
        #ifndef WIN
            this->status = var;
            std::cout << "status asignning: " << status << std::endl;
        #endif
    }
// -----------------------------------------------------------------------------
};

template <class T> volatile DWORD  Thread<T>::status;
// #############################################################################
#endif // __THREAD_H__
