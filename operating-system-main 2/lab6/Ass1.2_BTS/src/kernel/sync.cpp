#include "sync.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"

SpinLock::SpinLock()
{
    initialize();
}

void SpinLock::initialize()
{
    bolt = 0;
}

void SpinLock::lock()
{
    uint32 key = 1;

    do
    {
        asm_atomic_exchange(&key, &bolt);
        //printf("pid: %d\n", programManager.running->pid);
    } while (key);
}

void SpinLock::unlock()
{
    bolt = 0;
}

// BTSSpinLock的实现
BTSSpinLock::BTSSpinLock()
{
    initialize();
}

void BTSSpinLock::initialize()
{
    bolt = 0;
}

void BTSSpinLock::lock()
{
    // 使用your_asm_atomic_exchange函数尝试获取锁
    // 如果返回0，说明获取成功，退出循环
    // 如果返回1，说明锁被占用，继续循环
    while(your_asm_atomic_exchange(&bolt))
    {
        // 自旋等待
    }
}

void BTSSpinLock::unlock()
{
    // 释放锁只需要将bolt设为0
    bolt = 0;
}