#include "inc/asm.h"
#include "inc/rwlock.h"

#include <pthread.h>

void
rw_lock_init(rwlock_t *l)
{
	l->l = 0;
	pthread_mutex_init(&(l->mutex), NULL);
	pthread_cond_init(&(l->cond), NULL);
}

void
rw_read_lock(rwlock_t *l)
{
	while (1) {

		pthread_mutex_lock(&(l->mutex));
		while (l->l & (RW_WAIT | RW_WRITE))
			pthread_cond_wait(&(l->cond), &(l->mutex));
		pthread_mutex_unlock(&(l->mutex));
		
		if (!(atomic_xadd(&(l->l), RW_READ) & (RW_WAIT | RW_WRITE)))
			return;

		if (atomic_add(&(l->l), -RW_READ) < RW_WRITE) {
			pthread_mutex_lock(&(l->mutex));
			pthread_cond_broadcast(&(l->cond));
			pthread_mutex_unlock(&(l->mutex));
		}
	}
}

void
rw_read_unlock(rwlock_t *l)
{
	if (atomic_add(&(l->l), -RW_READ) < RW_WRITE) {
		pthread_mutex_lock(&(l->mutex));
		pthread_cond_broadcast(&(l->cond));
		pthread_mutex_unlock(&(l->mutex));
	}
	
	// atomic_add(&(l->l), -RW_READ);
	// pthread_mutex_lock(&(l->wmutex));
	// pthread_cond_signal(&(l->wcond));
	// pthread_mutex_unlock(&(l->wmutex));
}

void
rw_write_lock(rwlock_t *l)
{
	while (1) {
		pthread_mutex_lock(&(l->mutex));
		unsigned me = l->l;

		if (me < RW_WRITE) {
			if (atomic_cmpxchg(&(l->l), me, RW_WRITE) == me) {
				pthread_mutex_unlock(&(l->mutex));
				return;
			}
			me = l->l;
		}

		if (!(me & RW_WAIT))
			atomic_set_bit(&(l->l), RW_WAIT_BIT);
		
		while (l->l > RW_WAIT)
			pthread_cond_wait(&(l->cond), &(l->mutex));
		pthread_mutex_unlock(&(l->mutex));
	}
}

void
rw_write_unlock(rwlock_t *l)
{
	atomic_add(&(l->l), -RW_WRITE);
	pthread_mutex_lock(&(l->mutex));
	pthread_cond_broadcast(&(l->cond));
	pthread_mutex_unlock(&(l->mutex));
}