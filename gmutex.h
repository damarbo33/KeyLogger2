#ifndef _GMutex_h_
#define _GMutex_h_
#ifdef WIN
    #include <windows.h>
#else
    #include <pthread.h>
#endif

class GMutex{
    public:
        GMutex(){
            #ifdef WIN
                InitializeCriticalSection( &m_mutex );
            #else
                pthread_mutexattr_t   mta;
                pthread_mutexattr_init(&mta);
                pthread_mutex_init( &m_mutex, &mta);
            #endif
        }

        ~GMutex()
        {
            #ifdef WIN
                DeleteCriticalSection( &m_mutex );
            #else
                pthread_mutex_destroy( &m_mutex );
            #endif
        }

        inline void Lock()
        {
            #ifdef WIN
                EnterCriticalSection( &m_mutex );
            #else
                pthread_mutex_lock( &m_mutex );
            #endif
        }

        inline void Unlock()
        {
            #ifdef WIN
                LeaveCriticalSection( &m_mutex );
            #else
                pthread_mutex_unlock( &m_mutex );
            #endif
        }

    protected:
        #ifdef WIN
            CRITICAL_SECTION m_mutex;
        #else
            pthread_mutex_t m_mutex;
        #endif
};

#endif // _GMutex_h_
