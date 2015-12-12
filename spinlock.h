#include <inttypes.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>


typedef uintptr_t ngx_uint_t;

#define ngx_ncpu 8


typedef volatile unsigned long  ngx_atomic_t;





ngx_atomic_t* ngx_spinlock_create();
ngx_uint_t ngx_spinlock_trylock(ngx_atomic_t *lock, pid_t pid);


void ngx_spinlock_unlock(ngx_atomic_t *lock, pid_t pid);

void ngx_spinlock_lock(ngx_atomic_t *lock, pid_t pid, ngx_uint_t spin);
