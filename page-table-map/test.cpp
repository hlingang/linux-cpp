#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include <stdarg.h>

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////
struct page_t
{
    char data[64];
};

#define PMD_SHIFT 3
#define PGD_SHIFT 6
#define PTE_MASK 0b111

using pgd_t = struct {unsigned long pgd;};
using pmd_t = struct {unsigned long pmd;};
using pte_t = struct {unsigned long pte;};

#define pgd_val(val) (val).pgd 
#define pmd_val(val) (val).pmd
#define pte_val(val) (val).pte

#define pgd_none(val) (!pgd_val(val))
#define pmd_none(val) (!pmd_val(val))
#define pte_none(val) (!pte_val(val))

pgd_t * g_pgd;
///////////////////////////////////////////////////////////////////

pgd_t *pgd_offset(pgd_t* pgd, unsigned long addr, unsigned long* pos = nullptr)
{
    if( !pgd )
    {
        return nullptr;
    }
    unsigned long index = (addr >> PGD_SHIFT) & PTE_MASK; 
    if( pos )
        *pos = index;
    return g_pgd + index;
}

pmd_t *pmd_offset(pgd_t* pgd, unsigned long addr, unsigned long* pos = nullptr)
{
    if(pgd_none(*pgd))
    {
        pmd_t* __pmd =  (pmd_t*)malloc(sizeof(page_t));
        memset(__pmd, 0x00, sizeof(page_t));
        *pgd = (pgd_t){.pgd = (unsigned long)__pmd}; // C 语言构建初始化结构体
    }
    unsigned long index = (addr >> PMD_SHIFT) & PTE_MASK; 
    if( pos )
        *pos = index;
    return  (pmd_t*)pgd_val(*pgd) + index;
}

pte_t *pte_offset(pmd_t* pmd, unsigned long addr, unsigned long* pos = nullptr)
{
    if(pmd_none(*pmd))
    {
        pte_t* __pte =  (pte_t*)malloc(sizeof(page_t));
        memset(__pte, 0x00, sizeof(page_t));
        *pmd = (pmd_t){.pmd = (unsigned long)__pte}; // C 语言构建初始化结构体
    }
    unsigned long index = addr & PTE_MASK; 
    if( pos )
        *pos = index;
    return (pte_t*)pmd_val(*pmd) + index;
}


int main()
{
    g_pgd = (pgd_t *)malloc(sizeof(page_t));
    memset(g_pgd, 0x00, sizeof(page_t));
    unsigned long i_pgd, i_pmd, i_pte;
    for(int i = 0; i < 512; i++)
    {
        pgd_t *pgd = pgd_offset(g_pgd, i, &i_pgd); 
        pmd_t *pmd = pmd_offset(pgd, i, &i_pmd );
        pte_t *pte = pte_offset(pmd, i, &i_pte );
        void* page = (void*)malloc(sizeof(page_t));
        memset(page, 0x00, sizeof(page_t));
        unsigned long * ptr = (unsigned long *)page;
        *ptr = i+1;
        *(ptr +1) = i + 512;
        *pte = (pte_t){(unsigned long)page};
        printf("VAL:%d, PGD[%lu]: %lx, PMD[%lu]: %lx, PTE[%lu]: %lx\n",i, i_pgd, pgd_val(*pgd), i_pmd, pmd_val(*pmd), i_pte, pte_val(*pte));
    }
    for(int i = 0; i < 512; i++)
    {
        pgd_t *pgd = pgd_offset(g_pgd, i);
        pmd_t *pmd = pmd_offset(pgd, i);
        pte_t *pte = pte_offset(pmd, i);
        unsigned long *ptr = (unsigned long *)pte_val(*pte);
        printf("i:%d, pte=%p, val=%lu/%lu(delta:%lu)\n", i, (void*)pte_val(*pte), *ptr, *(ptr + 1), *(ptr+1) - *ptr);
    }
    return 0;
}