#ifndef _SINGLETEST_H_
#define _SINGLETEST_H
#include "singleton.h"
class SingleTest : public Singleton<SingleTest>
{
    friend class Singleton<SingleTest>;

public:
    void test();

private:
    SingleTest() {}
    ~SingleTest() {}
};
#endif