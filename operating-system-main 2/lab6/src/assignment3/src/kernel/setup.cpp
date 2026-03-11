#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "stdlib.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// ========== 哲学家就餐问题 ==========
#define NUM_PHILOSOPHERS 5   // 哲学家数量

// 哲学家状态
enum PhilosopherState {
    THINKING,  // 思考
    HUNGRY,    // 饥饿
    EATING     // 进餐
};

// 全局变量
Semaphore chopsticks[NUM_PHILOSOPHERS];  // 每个筷子一个信号量
PhilosopherState state[NUM_PHILOSOPHERS]; // 哲学家状态
int eatingCount[NUM_PHILOSOPHERS];       // 记录每个哲学家吃饭的次数

// 延迟函数
void delay(int time) {
    int i = time;
    while (i > 0) --i;
}

// 哲学家线程函数 - 基本版本，可能导致死锁
void philosopher(void *arg) {
    int id = (int)(long)arg;
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;
    
    // 循环：思考-饥饿-进餐
    while (true) {
        // 思考一段时间
        state[id] = THINKING;
        printf("Philosopher %d is thinking...\n", id);
        delay(0x2ffffff);
        
        // 变得饥饿，尝试拿起筷子进餐
        state[id] = HUNGRY;
        printf("Philosopher %d is hungry, trying to pick up chopsticks...\n", id);
        
        // 先尝试拿左边的筷子
        printf("Philosopher %d is trying to pick up left chopstick %d\n", id, left_chopstick);
        chopsticks[left_chopstick].P();
        printf("Philosopher %d picked up left chopstick %d\n", id, left_chopstick);
        
        // 添加短暂延迟，增加死锁的可能性
        delay(0xffffff);
        
        // 然后尝试拿右边的筷子
        printf("Philosopher %d is trying to pick up right chopstick %d\n", id, right_chopstick);
        chopsticks[right_chopstick].P();
        printf("Philosopher %d picked up right chopstick %d\n", id, right_chopstick);
        
        // 进餐
        state[id] = EATING;
        eatingCount[id]++;
        printf("Philosopher %d is eating, this is the %d time...\n", id, eatingCount[id]);
        delay(0x1ffffff);
        
        // 放下筷子
        chopsticks[left_chopstick].V();
        printf("Philosopher %d put down left chopstick %d\n", id, left_chopstick);
        
        chopsticks[right_chopstick].V();
        printf("Philosopher %d put down right chopstick %d\n", id, right_chopstick);
    }
}

// 哲学家线程函数 - 解决死锁的版本
void philosopher_no_deadlock(void *arg) {
    int id = (int)(long)arg;
    int first_chopstick, second_chopstick;
    
    // 打破循环等待条件：让偶数哲学家先拿左边筷子，奇数哲学家先拿右边筷子
    if (id % 2 == 0) {
        first_chopstick = id;  // 左边筷子
        second_chopstick = (id + 1) % NUM_PHILOSOPHERS;  // 右边筷子
    } else {
        first_chopstick = (id + 1) % NUM_PHILOSOPHERS;  // 右边筷子
        second_chopstick = id;  // 左边筷子
    }
    
    // 循环：思考-饥饿-进餐
    while (true) {
        // 思考一段时间
        state[id] = THINKING;
        printf("Philosopher %d (no deadlock version) is thinking...\n", id);
        delay(0xfffffff);
        
        // 变得饥饿，尝试拿起筷子进餐
        state[id] = HUNGRY;
        printf("Philosopher %d (no deadlock version) is hungry, trying to pick up chopsticks...\n", id);
        
        // 尝试拿第一根筷子
        printf("Philosopher %d (no deadlock version) is trying to pick up first chopstick %d\n", id, first_chopstick);
        chopsticks[first_chopstick].P();
        printf("Philosopher %d (no deadlock version) picked up first chopstick %d\n", id, first_chopstick);
        
        // 添加短暂延迟
        delay(0xfffffff);
        
        // 然后尝试拿第二根筷子
        printf("Philosopher %d (no deadlock version) is trying to pick up second chopstick %d\n", id, second_chopstick);
        chopsticks[second_chopstick].P();
        printf("Philosopher %d (no deadlock version) picked up second chopstick %d\n", id, second_chopstick);
        
        // 进餐
        state[id] = EATING;
        eatingCount[id]++;
        printf("Philosopher %d (no deadlock version) is eating, this is the %d time...\n", id, eatingCount[id]);
        delay(0xfffffff);
        
        // 放下筷子
        chopsticks[first_chopstick].V();
        printf("Philosopher %d (no deadlock version) put down first chopstick %d\n", id, first_chopstick);
        
        chopsticks[second_chopstick].V();
        printf("Philosopher %d (no deadlock version) put down second chopstick %d\n", id, second_chopstick);
    }
}

// 测试哲学家就餐问题（可能导致死锁的版本）
void test_dining_philosophers_deadlock() {
    printf("========== Dining Philosophers Problem (Basic version, may deadlock) ==========\n");
    
    // 初始化所有筷子信号量和状态
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        chopsticks[i].initialize(1);  // 每个筷子初始是可用的
        state[i] = THINKING;          // 所有哲学家初始都在思考
        eatingCount[i] = 0;           // 初始化进餐次数为0
    }
    
    // 创建哲学家线程
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        // 直接使用数字作为线程名
        char name[20] = "philosopher_";
        name[12] = '0' + i; // 添加数字(假设i小于10)
        name[13] = '\0';    // 字符串结束符
        programManager.executeThread(philosopher, (void *)(long)i, name, 1);
    }
    
    printf("All philosopher threads have been created, waiting for possible deadlock...\n");
    printf("Note: When all philosophers pick up their left chopsticks simultaneously, they will form a circular wait, causing deadlock\n");
}

// 测试哲学家就餐问题（解决死锁的版本）
void test_dining_philosophers_no_deadlock() {
    printf("========== Dining Philosophers Problem (No deadlock version) ==========\n");
    
    // 初始化所有筷子信号量和状态
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        chopsticks[i].initialize(1);  // 每个筷子初始是可用的
        state[i] = THINKING;          // 所有哲学家初始都在思考
        eatingCount[i] = 0;           // 初始化进餐次数为0
    }
    
    // 创建哲学家线程（无死锁版本）
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        // 直接使用数字作为线程名
        char name[30] = "philosopher_no_deadlock_";
        name[24] = '0' + i; // 添加数字(假设i小于10)
        name[25] = '\0';    // 字符串结束符
        programManager.executeThread(philosopher_no_deadlock, (void *)(long)i, name, 1);
    }
    
    printf("All philosopher threads have been created (no deadlock version)\n");
    printf("This version avoids deadlock by breaking the circular wait condition\n");
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i) {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 解释哲学家就餐问题
    // printf("=== Dining Philosophers Problem ===\n\n");
    // printf("The Dining Philosophers Problem is a classic synchronization problem that describes 5 philosophers sitting around a round table\n");
    // printf("Each philosopher needs to pick up both chopsticks on their left and right to eat\n");
    // printf("The problem is: if all philosophers pick up their left chopstick at the same time, a deadlock will occur\n\n");
    
    // 测试可能导致死锁的版本
    // 注意：取消下面的注释可以观察死锁情况
    // test_dining_philosophers_deadlock();
    
    // 测试无死锁版本
    test_dining_philosophers_no_deadlock();
    
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
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
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
