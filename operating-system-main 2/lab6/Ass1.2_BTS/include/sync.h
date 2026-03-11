#ifndef SYNC_H
#define SYNC_H

#include "os_type.h"

class SpinLock
{
private:
    // 共享变量
    uint32 bolt;
public:
    SpinLock();
    void initialize();
    // 请求进入临界区
    void lock();
    // 离开临界区
    void unlock();
};

// 使用bts指令和lock前缀实现的自旋锁
class BTSSpinLock
{
private:
    // 共享变量
    uint32 bolt;
public:
    BTSSpinLock();
    void initialize();
    // 请求进入临界区
    void lock();
    // 离开临界区
    void unlock();
};

#endif