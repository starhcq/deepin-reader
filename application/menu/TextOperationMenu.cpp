#include "TextOperationMenu.h"

#include <DFontSizeManager>
#include <DGuiApplicationHelper>

#include "ColorWidgetAction.h"

#include "business/AppInfo.h"

#include "gof/bridge/IHelper.h"
#include "widgets/main/MainTabWidgetEx.h"

TextOperationMenu::TextOperationMenu(DWidget *parent)
    : CustomMenu("TextOperationMenu", parent)
{
    initActions();
}

void TextOperationMenu::execMenu(const QPoint &showPoint, const bool &bHigh, const QString &sSelectText, const QString &sUuid)
{
    MainTabWidgetEx *pMtwe = MainTabWidgetEx::Instance();
    QString sCurPath = pMtwe->qGetCurPath();

    QList<int> pageList = dApp->m_pDBService->getBookMarkList(sCurPath);

    bool bBookState = pageList.contains(m_nClickPage);
    m_pAddBookMark->setEnabled(!bBookState);

    m_strSelectText = sSelectText;
    if (m_strSelectText == "") {
        m_pCopy->setEnabled(false);
    } else {
        m_pCopy->setEnabled(true);
    }

    if (m_nType == NOTE_ICON) {
        m_pRemoveHighLight->setText(tr("Remove annotation"));
        m_pAddNote->setText(tr("Add icon note"));
    } else if (m_nType == NOTE_HIGHLIGHT) {
        m_pRemoveHighLight->setText(tr("Remove highlight"));
        m_pAddNote->setText(tr("Add highlight note"));
    }
    m_strNoteUuid = sUuid;
    m_pRemoveHighLight->setEnabled(bHigh);

    m_pColorWidgetAction->setBtnAddLightState(bHigh);

    //  当前显示状态状态
//    int nState = dApp->m_pAppInfo->qGetCurShowState();
//    if (nState == FILE_FULLSCREEN) {
//        m_pExitFullScreen->setVisible(true);
//    } else {
//        m_pExitFullScreen->setVisible(false);
//    }

    this->exec(showPoint);
}

void TextOperationMenu::setClickPoint(const QPoint &clickPoint)
{
    m_pClickPoint = clickPoint;
}

void TextOperationMenu::setClickPage(int nClickPage)
{
    m_nClickPage = nClickPage;
}

void TextOperationMenu::initActions()
{
    m_pCopy = createAction(tr("Copy"), SLOT(slotCopyClicked()));

    m_pColorWidgetAction = new ColorWidgetAction(this);
    connect(m_pColorWidgetAction, SIGNAL(sigBtnGroupClicked(const int &)), this, SLOT(slotSetHighLight(const int &)));
//    connect(m_pColorWidgetAction, SIGNAL(sigBtnDefaultClicked()), this, SLOT(slotAddHighLightClicked()));
    this->addAction(m_pColorWidgetAction);
    this->addSeparator();

    m_pRemoveHighLight = createAction(tr("Remove highlight"), SLOT(slotRemoveHighLightClicked()));
    m_pAddNote = createAction(tr("Add note"), SLOT(slotAddNoteClicked()));

    m_pAddBookMark = createAction(tr("Add bookmark"), SLOT(slotAddBookMarkClicked()));
//    m_pExitFullScreen = createAction(tr("Exit fullscreen"), SLOT(slotExitFullScreenClicked()));
}

QAction *TextOperationMenu::createAction(const QString &text, const char *member)
{
    auto action = new  QAction(text, this);
    connect(action, SIGNAL(triggered()), member);
    this->addAction(action);
    this->addSeparator();

    return action;
}

void TextOperationMenu::notifyMsgToFrame(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
    this->hide();
}

void TextOperationMenu::slotSetHighLight(const int &nColor)
{
    m_pLightColor = nColor;

    bool bEnable = m_pRemoveHighLight->isEnabled();
    if (bEnable) {  //  移除高亮可以点,说明已有高亮, 点击操作 是 更新高亮
        QString sContent = QString::number(m_pLightColor) + Constant::sQStringSep +
                           m_strNoteUuid + Constant::sQStringSep +
                           QString::number(m_nClickPage);
        dApp->m_pHelper->qDealWithData(MSG_NOTE_UPDATE_HIGHLIGHT_COLOR, sContent);
    } else {    //  移除高亮不可点,说明没有高亮, 点击操作 是 添加高亮
        int nSx = m_pStartPoint.x();
        int nSy = m_pStartPoint.y();

        int nEx = m_pEndPoint.x();
        int nEy = m_pEndPoint.y();

        QString sContent = QString::number(nSx) + Constant::sQStringSep +
                           QString::number(nSy) + Constant::sQStringSep +
                           QString::number(nEx) + Constant::sQStringSep +
                           QString::number(nEy) + Constant::sQStringSep +
                           QString::number(m_pLightColor);

        dApp->m_pHelper->qDealWithData(MSG_NOTE_ADD_HIGHLIGHT_COLOR, sContent);
    }
}

void TextOperationMenu::slotCopyClicked()
{
    dApp->m_pHelper->qDealWithData(MSG_OPERATION_TEXT_COPY, m_strSelectText);
}

void TextOperationMenu::slotRemoveHighLightClicked()
{
    QString sContent = QString::number(m_pClickPoint.x()) + Constant::sQStringSep +  QString::number(m_pClickPoint.y());
    QString sRes = dApp->m_pHelper->qDealWithData(MSG_NOTE_REMOVE_HIGHLIGHT_COLOR, sContent);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        int nReturn = obj.value("return").toInt();
        if (nReturn == MSG_OK) {
            QString sUuid = obj.value("value").toString();

            QJsonObject notifyObj;

            notifyObj.insert("content", sUuid);
            notifyObj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + NOTE_WIDGET);

            QJsonDocument notifyDoc(notifyObj);
            notifyMsg(MSG_NOTE_DELETE_ITEM, notifyDoc.toJson(QJsonDocument::Compact));
        }
    }
}

void TextOperationMenu::slotAddNoteClicked()
{
    if (m_strNoteUuid == "") {
        QString msgContent = QString("%1").arg(m_nClickPage) + Constant::sQStringSep + QString("%1").arg(m_pClickPoint.x()) + Constant::sQStringSep + QString("%1").arg(m_pClickPoint.y());

        QJsonObject obj;
        obj.insert("content", msgContent);
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);

        notifyMsg(MSG_OPERATION_TEXT_ADD_ANNOTATION, doc.toJson(QJsonDocument::Compact));
    } else {
        QString msgContent = m_strNoteUuid.trimmed() + Constant::sQStringSep + QString::number(m_nClickPage);

        QJsonObject obj;
        obj.insert("content", msgContent);
        obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + DOC_SHOW_SHELL_WIDGET);

        QJsonDocument doc(obj);
        notifyMsg(MSG_OPERATION_TEXT_SHOW_NOTEWIDGET, doc.toJson(QJsonDocument::Compact));
    }
}

void TextOperationMenu::slotAddBookMarkClicked()
{
    QJsonObject obj;
    obj.insert("content", QString::number(m_nClickPage));
    obj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + BOOKMARK_WIDGET);

    QJsonDocument doc(obj);

    notifyMsg(MSG_OPERATION_ADD_BOOKMARK, QString("%1").arg(m_nClickPage));
}

void TextOperationMenu::slotExitFullScreenClicked()
{
    notifyMsgToFrame(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
}

void TextOperationMenu::setPEndPoint(const QPoint &pEndPoint)
{
    m_pEndPoint = pEndPoint;
}

void TextOperationMenu::setPStartPoint(const QPoint &pStartPoint)
{
    m_pStartPoint = pStartPoint;
}
