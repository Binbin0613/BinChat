#include "global.h"

// 在cpp文件中进行函数的实现
std::function<void(QWidget*)> repolish = [](QWidget* w){
    // 将原来的样式个卸载掉
    w->style()->unpolish(w);

    // 刷新：重新加载新的样式内容
    w->style()->polish(w);
};
