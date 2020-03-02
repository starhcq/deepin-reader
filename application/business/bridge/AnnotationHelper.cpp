/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AnnotationHelper.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "application.h"
#include "MsgModel.h"

#include "business/AppInfo.h"
#include "business/FileDataManager.h"

#include "widgets/main/MainTabWidgetEx.h"

AnnotationHelper::AnnotationHelper(QObject *parent)
    : HelperImpl(parent)
{
    m_pMsgList = {
        MSG_NOTE_PAGE_ADD_CONTENT, MSG_NOTE_PAGE_DELETE_CONTENT, MSG_NOTE_PAGE_UPDATE_CONTENT,
        MSG_NOTE_DELETE_CONTENT, MSG_NOTE_UPDATE_CONTENT, MSG_NOTE_REMOVE_HIGHLIGHT_COLOR,
        MSG_NOTE_UPDATE_HIGHLIGHT_COLOR, MSG_NOTE_ADD_HIGHLIGHT_COLOR, MSG_NOTE_ADD_HIGHLIGHT_NOTE
    };
}

void AnnotationHelper::notifyMsg(const int &msgType, const QString &msgContent)
{
    QString sCurPath = MainTabWidgetEx::Instance()->qGetCurPath();

    QJsonObject obj;
    obj.insert("need", true);
    obj.insert("path", sCurPath);
    obj.insert("content", msgContent);

    QJsonDocument doc = QJsonDocument(obj);

    dApp->m_pModelService->notifyMsg(msgType, doc.toJson(QJsonDocument::Compact));
}

QString AnnotationHelper::qDealWithData(const int &msgType, const QString &msgContent)
{
    QString sRes = "";
    if (msgType == MSG_NOTE_PAGE_ADD_CONTENT) {                 //  新增 页面注释
        AddPageIconAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_DELETE_CONTENT) {       //  删除页面注释
        __DeletePageIconAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_PAGE_UPDATE_CONTENT) {       //  更新页面注释
        __UpdatePageIconAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_DELETE_CONTENT) {            //  刪除高亮注释
        __RemoveAnnotation(msgContent);
    } else if (msgType == MSG_NOTE_UPDATE_CONTENT) {            //  更新高亮注释
        __UpdateAnnotationText(msgContent);
    } else if (msgType == MSG_NOTE_REMOVE_HIGHLIGHT_COLOR) {  //  移除高亮注释 的高亮
        __RemoveHighLight(msgContent, sRes);
    } else if (msgType == MSG_NOTE_UPDATE_HIGHLIGHT_COLOR) {  //  更新高亮颜色
        __ChangeAnnotationColor(msgContent);
    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_COLOR) {                 //  添加高亮
        __AddHighLight(msgContent);
    } else if (msgType == MSG_NOTE_ADD_HIGHLIGHT_NOTE) {            //  添加高亮注释
        __AddHighLightAnnotation(msgContent);
    }

    int nRes = MSG_NO_OK;
    if (m_pMsgList.contains(msgType)) {
        nRes = MSG_OK;
    }

    QJsonObject obj;
    obj.insert("return", nRes);
    obj.insert("value", sRes);

    QJsonDocument doc(obj);

    return doc.toJson(QJsonDocument::Compact);
}

//  删除注释节点
void AnnotationHelper::__DeletePageIconAnnotation(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);

            _proxy->removeAnnotation(sUuid, sPage.toInt());

            notifyMsg(MSG_NOTE_PAGE_DELETE_ITEM, sUuid);
        }
    }
}


//  更新注释节点内容
void AnnotationHelper::__UpdatePageIconAnnotation(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sNote = sList.at(0);
            QString sUuid = sList.at(1);
            QString sPage = sList.at(2);
            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);

            notifyMsg(MSG_NOTE_PAGE_UPDATE_ITEM, msgContent);
        }
    }
}

void AnnotationHelper::UpdateDocFileState()
{
    QString sPath = MainTabWidgetEx::Instance()->qGetCurPath();

    QJsonObject obj;
    obj.insert("need", true);
    obj.insert("path", sPath);
    obj.insert("content", "1");

    QJsonDocument doc = QJsonDocument(obj);

    notifyMsg(MSG_FILE_IS_CHANGE, doc.toJson(QJsonDocument::Compact));
}

void AnnotationHelper::__AddHighLight(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QString nSx = "", nSy = "", nEx = "", nEy = "";
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 4) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
        } else if (sList.size() == 5) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
            int iIndex = sList.at(4).toInt();

            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
            dApp->m_pAppInfo->setSelectColor(color);
        }

        QPoint pStartPoint(nSx.toInt(), nSy.toInt());
        QPoint pEndPoint(nEx.toInt(), nEy.toInt());
        QColor color = dApp->m_pAppInfo->selectColor();

        _proxy->addAnnotation(pStartPoint, pEndPoint, color);
    }
}

void AnnotationHelper::__AddHighLightAnnotation(const QString &msgContent)
{
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 6) {
        QString nSx = sList.at(0);
        QString nSy = sList.at(1);
        QString nEx = sList.at(2);
        QString nEy = sList.at(3);
        QString sNote = sList.at(4);
        QString sPage = sList.at(5);

        DocummentProxy *_proxy = DocummentProxy::instance();
        if (_proxy) {
            QPoint pStartPoint(nSx.toInt(), nSy.toInt());
            QPoint pEndPoint(nEx.toInt(), nEy.toInt());
            QColor color = dApp->m_pAppInfo->selectColor();

            QString strUuid = _proxy->addAnnotation(pStartPoint, pEndPoint, color);
            if (strUuid != "") {
                _proxy->setAnnotationText(sPage.toInt(), strUuid, sNote);

                QString t_str = strUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
                notifyMsg(MSG_NOTE_ADD_ITEM, t_str);
            }
        }
    }
}

void AnnotationHelper::__RemoveHighLight(const QString &msgContent, QString &sRes)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 2) {
            QString sX = contentList.at(0);
            QString sY = contentList.at(1);

            QPoint tempPoint(sX.toInt(), sY.toInt());

            QString sUuid = _proxy->removeAnnotation(tempPoint);
            if (sUuid != "") {
                sRes = sUuid;
                notifyMsg(MSG_NOTE_DELETE_ITEM, sUuid);
            }
        }
    }
}

//  更新高亮颜色
void AnnotationHelper::__ChangeAnnotationColor(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 3) {
            QString sIndex = contentList.at(0);
            QString sUuid = contentList.at(1);
            QString sPage = contentList.at(2);

            int iIndex = sIndex.toInt();
            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);

            _proxy->changeAnnotationColor(sPage.toInt(), sUuid, color);     //  更新高亮顏色,  是对文档进行了操作
        }
    }
}

void AnnotationHelper::__RemoveAnnotation(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            _proxy->removeAnnotation(sUuid, sPage.toInt());

            notifyMsg(MSG_NOTE_DELETE_ITEM, sUuid);
        }
    }
}

void AnnotationHelper::__UpdateAnnotationText(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sText = sList.at(0);
            QString sUuid = sList.at(1);
            QString sPage = sList.at(2);

            _proxy->setAnnotationText(sPage.toInt(), sUuid, sText);
            notifyMsg(MSG_NOTE_UPDATE_ITEM, msgContent);
        }
    }
}

void AnnotationHelper::AddPageIconAnnotation(const QString &msgContent)
{
    DocummentProxy *_proxy = DocummentProxy::instance();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sNote = sList.at(0);
            QString sUuid = sList.at(1);
            QString sPage = sList.at(2);
            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
            QString t_str = sUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
            notifyMsg(MSG_NOTE_PAGE_ADD_ITEM, t_str);
        }
    }
}