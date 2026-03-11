#include <iostream>
#include <stdarg.h>
#include<string>

using namespace std;

void print_any_number_of_integers(int n, ...);
void print_any_number_of_chars(int n, ...);
void print_any_number_of_strings(int n, ...);

int main()
{

    print_any_number_of_integers(3, 487, -12, 0);
    print_any_number_of_chars(3,'a','b','c');
    print_any_number_of_strings(3, "I am","a","person.");

}

void print_any_number_of_integers(int n, ...)
{
    // 定义一个指向可变参数的指针parameter
    va_list parameter;
    // 使用固定参数列表的最后一个参数来初始化parameter
    // parameter指向可变参数列表的第一个参数
    va_start(parameter, n);

    for ( int i = 0; i < n; ++i ) {
        // 引用parameter指向的int参数，并使parameter指向下一个参数
        std::cout << va_arg(parameter, int) << " ";
    }
    
    // 清零parameter
    va_end(parameter);

    std::cout << std::endl;
}

void print_any_number_of_chars(int n, ...)
{
    va_list parameter;
    va_start(parameter, n);

    for ( int i = 0; i < n; ++i ) {
        // 使用 va_arg 获取下一个参数，类型为 int
        // 因为 char 类型的参数在传递时会被提升为 int
        // 所以这里使用 int 类型并将其转换为 char
        std::cout << static_cast<char>(va_arg(parameter, int)) << " "; 
    }
    
    va_end(parameter);
    std::cout << std::endl;
}

void print_any_number_of_strings(int n, ...)
{
    va_list parameter;
    va_start(parameter, n);

    for ( int i = 0; i < n; ++i ) {
        // 使用 va_arg 获取下一个参数，类型为 char*
        // 这样可以正确获取传递的字符串参数
        std::cout << va_arg(parameter, char*) << " "; 
    }
    
    va_end(parameter);
    std::cout << std::endl;
}