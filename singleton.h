#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

// 声明一个单例类
template <typename T>
class Singleton{
protected:
    Singleton() = default;
    // 拷贝构造函数 和 拷贝赋值是不允许的，=delete
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator = (const Singleton<T>& st) = delete;

    // 因为是单例类，只返回一个对象，因此就在这个实例instance前面加上static
    static std::shared_ptr<T> _instance;

// 对外提供的
public:
    static std::shared_ptr<T> GetInstance(){
        // 引入一个局部的静态变量
        // static特性：多次的调用，只会被初始化一次
        static std::once_flag s_flag;
        // 回调函数只会被调用一次
        std::call_once(s_flag, [&](){
            // 这里不使用make_shared()进行构造，而使用shared_ptr<>(new T)
            // 原因是make_shared()构造对象需要调用构造函数，而这里的托管对象是单例，构造函数是protected，无法访问受保护的构造函数，new可以在类的内部直接使用
            _instance = std::shared_ptr<T>(new T);

        });
        // 初始化完成之后，返回
        return _instance;
    }

    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton(){
        std::cout << "this is singleton destruct" << std::endl;
    }

};

// 这一步的目的是在类当中，模板类的静态成员变量_instance需要在.h文件中进行初始化
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;


#endif // SINGLETON_H
