#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <stddef.h>
#include <map>
#include <string>
#include <string.h>
#include <memory>
#include <iostream>

#define list_node(parent, p) for (p = (parent)->child; p; p = p->next)
enum
{
    cz_int,
    cz_str,
};
enum
{
    v_null,
    v_i,
    v_d,
    v_f,
    v_s,
    v_o
};
struct CZ
{
    int index;
    std::string key;
    int d_type;
    CZ() : index(0), d_type(cz_int) {}
    CZ(const char *__key) : key(__key), d_type(cz_str) {}
    CZ(int __index) : index(__index), d_type(cz_int) {}
    bool operator<(const CZ &oth) const
    {
        if (d_type != oth.d_type)
            return d_type < oth.d_type;
        if (d_type == cz_int)
            return index < oth.index;
        return key < oth.key;
    }
    std::string to_string() const
    {
        std::stringstream ss;
        if (d_type == cz_int)
            ss << index;
        else
            ss << "\"" << key << "\"";
        return ss.str();
    }
    bool operator==(const CZ &oth) const
    {
        if (d_type != oth.d_type)
            return false;
        if (d_type == cz_int)
            return index == oth.index;
        return key == oth.key;
    }
};

struct node_base
{
    node_base *parent;
    node_base *child;
    node_base *next;
    CZ zky;
    node_base() : parent(nullptr), child(nullptr), next(nullptr) {}
    virtual ~node_base() {}
};

inline node_base *find_node(const node_base *parent, const CZ &key)
{
    node_base *p;
    list_node(parent, p)
    {
        if (p->zky == key)
            return p;
    }
    return nullptr;
}
inline int add_node(node_base *parent, node_base *__child)
{
    if (nullptr == parent || nullptr == __child)
        return 0;
    node_base **p = &parent->child;
    __child->parent = parent;
    __child->next = *p;
    *p = __child;
    return 1;
}

inline node_base *del_node(node_base *parent)
{
    node_base *ret = parent->child;
    if (ret)
    {
        parent->child = ret->next;
        return ret;
    }
    return nullptr;
}

struct node : public node_base
{
    union _V
    {
        int i;
        double d;
        float f;
        struct
        {
            char cache[32];
            char *c;
            size_t len;
        } str;
        _V() : str() {}; // 按字段进行值初始化
        _V(int __i) : i(__i) {}
        _V(double __d) : d(__d) {}
        _V(float __f) : f(__f) {}
        _V(const char *__c)
        {
            __dupstr(__c, strlen(__c));
        }
        void __dupstr(const char *__c, size_t __len)
        {
            if (nullptr == __c || 0 == __len)
                return;
            if (__len >= sizeof(v.str.cache))
            {
                auto *__p = (char *)malloc(__len);
                memcpy(__p, __c, __len);
                __p[__len] = '\0';
                str.c = __p;
                str.len = __len;
            }
            else
            {
                memcpy(str.cache, __c, __len);
                str.cache[__len] = '\0';
                str.c = str.cache;
                str.len = __len;
            }
        }
    } v;
    int d_type;
    node() : v(), d_type(v_null) {}
    ~node()
    {
        __freestr();
        __freemap();
    }
    // 拷贝构造
    node(const node &oth)
    {
        *this = oth;
    }
    node(int __i) : d_type(v_i), v(__i) {}
    node(double __d) : d_type(v_d), v(__d) {}
    node(float __f) : d_type(v_f), v(__f) {}
    node(const char *__c) : d_type(v_s), v(__c) {}
    node &operator[](const CZ &cz)
    {
        if (d_type == v_null)
            d_type = v_o;
        if (d_type != v_o)
            throw std::runtime_error("type_error");
        node_base *ret = nullptr;
        if ((ret = find_node(this, cz)) == nullptr)
        {
            ret = new node();
            ret->zky = cz;
            add_node(this, ret);
        }
        return *static_cast<node *>(ret);
    }
    node &operator=(const node &oth)
    {
        __freestr();
        d_type = oth.d_type;
        if (d_type == v_null)
            v = oth.v;
        else if (d_type == v_i)
            v.i = oth.v.i;
        else if (d_type == v_d)
            v.d = oth.v.d;
        else if (d_type == v_f)
            v.f = oth.v.f;
        else if (d_type == v_s)
            __dupstr(oth);
        else
        {
            // 此处需要清理旧的对象(需手动处理指针对象)
            __dispose_map(this);
            node_base *__p;
            list_node(&oth, __p)
            {
                CZ key = __p->zky;
                (*this)[key] = *static_cast<node *>(__p);
            }
        }
        return *this;
    }
    const std::string to_string()
    {
        std::stringstream ss;
        if (d_type == v_null)
            ss << "";
        else if (d_type == v_i)
            ss << v.i;
        else if (d_type == v_d)
            ss << v.d;
        else if (d_type == v_f)
            ss << v.f;
        else if (d_type == v_s)
            ss << "\"" << v.str.c << "\"";
        else if (d_type == v_o)
        {
            ss << "{";
            node_base *__p = this->child;
            while (__p)
            {
                ss << __p->zky.to_string() << ":" << static_cast<node *>(__p)->to_string() << ",";
                __p = __p->next;
            }
            if (ss.tellp() > 1)
                ss.seekp((size_t)ss.tellp() - 1);
            ss << "}";
        }
        return ss.str();
    }
    int remove(const CZ &key)
    {
        node_base **__p = &this->child;
        node_base *ret = nullptr;
        while (*__p)
        {
            if ((*__p)->zky == key)
            {
                ret = *__p;
                *__p = (*__p)->next;
                delete ret;
                return 1;
            }
            __p = &(*__p)->next;
        }
        return 0;
    }
    int __freestr()
    {
        if (d_type == v_s && (v.str.c && v.str.c != v.str.cache))
        {
            free(v.str.c);
            v.str.c = nullptr;
            v.str.len = 0;
            return 1;
        }
        return 0;
    }
    int __freemap()
    {
        if (d_type == v_o && nullptr != this->child)
        {
            __dispose_map(this);
            return 1;
        }
        return 0;
    }

    void __dupstr(const node &oth)
    {
        return v.__dupstr(oth.v.str.c, oth.v.str.len);
    }
    static void __dispose_map(node_base *parent)
    {
        while (parent->child)
        {
            auto ret = del_node(parent);
            if (ret)
                delete ret; // 调用析构函数
        }
    }
};