/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKSHAPE_P_H
#define QQUICKSHAPE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qquickitem.h"

#include <private/qtquickglobal_p.h>
#include <private/qquickpath_p_p.h>
#include <private/qquickrectangle_p.h>

QT_BEGIN_NAMESPACE

class QQuickShapePathPrivate;
class QQuickShapePrivate;

class QQuickShapeGradient : public QQuickGradient
{
    Q_OBJECT
    Q_PROPERTY(SpreadMode spread READ spread WRITE setSpread NOTIFY spreadChanged)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    enum SpreadMode {
        PadSpread,
        RepeatSpread,
        ReflectSpread
    };
    Q_ENUM(SpreadMode)

    QQuickShapeGradient(QObject *parent = nullptr);

    SpreadMode spread() const;
    void setSpread(SpreadMode mode);

signals:
    void spreadChanged();

private:
    SpreadMode m_spread;
};

class QQuickShapeLinearGradient : public QQuickShapeGradient
{
    Q_OBJECT
    Q_PROPERTY(qreal x1 READ x1 WRITE setX1 NOTIFY x1Changed)
    Q_PROPERTY(qreal y1 READ y1 WRITE setY1 NOTIFY y1Changed)
    Q_PROPERTY(qreal x2 READ x2 WRITE setX2 NOTIFY x2Changed)
    Q_PROPERTY(qreal y2 READ y2 WRITE setY2 NOTIFY y2Changed)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    QQuickShapeLinearGradient(QObject *parent = nullptr);

    qreal x1() const;
    void setX1(qreal v);
    qreal y1() const;
    void setY1(qreal v);
    qreal x2() const;
    void setX2(qreal v);
    qreal y2() const;
    void setY2(qreal v);

signals:
    void x1Changed();
    void y1Changed();
    void x2Changed();
    void y2Changed();

private:
    QPointF m_start;
    QPointF m_end;
};

class QQuickShapePath : public QQuickPath
{
    Q_OBJECT

    Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor NOTIFY strokeColorChanged)
    Q_PROPERTY(qreal strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY strokeWidthChanged)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
    Q_PROPERTY(FillRule fillRule READ fillRule WRITE setFillRule NOTIFY fillRuleChanged)
    Q_PROPERTY(JoinStyle joinStyle READ joinStyle WRITE setJoinStyle NOTIFY joinStyleChanged)
    Q_PROPERTY(int miterLimit READ miterLimit WRITE setMiterLimit NOTIFY miterLimitChanged)
    Q_PROPERTY(CapStyle capStyle READ capStyle WRITE setCapStyle NOTIFY capStyleChanged)
    Q_PROPERTY(StrokeStyle strokeStyle READ strokeStyle WRITE setStrokeStyle NOTIFY strokeStyleChanged)
    Q_PROPERTY(qreal dashOffset READ dashOffset WRITE setDashOffset NOTIFY dashOffsetChanged)
    Q_PROPERTY(QVector<qreal> dashPattern READ dashPattern WRITE setDashPattern NOTIFY dashPatternChanged)
    Q_PROPERTY(QQuickShapeGradient *fillGradient READ fillGradient WRITE setFillGradient RESET resetFillGradient)

public:
    enum FillRule {
        OddEvenFill = Qt::OddEvenFill,
        WindingFill = Qt::WindingFill
    };
    Q_ENUM(FillRule)

    enum JoinStyle {
        MiterJoin = Qt::MiterJoin,
        BevelJoin = Qt::BevelJoin,
        RoundJoin = Qt::RoundJoin
    };
    Q_ENUM(JoinStyle)

    enum CapStyle {
        FlatCap = Qt::FlatCap,
        SquareCap = Qt::SquareCap,
        RoundCap = Qt::RoundCap
    };
    Q_ENUM(CapStyle)

    enum StrokeStyle {
        SolidLine = Qt::SolidLine,
        DashLine = Qt::DashLine
    };
    Q_ENUM(StrokeStyle)

    QQuickShapePath(QObject *parent = nullptr);
    ~QQuickShapePath();

    QColor strokeColor() const;
    void setStrokeColor(const QColor &color);

    qreal strokeWidth() const;
    void setStrokeWidth(qreal w);

    QColor fillColor() const;
    void setFillColor(const QColor &color);

    FillRule fillRule() const;
    void setFillRule(FillRule fillRule);

    JoinStyle joinStyle() const;
    void setJoinStyle(JoinStyle style);

    int miterLimit() const;
    void setMiterLimit(int limit);

    CapStyle capStyle() const;
    void setCapStyle(CapStyle style);

    StrokeStyle strokeStyle() const;
    void setStrokeStyle(StrokeStyle style);

    qreal dashOffset() const;
    void setDashOffset(qreal offset);

    QVector<qreal> dashPattern() const;
    void setDashPattern(const QVector<qreal> &array);

    QQuickShapeGradient *fillGradient() const;
    void setFillGradient(QQuickShapeGradient *gradient);
    void resetFillGradient();

Q_SIGNALS:
    void shapePathChanged();
    void strokeColorChanged();
    void strokeWidthChanged();
    void fillColorChanged();
    void fillRuleChanged();
    void joinStyleChanged();
    void miterLimitChanged();
    void capStyleChanged();
    void strokeStyleChanged();
    void dashOffsetChanged();
    void dashPatternChanged();
    void fillGradientChanged();

private:
    Q_DISABLE_COPY(QQuickShapePath)
    Q_DECLARE_PRIVATE(QQuickShapePath)
    Q_PRIVATE_SLOT(d_func(), void _q_fillGradientChanged())
};

class QQuickShape : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(RendererType renderer READ rendererType NOTIFY rendererChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool vendorExtensionsEnabled READ vendorExtensionsEnabled WRITE setVendorExtensionsEnabled NOTIFY vendorExtensionsEnabledChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum RendererType {
        UnknownRenderer,
        GeometryRenderer,
        NvprRenderer,
        SoftwareRenderer
    };
    Q_ENUM(RendererType)

    enum Status {
        Null,
        Ready,
        Processing
    };
    Q_ENUM(Status)

    QQuickShape(QQuickItem *parent = nullptr);
    ~QQuickShape();

    RendererType rendererType() const;

    bool asynchronous() const;
    void setAsynchronous(bool async);

    bool vendorExtensionsEnabled() const;
    void setVendorExtensionsEnabled(bool enable);

    Status status() const;

    QQmlListProperty<QObject> data();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;
    void updatePolish() override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void componentComplete() override;
    void classBegin() override;

Q_SIGNALS:
    void rendererChanged();
    void asynchronousChanged();
    void vendorExtensionsEnabledChanged();
    void statusChanged();

private:
    Q_DISABLE_COPY(QQuickShape)
    Q_DECLARE_PRIVATE(QQuickShape)
    Q_PRIVATE_SLOT(d_func(), void _q_shapePathChanged())
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickShape)

#endif // QQUICKSHAPE_P_H
