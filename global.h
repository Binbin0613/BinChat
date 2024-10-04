#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include "QStyle"

#include <memory>       // 可能会使用到智能指针
#include <iostream>     // 可能会使用到输入输出
#include <mutex>        // 可能会使用到互斥量

#include <QByteArray>
#include <QNetworkReply>


// 在全局的头文件里面定义常用的属性等


/**
 * @brief repolish: 用于刷新qss的函数
 */
extern std::function<void(QWidget*)> repolish;

/**
 * @brief The ReqId enum     相应的服务项
 */
enum ReqId{
    ID_GET_VARIFY_CODE = 1001,      // 获取验证码服务
    ID_REG_USER        = 1002,      // 注册用户服务
};

/**
 * @brief The Modules enum    相应的服务类型
 */
enum Modules{
    REGISTERMOD     = 0,            // 注册模块
};

/**
 * @brief The ErrorCodes enum   生成错误码
 * 一种常用的场景就是在后台会给出清晰的定义，但是返回给前端的时候给出较模糊的场景，防止被其他人爆破
 */
enum ErrorCodes{
    SUCCESS     =  0,
    ERR_JSON    =  1,       // json解析失败
    ERR_NETWORK =  2,       // 网络错误
};


#endif // GLOBAL_H
