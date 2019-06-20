
#ifndef  __MUTEX_H__
#define  __MUTEX_H__

#ifdef WIN32

#else
    #include <pthread.h>
#endif

typedef struct mutex_t{

#if defined(__WIN32__) || defined(_WIN32)
	void *mutex;   //»¥³â
#else
    pthread_mutex_t mutex;
#endif	
}mutex_t;

mutex_t *  mutex_create(void);
int   mutex_lock(mutex_t *handle);
int   mutex_unlock(mutex_t *handle);
void  mutex_delete(mutex_t *handle);


#endif

