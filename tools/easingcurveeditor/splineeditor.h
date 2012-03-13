/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SPLINEEDITOR_H
#define SPLINEEDITOR_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QScrollArea>

#include <QEasingCurve>
#include <QHash>

class SegmentProperties;

class SplineEditor : public QWidget
{
    Q_OBJECT

     Q_PROPERTY(QEasingCurve easingCurve READ easingCurve WRITE setEasingCurve NOTIFY easingCurveChanged);

public:
    explicit SplineEditor(QWidget *parent = 0);
    QString generateCode();
    QStringList presetNames() const;
    QWidget *pointListWidget();

    void setControlPoint(int index, const QPointF &point)
    {
        m_controlPoints[index] = point;
        update();
    }

    void setSmooth(int index, bool smooth)
    {
        m_smoothAction->setChecked(smooth);
        smoothPoint(index * 3 + 2);
        //update();
    }

signals:
    void easingCurveChanged();
    void easingCurveCodeChanged(const QString &code);


public slots:
    void setEasingCurve(const QEasingCurve &easingCurve);
    void setPreset(const QString &name);
    void setEasingCurve(const QString &code);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);

    void invalidate();
    void invalidateSmoothList();
    void invalidateSegmentProperties();

    QEasingCurve easingCurve() const
    { return m_easingCurve; }

    QHash<QString, QEasingCurve> presets() const;

private:
    int findControlPoint(const QPoint &point);
    bool isSmooth(int i) const;

    void smoothPoint( int index);
    void cornerPoint( int index);
    void deletePoint(int index);
    void addPoint(const QPointF point);

    void initPresets();

    void setupPointListWidget();

    bool isControlPointSmooth(int i) const;

    QEasingCurve m_easingCurve;
    QVector<QPointF> m_controlPoints;
    QVector<bool> m_smoothList;
    int m_numberOfSegments;
    int m_activeControlPoint;
    bool m_mouseDrag;
    QPoint m_mousePress;
    QHash<QString, QEasingCurve> m_presets;

    QMenu *m_pointContextMenu;
    QMenu *m_curveContextMenu;
    QAction *m_deleteAction;
    QAction *m_smoothAction;
    QAction *m_cornerAction;
    QAction *m_addPoint;

    QScrollArea *m_pointListWidget;

    QList<SegmentProperties*> m_segmentProperties;
    bool m_block;
};

#endif // SPLINEEDITOR_H
