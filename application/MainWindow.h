﻿#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>
#include <DMenu>

#include "application.h"

DWIDGET_USE_NAMESPACE

class QSignalMapper;

/**
 * @brief The MainWindow class
 * @brief 不做任何的业务处理， 只发送信号， 对应的模块处理相应的业务
 */

class MainWindow : public DMainWindow, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(DMainWindow *parent = nullptr);
    ~MainWindow() override;

private:
    static MainWindow *g_onlyMainWindow;
public:
    static MainWindow *Instance();

    void openfile(const QString &filepath);
    void setSreenRect(const QRect &); //得到屏幕的分辨率

public:
    void SetSliderShowState(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *ev) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void initUI();
//    void initConnections();

//    void onSetAppTitle(const QString &);
    void onAppExit();

    void initThemeChanged();
    void setCurTheme();
    void displayShortcuts();

    void notifyMsg(const int &, const QString &msgContent = "") override;

    void showDefaultSize();

    void initShortCut();

private slots:
    void slotShortCut(const QString &);

private:
    QList<int>      m_pMsgList;
    Qt::WindowStates    m_nOldState = Qt::WindowNoState;        //  旧的窗口状态
};

#endif // MainWindow_H
