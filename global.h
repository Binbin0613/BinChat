#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include "QStyle"


// 在全局的头文件里面定义常用的属性等


/**
 * @brief repolish: 用于刷新qss的函数
 */
extern std::function<void(QWidget*)> repolish;




#endif // GLOBAL_H
