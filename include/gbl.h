#ifndef  __GBL_H__
#define  __GBL_H__

#include <malloc.h>
#include "mutex.h"

#if defined(__WIN32__) || defined(_WIN32)
#else
	#define __stdcall
	#include <unistd.h>
	#include <pthread.h>
	#define Sleep(x) sleep(x/1000)
#endif	

#define MAX_CONNECT_DEVICE_NUM	50
#define CLEAR(x, len) memset(x, 0, len)

/* ³ö´íÇå³ýÍË³öºê */
#define cleanup(x)                \
    status = (x);                 \
    gbl_set_quit(1);              \
    goto cleanup

typedef struct global_t{
	int             quit;                /* Global quit flag */
	mutex_t			*mutex;              /* Mutex to protect the global data */
}global_t;

extern void gbl_init(void);
extern void gbl_exit(void);
extern int  gbl_get_quit(void);
extern void gbl_set_quit(int quit);

#if defined(__WIN32__) || defined(_WIN32)
typedef void (*thread_callback)(void *param);
#else
typedef void* (*thread_callback)(void *param);
#endif
extern int rk_sdk_create_thread(thread_callback callback, void *param);

extern int createDir(char *fileName);
#endif
