#include <cache.h>
#include <vector>

using namespace std;

struct TIM
{
  int id;
  int val;
  char name[32];
};

int main()
{
  KmemCache<TIM> cache(1);
  vector<TIM *> vec;
  int num = cache.get_num();
  printf("cache num = %d\n", num);
  for (int i = 0; i < 3 * num; i++)
  {
    TIM *tim = cache.kmem_alloc();
    tim->id = i;
    tim->val = i + 1;
    snprintf(tim->name, sizeof(tim->name), "tim->%d:%d", i, i % num);
    vec.push_back(tim);
    cache.print_size();
    printf("id = %d\n", i + 1);
    printf("---------------------------\n");
  }
  cache.dump();
  printf("\n======================\n");
  for (int i = 0; i < 2 * num + 10; i++)
  {
    cache.kmem_free(vec[i]);
    cache.print_size();
    printf("id = %d\n", i + 1);
    printf("---------------------------\n");
  }
  cache.dump();
  return 0;
}