#include "inc/asm.h"
#include "inc/rwlock.h"

#include <pthread.h>

void
rw_lock_init(rwlock_t *l)
{
	l->read = l->write = l->wait = 0;
	pthread_mutex_init(&(l->mutex), NULL);
	pthread_cond_init(&(l->cond), NULL);
}

void
rw_read_lock(rwlock_t *l)
{
	pthread_mutex_lock(&(l->mutex));

	while (l->write != 0 || l->wait != 0) {
		pthread_cond_wait(&(l->cond), &(l->mutex));
	}

	l->read++;

	pthread_mutex_unlock(&(l->mutex));
}

void
rw_read_unlock(rwlock_t *l)
{
	pthread_mutex_lock(&(l->mutex));
	l->read--;
	if (l->read == 0)
		pthread_cond_broadcast(&(l->cond));
	pthread_mutex_unlock(&(l->mutex));
}

void
rw_write_lock(rwlock_t *l)
{
	pthread_mutex_lock(&(l->mutex));

	while (l->read != 0 || l->write != 0) {
		if (l->wait == 0) l->wait = 1;
		pthread_cond_wait(&(l->cond), &(l->mutex));
	}

	l->write = 1;
	l->wait = 0;

	pthread_mutex_unlock(&(l->mutex));
}

void
rw_write_unlock(rwlock_t *l)
{
	pthread_mutex_lock(&(l->mutex));
	l->write = 0;
	l->wait = 0;
	pthread_cond_broadcast(&(l->cond));
	pthread_mutex_unlock(&(l->mutex));
}