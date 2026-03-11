#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// ========== 有界缓冲区问题定义 ==========
#define BUFFER_SIZE 5  // 缓冲区大小

// 缓冲区结构
struct CircularBuffer {
    int buffer[BUFFER_SIZE];  // 数据缓冲区
    int in;                   // 生产者放入数据的位置
    int out;                  // 消费者取出数据的位置
    int count;                // 当前缓冲区中的数据数量
};

CircularBuffer circularBuffer;  // 全局缓冲区

 // 信号量
Semaphore mutex;         // 互斥信号量，初值为1
Semaphore empty_slots;   // 空槽位信号量，初值为BUFFER_SIZE
Semaphore filled_slots;  // 满槽位信号量，初值为0

// 不使用同步机制的情况
void unsync_producer(void *arg) {
    int item = (int)(long)arg;  // 要生产的数据项
    int delay;
    
    printf("Producer %d: trying to produce item\n", item);
    
    // 检查缓冲区是否已满
    if (circularBuffer.count >= BUFFER_SIZE) {
        printf("Producer %d: Buffer full, giving up!\n", item);
        return;
    }
    
    // 生产数据需要一些时间
    delay = 0xfffffff;
    while (delay) --delay;
    
    // 将数据放入缓冲区
    circularBuffer.buffer[circularBuffer.in] = item;
    printf("Producer %d: item produced at position %d\n", item, circularBuffer.in);
    
    // 更新in指针
    circularBuffer.in = (circularBuffer.in + 1) % BUFFER_SIZE;
    
    // 更新计数
    circularBuffer.count++;
    
    printf("Producer %d: Buffer count is now %d\n", item, circularBuffer.count);
}

void unsync_consumer(void *arg) {
    int consumerID = (int)(long)arg;
    int item;
    int delay;
    
    printf("Consumer %d: trying to consume item\n", consumerID);
    
    // 检查缓冲区是否为空
    if (circularBuffer.count <= 0) {
        printf("Consumer %d: Buffer empty, giving up!\n", consumerID);
        return;
    }
    
    // 消费数据需要一些时间
    delay = 0xffffffff;
    while (delay) --delay;
    
    // 从缓冲区取出数据
    item = circularBuffer.buffer[circularBuffer.out];
    printf("Consumer %d: consumed item %d at position %d\n", consumerID, item, circularBuffer.out);
    
    // 更新out指针
    circularBuffer.out = (circularBuffer.out + 1) % BUFFER_SIZE;
    
    // 更新计数
    circularBuffer.count--;
    
    printf("Consumer %d: Buffer count is now %d\n", consumerID, circularBuffer.count);
}

// 使用信号量实现同步的生产者
void sync_producer(void *arg) {
    int item = (int)(long)arg;  // 要生产的数据项
    int delay;
    
    printf("Producer %d: trying to produce item\n", item);
    
    // P(empty_slots): 等待一个空槽位
    empty_slots.P();
    
    // P(mutex): 获取缓冲区互斥访问权
    mutex.P();
    
    // 生产数据需要一些时间
    delay = 0x1ffffff;
    while (delay) --delay;
    
    // 将数据放入缓冲区
    circularBuffer.buffer[circularBuffer.in] = item;
    printf("Producer %d: item produced at position %d\n", item, circularBuffer.in);
    
    // 更新in指针
    circularBuffer.in = (circularBuffer.in + 1) % BUFFER_SIZE;
    
    // 更新计数
    circularBuffer.count++;
    
    printf("Producer %d: Buffer count is now %d\n", item, circularBuffer.count);
    
    // V(mutex): 释放缓冲区互斥访问权
    mutex.V();
    
    // V(filled_slots): 通知有一个满槽位
    filled_slots.V();
}

// 使用信号量实现同步的消费者
void sync_consumer(void *arg) {
    int consumerID = (int)(long)arg;
    int item;
    int delay;
    
    printf("Consumer %d: trying to consume item\n", consumerID);
    
    // P(filled_slots): 等待一个满槽位
    filled_slots.P();
    
    // P(mutex): 获取缓冲区互斥访问权
    mutex.P();
    
    // 消费数据需要一些时间
    delay = 0x1ffffff;
    while (delay) --delay;
    
    // 从缓冲区取出数据
    item = circularBuffer.buffer[circularBuffer.out];
    printf("Consumer %d: consumed item %d at position %d\n", consumerID, item, circularBuffer.out);
    
    // 更新out指针
    circularBuffer.out = (circularBuffer.out + 1) % BUFFER_SIZE;
    
    // 更新计数
    circularBuffer.count--;
    
    printf("Consumer %d: Buffer count is now %d\n", consumerID, circularBuffer.count);
    
    // V(mutex): 释放缓冲区互斥访问权
    mutex.V();
    
    // V(empty_slots): 通知有一个空槽位
    empty_slots.V();
}

void first_thread_unsync(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 初始化缓冲区
    circularBuffer.in = 0;
    circularBuffer.out = 0;
    circularBuffer.count = 0;
    
    printf("===no semophere===\n\n");
    
    // 创建多个生产者和消费者线程
    printf("creating multi producers and consumers...\n\n");
    
    // 首先让一些消费者尝试从空缓冲区消费
    programManager.executeThread(unsync_consumer, (void *)1, "consumer 1", 1);
    programManager.executeThread(unsync_consumer, (void *)2, "consumer 2", 1);
    
    // 创建生产者线程
    programManager.executeThread(unsync_producer, (void *)1, "producer 1", 1);
    programManager.executeThread(unsync_producer, (void *)2, "producer 2", 1);
    programManager.executeThread(unsync_producer, (void *)3, "producer 3", 1);

     // 创建更多生产者线程，尝试使缓冲区溢出
    programManager.executeThread(unsync_producer, (void *)4, "producer 4", 1);
    programManager.executeThread(unsync_producer, (void *)5, "producer 5", 1);
    programManager.executeThread(unsync_producer, (void *)6, "producer 6", 1);
    
    // 创建更多消费者线程
    programManager.executeThread(unsync_consumer, (void *)3, "consumer 3", 1);
    programManager.executeThread(unsync_consumer, (void *)4, "consumer 4", 1);
    
   
    
    asm_halt();
}

void first_thread_sync(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 初始化缓冲区
    circularBuffer.in = 0;
    circularBuffer.out = 0;
    circularBuffer.count = 0;

    mutex.initialize((uint32)1);
    empty_slots.initialize((uint32)5);
    filled_slots.initialize((uint32)0);

    printf("===with semophere===\n\n");
    
    // 创建多个生产者和消费者线程
    printf("creating multi producers and consumers...\n\n");
    
    // 首先让一些消费者尝试从空缓冲区消费
    programManager.executeThread(sync_consumer, (void *)1, "consumer 1", 1);
    programManager.executeThread(sync_consumer, (void *)2, "consumer 2", 1);
    
    // 创建生产者线程
    programManager.executeThread(sync_producer, (void *)1, "producer 1", 1);
    programManager.executeThread(sync_producer, (void *)2, "producer 2", 1);
    programManager.executeThread(sync_producer, (void *)3, "producer 3", 1);

     // 创建更多生产者线程，尝试使缓冲区溢出
    programManager.executeThread(sync_producer, (void *)4, "producer 4", 1);
    programManager.executeThread(sync_producer, (void *)5, "producer 5", 1);
    programManager.executeThread(sync_producer, (void *)6, "producer 6", 1);
    
    // 创建更多消费者线程
    programManager.executeThread(sync_consumer, (void *)3, "consumer 3", 1);
    programManager.executeThread(sync_consumer, (void *)4, "consumer 4", 1);
    
   
    
    asm_halt();
}

extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread_sync, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
