#ifndef __SINGLETEST_H_
#define __SINGLETEST_H_
#include "singleton.h"
class SingleTest : public Singleton< SingleTest >
{

public:
    void test();

public:
    SingleTest() {}
    ~SingleTest() {}
};
#endif