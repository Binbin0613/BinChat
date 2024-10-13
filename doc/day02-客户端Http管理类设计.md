## 完善注册类界面
先在注册类构造函数里添加lineEdit的模式为密码模式
``` cpp
ui->lineEdit_Passwd->setEchoMode(QLineEdit::Password);
ui->lineEdit_Confirm->setEchoMode(QLineEdit::Password);
```
我们在注册界面的ui里添加一个widget，widget内部包含一个tip居中显示，用来提示错误。设置label的显示为文字居中。

![https://cdn.llfc.club/1709103910427.jpg](https://cdn.llfc.club/1709103910427.jpg)

我们在qss里添加err_tip样式，根据不同的状态做字体显示
``` qss
#err_tip[state='normal']{
   color: green;
}

#err_tip[state='err']{
   color: red;
}
```
接下来项目中添加global.h和global.cpp文件，global.h声明repolish函数，global.cpp用来定义这个函数。

.h中的声明
``` cpp
#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
extern std::function<void(QWidget*)> repolish;

#endif // GLOBAL_H
```

.cpp中的定义

``` cpp
#include "global.h"

std::function<void(QWidget*)> repolish =[](QWidget *w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

```
在Register的构造函数中添加样式设置。
``` cpp
ui->err_tip->setProperty("state","normal");
repolish(ui->err_tip);
```
接下来实现获取验证码的逻辑,ui里关联get_code按钮的槽事件，并实现槽函数
``` cpp
void RegisterDialog::on_get_code_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"));
    }
}
```
在RegisterDialog中添加showTip函数
``` cpp
void RegisterDialog::showTip(QString str)
{
    ui->err_tip->setText(str);
    ui->err_tip->setProperty("state","err");
    repolish(ui->err_tip);
}
```
好了，这样就完成了。测试提示功能正确，下面要实现判断邮箱正确后发送http请求。
## 单例类封装
网络请求类要做成一个单例类，这样方便在任何需要发送http请求的时候调用，我们先实现单例类,添加singleton.h实现如下
``` cpp
#include <memory>
#include <mutex>
#include <iostream>
using namespace std;
template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;

    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = shared_ptr<T>(new T);
            });

        return _instance;
    }
    void PrintAddress() {
        std::cout << _instance.get() << endl;
    }
    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
```
## http管理类
http管理类主要用来管理http发送接收等请求得，我们需要在pro中添加网络库
``` cpp
QT       += core gui network
```
在pro中添加C++类，命名为HttpMgr，然后头文件如下
``` cpp
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include "global.h"
#include <memory>
#include <QJsonObject>
#include <QJsonDocument>
class HttpMgr:public QObject, public Singleton<HttpMgr>,
        public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;
signals:
    void sig_http_finish();
};
```
我们先实现PostHttpReq请求的函数，也就是发送http的post请求, 发送请求要用到请求的url，请求的数据(json或者protobuf序列化)，以及请求的id，以及哪个模块发出的请求mod，那么一个请求接口应该是这样的
``` cpp
void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
```
我们去global.h定义ReqId枚举类型
``` cpp
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};
```

在global.h定义ErrorCodes
``` cpp
enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};
```
在global.h中定义模块
``` cpp
enum Modules{
    REGISTERMOD = 0,
};
```

还需要修改下要发送的信号，在HttpMgr的头文件里，让他携带参数
``` cpp
void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
```

我们实现PostHttpReq
``` cpp
void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //创建一个HTTP POST请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    //通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    //发送请求，并处理响应, 获取自己的智能指针，构造伪闭包并增加智能指针引用计数
    auto self = shared_from_this();
    QNetworkReply * reply = _manager.post(request, data);
    //设置信号和槽等待发送完成
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        //处理错误的情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        //无错误则读回请求
        QString res = reply->readAll();

        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS,mod);
        reply->deleteLater();
        return;
    });
}
```
加下来HttpMgr内实现一个slot_http_finish的槽函数用来接收sig_http_finish信号。
``` cpp
void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        //发送信号通知指定模块http响应结束
        emit sig_reg_mod_finish(id, res, err);
    }
}
```

我们在HttpMgr.h中添加信号sig_reg_mod_finish，

``` cpp
class HttpMgr:public QObject, public Singleton<HttpMgr>,
        public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
   //...省略
signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};
```
并且在cpp文件中连接slot_http_finish和sig_http_finish.
``` cpp
HttpMgr::HttpMgr()
{
    //连接http请求和完成信号，信号槽机制保证队列消费
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}
```
我们在注册界面连接sig_reg_mod_finish信号
``` cpp
RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    //省略...
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
}
```
接下俩实现slot_reg_mod_finish函数
``` cpp
void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    //调用对应的逻辑
    return;
}
```
showTip逻辑稍作修改，增加bool类型参数
``` cpp
void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
         ui->err_tip->setProperty("state","err");
    }else{
        ui->err_tip->setProperty("state","normal");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}
```
## 注册消息处理
我们需要对RegisterDialog注册消息处理，头文件声明
``` cpp
QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
```
在RegisterDialog中添加注册消息处理的声明和定义
``` cpp
void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });
}
```
回到slot_reg_mod_finish函数添加根据id调用函数处理对应逻辑
``` cpp
void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
   //前面逻辑省略...
    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}
```