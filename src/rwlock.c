#include "inc/asm.h"
#include "inc/rwlock.h"

void
rw_read_lock(rwlock_t *l)
{
	while (1) {
		while (*l & (RW_WAIT | RW_WRITE))
			pause();
		
		if (!(atomic_xadd(l, RW_READ) & (RW_WAIT | RW_WRITE)))
			return;

		atomic_add(l, -RW_READ);
	}
}

void
rw_read_unlock(rwlock_t *l)
{
	atomic_add(l, -RW_READ);
}

void
rw_write_lock(rwlock_t *l)
{
	while (1) {
		unsigned me = *l;

		if (me < RW_WRITE) {
			if (atomic_cmpxchg(l, me, RW_WRITE) == me)
				return;
			me = *l;
		}

		if (!(me & RW_WAIT))
			atomic_set_bit(l, RW_WAIT_BIT);
		
		while (*l > RW_WAIT)
			pause();
	}
}

void
rw_write_unlock(rwlock_t *l)
{
	atomic_add(l, -RW_WRITE);
}