#include "spinlock.h"


ngx_atomic_t*
ngx_spinlock_create()
{
     return (ngx_atomic_t *)mmap(NULL, sizeof(ngx_atomic_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
}

ngx_uint_t
ngx_spinlock_trylock(ngx_atomic_t *lock, pid_t pid)
{
    return (*lock == 0 && __sync_bool_compare_and_swap(lock, 0, pid));
}


void
ngx_spinlock_unlock(ngx_atomic_t *lock, pid_t pid)
{
    if (!__sync_bool_compare_and_swap(lock, pid, 0)) {
        printf("%s\n", "ngx_spinlock_unlock error");
    }
}


void
ngx_spinlock_lock(ngx_atomic_t *lock, pid_t pid, ngx_uint_t spin)
{
    ngx_uint_t  i, n;

    //一直处于循环中，直到获取到锁
    for ( ;; ) {

        //lock为0表示没有其他进程持有锁，这时将lock值设置为value参数表示当前进程持有了锁
        //__sync_bool_compare_and_swap(lock, old, set)
        //这是原子操作
        //尝试把lock的值设置成value，如果不为0就说明已经有其他进程获取了该锁
        if (*lock == 0 && __sync_bool_compare_and_swap(lock, 0, pid)) {
            //获取锁成功
            return;
        }

        //如果是多处理器系统
        if (ngx_ncpu > 1) {
            //spin=1024，每次shift一位


            for (n = 1; n < spin; n <<= 1) {
                //随着等待的次数越来越多，实际去检查锁的间隔时间越来越大
                //n=1，2，4，8，16，32，64，128 ...
                //每次等待时间增加一倍
                for (i = 0; i < n; i++) {

                    //__asm__(".byte 0xf3, 0x90")
                    //__asm__("pause")
                    //等待一段时间
                    __asm__ ("pause");

                }

                //等待一段时间再去尝试获取锁
                if (*lock == 0 && __sync_bool_compare_and_swap(lock, 0, pid)) {
                    //获取锁成功
                    return;
                }
            }
        }

        //试了这么多次，还是没有获取锁，那就休息一下吧
        //sched_yield()
        //usleep(1)
        sched_yield();
    }

}

