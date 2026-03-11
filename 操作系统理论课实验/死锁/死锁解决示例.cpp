#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <string>

class Account {
private:
    int id;
    double balance;
    std::mutex mutex;

public:
    Account(int id, double initialBalance) : id(id), balance(initialBalance) {}

    int getId() const { return id; }

    void withdraw(double amount) {
        balance -= amount;
        // 模拟操作延时
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void deposit(double amount) {
        balance += amount;
        // 模拟操作延时
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    double getBalance() const { return balance; }

    std::mutex& getMutex() { return mutex; }
};

// 错误的实现 - 可能导致死锁
void unsafeTransaction(Account& from, Account& to, double amount, const std::string& threadName) {
    std::cout << threadName << " 尝试获取第一个锁 (账户 " << from.getId() << ")\n";
    std::lock_guard<std::mutex> lockFrom(from.getMutex());
    
    // 模拟一些操作延时，增加死锁可能性
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << threadName << " 尝试获取第二个锁 (账户 " << to.getId() << ")\n";
    std::lock_guard<std::mutex> lockTo(to.getMutex());
    
    std::cout << threadName << " 成功获取两个锁，执行转账\n";
    from.withdraw(amount);
    to.deposit(amount);
    
    std::cout << threadName << " 完成转账: " << amount << " 从账户 " 
              << from.getId() << " 到账户 " << to.getId() << "\n";
}

// 安全实现 - 避免死锁
void safeTransaction(Account& from, Account& to, double amount, const std::string& threadName) {
    // 按账户ID排序，确保统一的锁获取顺序
    Account *first = &from;
    Account *second = &to;
    bool reversed = false;
    
    if (from.getId() > to.getId()) {
        first = &to;
        second = &from;
        reversed = true;
    }
    
    std::cout << threadName << " 尝试获取账户ID较小的锁 (账户 " << first->getId() << ")\n";
    std::lock_guard<std::mutex> lockFirst(first->getMutex());
    
    // 模拟操作延时
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << threadName << " 尝试获取账户ID较大的锁 (账户 " << second->getId() << ")\n";
    std::lock_guard<std::mutex> lockSecond(second->getMutex());
    
    std::cout << threadName << " 成功获取两个锁，执行转账\n";
    
    if (!reversed) {
        from.withdraw(amount);
        to.deposit(amount);
    } else {
        to.withdraw(amount);
        from.deposit(amount);
    }
    
    std::cout << threadName << " 完成转账: " << amount 
              << (reversed ? " 从账户 " + std::to_string(to.getId()) + " 到账户 " + std::to_string(from.getId())
                           : " 从账户 " + std::to_string(from.getId()) + " 到账户 " + std::to_string(to.getId()))
              << "\n";
}

// 另一种更简洁的安全实现 - 使用C++标准库的std::lock函数
void safeTransactionWithStdLock(Account& from, Account& to, double amount, const std::string& threadName) {
    std::cout << threadName << " 尝试获取两个锁\n";
    
    // std::lock自动以避免死锁的方式锁定多个互斥量
    std::unique_lock<std::mutex> lockFrom(from.getMutex(), std::defer_lock);
    std::unique_lock<std::mutex> lockTo(to.getMutex(), std::defer_lock);
    std::lock(lockFrom, lockTo);
    
    std::cout << threadName << " 成功获取两个锁，执行转账\n";
    from.withdraw(amount);
    to.deposit(amount);
    
    std::cout << threadName << " 完成转账: " << amount << " 从账户 " 
              << from.getId() << " 到账户 " << to.getId() << "\n";
}

// 演示死锁的函数
void demonstrateDeadlock() {
    Account checkingAccount(1, 1000);
    Account savingsAccount(2, 2000);
    
    std::cout << "=== 死锁演示（可能会卡住） ===\n";
    std::cout << "初始余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
    
    // 创建两个可能导致死锁的线程
    std::thread t1(unsafeTransaction, std::ref(checkingAccount), std::ref(savingsAccount), 
                   500, "线程1 (checking->savings)");
    std::thread t2(unsafeTransaction, std::ref(savingsAccount), std::ref(checkingAccount), 
                   300, "线程2 (savings->checking)");
    
    t1.join();
    t2.join();
    
    std::cout << "最终余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
}

// 演示解决方案的函数
void demonstrateSolution() {
    Account checkingAccount(1, 1000);
    Account savingsAccount(2, 2000);
    
    std::cout << "\n=== 解决方案演示（按ID排序） ===\n";
    std::cout << "初始余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
    
    // 创建两个安全的线程
    std::thread t1(safeTransaction, std::ref(checkingAccount), std::ref(savingsAccount), 
                   500, "线程1 (checking->savings)");
    std::thread t2(safeTransaction, std::ref(savingsAccount), std::ref(checkingAccount), 
                   300, "线程2 (savings->checking)");
    
    t1.join();
    t2.join();
    
    std::cout << "最终余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
}

// 演示使用std::lock的解决方案
void demonstrateStdLockSolution() {
    Account checkingAccount(1, 1000);
    Account savingsAccount(2, 2000);
    
    std::cout << "\n=== 解决方案演示（使用std::lock） ===\n";
    std::cout << "初始余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
    
    // 创建两个使用std::lock的安全线程
    std::thread t1(safeTransactionWithStdLock, std::ref(checkingAccount), std::ref(savingsAccount), 
                   500, "线程1 (checking->savings)");
    std::thread t2(safeTransactionWithStdLock, std::ref(savingsAccount), std::ref(checkingAccount), 
                   300, "线程2 (savings->checking)");
    
    t1.join();
    t2.join();
    
    std::cout << "最终余额 - 账户1: " << checkingAccount.getBalance() 
              << ", 账户2: " << savingsAccount.getBalance() << "\n";
}

int main() {
    std::cout << "死锁问题解决方案演示\n";
    std::cout << "------------------------\n\n";
    
    // 注意：死锁演示可能会导致程序卡住
    // 如果程序卡住，请按Ctrl+C终止
    // demonstrateDeadlock();
    
    // 演示两种解决方案
    demonstrateSolution();
    demonstrateStdLockSolution();
    
    return 0;
} 