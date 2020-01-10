/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef BUFFERWIDGET_H
#define BUFFERWIDGET_H

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <DSpinner>

#include "CustomControl/CustomWidget.h"

class BufferWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BufferWidget)
public:
    explicit BufferWidget(CustomWidget *parent = nullptr);
    ~BufferWidget() Q_DECL_OVERRIDE;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif  // BUFFERWIDGET_H
