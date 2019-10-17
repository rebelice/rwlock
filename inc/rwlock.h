#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#include <pthread.h>

typedef struct rwlock_t rwlock_t;

struct rwlock_t{
	unsigned read, write, wait;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

#define RW_WAIT_BIT 	0
#define RW_WRITE_BIT	1
#define RW_READ_BIT		2

#define RW_WAIT			1
#define RW_WRITE		2
#define RW_READ			4

void rw_lock_init(rwlock_t *);
void rw_read_lock(rwlock_t *);
void rw_read_unlock(rwlock_t *);
void rw_write_lock(rwlock_t *);
void rw_write_unlock(rwlock_t *);

#endif