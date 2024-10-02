#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 点击reg_btn会产生clicked信号，this（loginDialog会接受到信号），接受信号后会发送switchRegister信号到mainwindow
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}
