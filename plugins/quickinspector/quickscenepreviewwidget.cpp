/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quickscenepreviewwidget.h"
#include "quickinspectorinterface.h"

#include <common/streamoperators.h>

#include <QMouseEvent>
#include <QPainter>

using namespace GammaRay;
static qint32 QuickScenePreviewWidgetStateVersion = 2;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::QuickInspectorInterface::RenderMode)
QT_END_NAMESPACE

QuickScenePreviewWidget::QuickScenePreviewWidget(QuickInspectorInterface *inspector,
                                                 QuickSceneControlWidget *control,
                                                 QWidget *parent)
    : RemoteViewWidget(parent)
    , m_inspectorInterface(inspector)
    , m_control(control)
{
    setName(QStringLiteral("com.kdab.GammaRay.QuickRemoteView"));
    setUnavailableText(tr("No remote view available.\n(This happens e.g. when the window is minimized or the scene is hidden)"));
}

QuickScenePreviewWidget::~QuickScenePreviewWidget()
{
}

void QuickScenePreviewWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return;

    QDataStream stream(state);
    qint32 version;
    QuickInspectorInterface::RenderMode mode = m_control->customRenderMode();
    bool drawDecorations = m_control->serverSideDecorationsEnabled();
    RemoteViewWidget::restoreState(stream);

    stream >> version;

    switch (version) {
    case 1: {
        stream
                >> mode
                ;
        break;
    }
    case 2: {
        stream
                >> mode
                >> drawDecorations
                ;
        break;
    }
    }

    m_control->setCustomRenderMode(mode);
    m_control->setServerSideDecorationsEnabled(drawDecorations);
}

QByteArray QuickScenePreviewWidget::saveState() const
{
    QByteArray data;

    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        RemoteViewWidget::saveState(stream);

        stream << QuickScenePreviewWidgetStateVersion;

        switch (QuickScenePreviewWidgetStateVersion) {
        case 1: {
            stream
                    << m_control->customRenderMode()
                       ;
            break;
        }
        case 2: {
            stream
                    << m_control->customRenderMode()
                    << m_control->serverSideDecorationsEnabled()
                       ;
            break;
        }
        }
    }

    return data;
}

void QuickScenePreviewWidget::resizeEvent(QResizeEvent *e)
{
    RemoteViewWidget::resizeEvent(e);
}

void QuickScenePreviewWidget::drawDecoration(QPainter *p)
{
    if (frame().data().userType() == qMetaTypeId<QuickItemGeometry>()) {
        // scaled and translated
        auto itemGeometry = frame().data().value<QuickItemGeometry>();
        itemGeometry.scaleTo(zoom());
        const QuickDecorationsRenderInfo renderInfo(m_overlaySettings, itemGeometry, frame().viewRect(), zoom());
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Decorations, *p, renderInfo);
        drawer.drawDecorations();
    } else if (frame().data().userType() == qMetaTypeId<QVector<QuickItemGeometry>>()) {
        // Scaling and translations will be done on demand
        const auto itemsGeometry = frame().data().value<QVector<QuickItemGeometry>>();
        const QuickDecorationsTracesInfo tracesInfo(m_overlaySettings, itemsGeometry, frame().viewRect(), zoom());
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Traces, *p, tracesInfo);
        drawer.drawTraces();
    }
}

QuickDecorationsSettings QuickScenePreviewWidget::overlaySettings() const
{
    return m_overlaySettings;
}
