#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include <DSplitter>
#include "FileDataModel.h"
#include "docview/commonstruct.h"

DWIDGET_USE_NAMESPACE

class SpinnerWidget;
class SheetBrowserPDF;
class SheetSidebar;
class DocummentProxy;
class SheetBrowserArea;
class DocSheet : public DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(DocSheet)

public:
    explicit DocSheet(DocType_EM type, DWidget *parent = nullptr);
    ~DocSheet() override;

signals:
    void sigOpenFileResult(const QString &, const bool &);

    void sigFileChanged(bool hasChanged);     //被修改了

public:
    void setFileChanged(bool hasChanged);

    QString qGetPath();

    void qSetPath(const QString &strPath);

    int qGetFileChange();

    void saveData();

    FileDataModel qGetFileData();

    DocummentProxy *getDocProxy();

    void OnOpenSliderShow();

    void OnExitSliderShow();

    void ShowFindWidget();

    DocType_EM type();

//    bool close();

//    bool save();

//    bool saveAs(QString filePath);

private:
    void initPDF();

private slots:
    void SlotFileOpenResult(const QString &, const bool &);

    void SlotNotifyMsg(const int &, const QString &);

private:
    DocType_EM      m_type;
    void *m_sidebar = nullptr;
    void *m_browser = nullptr;

    QStackedWidget *m_pRightWidget = nullptr;
    SpinnerWidget  *m_pSpinnerWidget = nullptr;
    bool            m_bOldState = false;
};

#endif // MAINSPLITTER_H