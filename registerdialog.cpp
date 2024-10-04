#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    // 为几个组件设计输入模式wei password 不可见的文本框
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);


    // 设置err_tip的两种状态属性
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    // 连接http这边发的通知
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_http_finish,
            this, &RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

// 在ui文件当中右击"转到槽"，选择click事件，即会跳转到当前函数位置上
void RegisterDialog::on_get_code_clicked()
{
    // 首先获取文本内容
    auto email = ui->email_edit->text();
    // 编写正则表达式，用于校验邮箱的内容是否符合邮箱的格式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");

    // 进行匹配
    bool match = regex.match(email).hasMatch();

    // 验证是否匹配成功
    if(match){
        // 发送http验证码
    }else{
        // 显示邮箱输入匹配失败的提示
        showTip(tr("邮箱地址输入有误！"), false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        return;
    }
    // 解析数据的json内容: res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"), false);
        return;
    }
    // json 解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析失败"), false);
        return;
    }
    // 正确的获取数据
    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::initHttpHandlers()
{
    // 注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送至邮箱，请查收"), true);
        qDebug() << "email is :" << email;
    });
}

// 该函数在registerdialog.h文件中进行声明，在此处进行实现
void RegisterDialog::showTip(QString str, bool b_ok)
{
    // 设置err_tip中的内容，调整qss中err_tip的状态，然后刷新qss
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

