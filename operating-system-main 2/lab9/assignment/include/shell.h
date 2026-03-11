#ifndef SHELL_H
#define SHELL_H

class Shell
{
public:
    Shell();
    void initialize();
    void run();
private:
    void printLogo();
    void testMemoryAllocation(); // 测试内存分配
};

#endif