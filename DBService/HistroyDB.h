﻿/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#ifndef CONTROLLER_HISTROYDBEX_H
#define CONTROLLER_HISTROYDBEX_H

// FilesTable
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FilePath           | FileScale | FileDoubPage | FileFit  | FileRotate | FileShowLeft | ListIndex | CurPage //
//TEXT primari key   | TEXT      | TEXT         | TEXT     | TEXT       | TEXT         | TEXT      | TEXT    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DBFactory.h"
#include <QJsonObject>

class HistroyDB : public DBFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(HistroyDB)

public:
    explicit HistroyDB(QObject *parent = nullptr);

    // DBFactory interface
public:
    void saveData(const QString &newPath) Q_DECL_OVERRIDE;
    void qSelectData() Q_DECL_OVERRIDE;

    QJsonObject getHistroyData() const;
    void setHistroyData(const QString &, const int &);

private:
    void checkDatabase() Q_DECL_OVERRIDE;
    void clearInvalidRecord() Q_DECL_OVERRIDE;

    void insertData(const QString &);
    void updateData(const QString &);
    void deleteData();

    int GetKeyValue(const QString &);

private:
    QString m_strTableName = "FilesTable";
    QJsonObject m_pDataObj;
    QJsonObject m_pNewDataObj;
};

#endif // CONTROLLER_HISTROYDBEX_H