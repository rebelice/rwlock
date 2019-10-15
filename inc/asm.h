#ifndef __ASM_H__
#define __ASM_H__

#define atomic_xadd(A, B) __sync_fetch_and_add((A), (B))
#define atomic_add(A, B) __sync_add_and_fetch((A), (B))
#define atomic_cmpxchg(A, B, C) __sync_val_compare_and_swap((A), (B), (C))
#define atomic_set_bit(A, B) __sync_or_and_fetch((A), 1<<(B))

#define pause() asm volatile("pause\n":::"memory")

#endif /* !__ASM_H__ */