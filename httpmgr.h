#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
// qt中对JSON进行序列化和反序列化的工具
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @brief The HttpMgr class
 *
 *
 * 学习C++当中的CRTP
 */

// C++支持多重继承，这里继承Singleton的HttpMgr类型，这里是可以这样使用的-CRTP
class HttpMgr:public QObject,
              public Singleton<HttpMgr>,
              public std::enable_shared_from_this<HttpMgr>{
    // 定义一个宏
    Q_OBJECT
public:
    ~HttpMgr();

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;
    // 定义发送数据的函数
    /**
     * @brief PostHttpReq
     * @param url       地址
     * @param json      json格式的内容
     * @param req_id    请求的id
     * @param mod       请求的模块
     */
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);


// 编写槽函数
private slots:
    // 槽函数接受的参数需要与信号接收到的参数相匹配, 信号的参数 >= 槽函数的参数
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

};


#endif // HTTPMGR_H
