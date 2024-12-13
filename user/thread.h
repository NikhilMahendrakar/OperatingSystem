#ifndef _THREAD_H_
#define _THREAD_H_

#define PGSIZE 4096
typedef int lock_t;

// Function prototypes
int thread_create(void *(*start_routine)(void *), void *arg);
void lock_init(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

#endif
