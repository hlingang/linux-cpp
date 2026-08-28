#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <stddef.h>
#include <map>
#include <string>
#include <string.h>
#include <memory>
#include <iostream>

struct UTest
{
    union U
    {
        int a;
        std::string b;
        U() : a() {}
        ~U() {}
    } v;
    UTest() : v() {};
};

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
};

struct node
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
        _V() = default;
        _V(int __i) : i(__i) {}
        _V(double __d) : d(__d) {}
        _V(float __f) : f(__f) {}
        _V(const char *__c)
        {
            auto __len = strlen(__c);
            __dupstr(__c, __len);
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
    std::map<CZ, node> *node_map;
    node() : v(), d_type(v_null), node_map(nullptr) {}
    node(const node &oth)
    {
        *this = oth;
    }
    ~node()
    {
        if (d_type == v_s)
        {
            if (v.str.c && v.str.c != v.str.cache)
            {
                free(v.str.c);
                v.str.c = nullptr;
                v.str.len = 0;
            }
        }
        else if (d_type == v_o)
        {
            if (node_map)
            {
                __dispose_map(node_map);
                __destroy_map(node_map);
            }
        }
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
        if (!node_map)
            node_map = new std::map<CZ, node>;
        if (node_map->find(cz) == node_map->end())
            node_map->operator[](cz) = node();
        return node_map->operator[](cz); // 此处返回空对象
    }
    void __freestr()
    {
        if (d_type == v_s && (v.str.c && v.str.c != v.str.cache))
        {
            free(v.str.c);
            v.str.c = nullptr;
            v.str.len = 0;
        }
    }
    void __dupstr(const node &oth)
    {
        return v.__dupstr(oth.v.str.c, oth.v.str.len);
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
            // 此处需要清理旧的对象
            __dispose_map(node_map);
            auto *temp = new std::map<CZ, node>(*oth.node_map);
            node_map = temp;
        }
        return *this;
    }
    const std::string to_string() const
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
            for (const auto &val : *node_map)
                ss << val.first.to_string() << ":" << val.second.to_string() << ",";
            if (ss.tellp() > 1)
                ss.seekp((size_t)ss.tellp() - 1);
            ss << "}";
        }
        return ss.str();
    }
    static void __dispose_map(std::map<CZ, node> *__node_map)
    {
        if (__node_map)
            __node_map->clear(); // 自动调用对象析构函数
    }
    static void __destroy_map(std::map<CZ, node> *__node_map)
    {
        delete __node_map;
    }
    static void __destroy_node(node __node)
    {
        __node.~node();
    }
};