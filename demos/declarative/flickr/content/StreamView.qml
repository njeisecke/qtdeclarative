/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt.labs.particles 2.0

Item{
    id: container
    property alias model: mp.model
    property alias delegate: mp.delegate
    property bool jumpStarted: false
    ParticleSystem{ 
        id: sys 
        anchors.fill:parent
        overwrite: false
    }
    DataParticle{
        id: mp
        fade: false
        system: sys
        anchors.fill: parent
        onModelCountChanged: {
            if(!jumpStarted && modelCount > 0){
                console.log("Jumping");
                jumpStarted = true;
                sys.fastForward(8000);
            }
        }
    }
    property real emitterSpacing: parent.width/3
    TrailEmitter{
        system: sys
        width: emitterSpacing - 64
        x: emitterSpacing*0 + 32
        y: -128
        height: 32
        speed: PointVector{ y: (container.height + 128)/12 }
        particlesPerSecond: 0.4
        particleDuration: 1000000//eventually -1 should mean a million seconds for neatness
        maxParticles: 15
    }
    TrailEmitter{
        system: sys
        width: emitterSpacing - 64
        x: emitterSpacing*1 + 32
        y: -128
        height: 32
        speed: PointVector{ y: (container.height + 128)/12 }
        particlesPerSecond: 0.4
        particleDuration: 1000000//eventually -1 should mean a million seconds for neatness
        maxParticles: 15
    }
    TrailEmitter{
        system: sys
        width: emitterSpacing - 64
        x: emitterSpacing*2 + 32
        y: -128
        height: 32
        speed: PointVector{ y: (container.height + 128)/12 }
        particlesPerSecond: 0.4
        particleDuration: 1000000//eventually -1 should mean a million seconds for neatness
        maxParticles: 15
    }
    Kill{
        system: sys
        y: container.height + 64
        width: container.width
        height: 6400
    }
}
