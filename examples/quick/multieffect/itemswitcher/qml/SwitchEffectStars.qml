// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Effects

Item {
    id: rootItem
    // We expect all effects to be placed under ItemSwitcher
    property Item switcher: rootItem.parent

    anchors.fill: parent

    Item {
        id: mask
        anchors.fill: parent
        layer.enabled: true
        visible: false
        clip: true
        Image {
            anchors.fill: parent
            source: "images/star.png"
            scale: switcher.inAnimation * 5
            rotation: switcher.outAnimation * 360
        }
    }

    // Item going out
    MultiEffect {
        source: switcher.fromItem
        anchors.fill: parent
        maskEnabled: true
        maskSource: mask
        maskInverted: true
        maskThresholdLow: 0.5
        maskSpreadLow: 0.5
    }
    // Item coming in
    MultiEffect {
        source: switcher.toItem
        anchors.fill: parent
        maskEnabled: true
        maskSource: mask
        maskThresholdLow: 0.5
        maskSpreadLow: 0.5
        colorizeColor: "#ffd020"
        colorize: Math.max(0, 1.0 - switcher.inAnimation * 2)
        brightness: Math.max(0.0, 0.8 - switcher.inAnimation * 2)
    }
}
