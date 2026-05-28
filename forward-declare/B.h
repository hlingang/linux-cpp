#ifndef __B_H__
#define __B_H__
/*DO not want to include B.h, use forward declare instead */
/*避免头文件循环引用的问题 */
class A;  // 前置声明
class B
{
public:
    void test( A& __a );  // 声明和实现分离，避免头文件循环引用的问题
};

#endif  // __B_H__