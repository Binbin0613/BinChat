#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 将loginDialog放置到主界面当中
    _login_dlg = new LoginDialog(this);
    setCentralWidget(_login_dlg);
    // _login_dlg->show();

    // 创建和注册消息链接
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);


    // 进行初始化
    _reg_dlg = new RegisterDialog(this);

    // 添加页面窗口的边框: 将跳转之后的页面存放到主界面当中去，不加以下两句的话，会跳出两个窗口
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    _reg_dlg->hide();


}

// 析构函数部分，用于回收页面
MainWindow::~MainWindow()
{
    delete ui;

    // 用于回收其他页面
    // if(_login_dlg){
    //     delete _login_dlg;
    //     _login_dlg = nullptr;
    // }
    // if(_reg_dlg){
    //     delete _reg_dlg;
    //     _reg_dlg = nullptr;
    // }
}

void MainWindow::SlotSwitchReg(){
    // 实现的是界面的切换
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();

}
