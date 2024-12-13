#include "kernel/types.h" 
#include "kernel/stat.h" 
#include "user/thread.h"
#include "user/user.h" 

int 
thread_create(void *(*start_routine)(void *), void *arg) 
{
    void *stack = malloc(PGSIZE);
    if (!stack)
        return -1;

    int ret = clone(stack);
    if (ret == 0) {
        // Child thread 
        start_routine(arg); 
        exit(0);
    }

    return ret;
}

void 
lock_init(lock_t *lock) 
{
    *lock = 0;
}

void 
lock_acquire(lock_t *lock) 
{
    while (__sync_lock_test_and_set(lock, 1))
        ;
}

void 
lock_release(lock_t *lock) 
{
    __sync_lock_release(lock);
}