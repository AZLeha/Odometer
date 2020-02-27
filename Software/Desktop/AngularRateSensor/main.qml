import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

import QtQuick.Dialogs 1.3

import custom.controller 1.0



Window {
    id: root


    visible: false
    width: 680
    height: 400
    minimumWidth: 680
    minimumHeight: 400
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth
    title: qsTr("Angular rate sensor")

    property int leftRPM:       controller.leftRPM
    property int rightRPM:      controller.rightRPM
    property int diferentRPM:   controller.diferentRPM
    property real leftDyno:     controller.leftDyno / 100
    property real rightDyno:    controller.rightDyno / 100

    property int defaultOffSet: 0
    property bool defaultRun:   false
    property alias  offSet:     _dial.value
    property alias  isRun:      _runButton.checked


    //onOffSetChanged: {diferentRPM = offSet }


    onIsRunChanged: {
        controller.isRun = isRun
      // root.visible = false;
      // _comDialog.visible = true
    }
    ComDialog
    {
        id: _comDialog
        // visible:true
        onRejected: Qt.quit()


        onAccepted: {
            root.visible = true
            if(!controller.connectToPort(_comDialog.currentPort,_comDialog.currentBaudRate, _comDialog.folderPath))
            {
                messageDialog.visible = true
            }
            //! \todo дописать поключение порта
        }
    }

    MessageDialog {
        id: messageDialog
        title: "Ошибка поключения"
        text: "Проверити подключения порта"
        icon: StandardIcon.Critical
        onAccepted: {

          Qt.quit()
        }
    }

    Controller
    {
        id: controller
        onIsRunChanged:
        {
            root.isRun = controller.isRun
        }
    }

    Component.onCompleted:
    {
        _comDialog.visible = true

        var listPorts = controller.getListPorts();
        _comDialog.listCom= listPorts
        console.log(listPorts)

    }




   Rectangle{
        anchors.fill: parent; z:0
        color: "gray"
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0


        //-----------------------------------------------------------------------------
        //******************************Control elements*******************************
        //-----------------------------------------------------------------------------
        Tachometer
        {
           id: _leftTachometer
           size: 150
           value: leftRPM
           anchors.bottom: _diferentTachometer.bottom
           anchors.right: _diferentTachometer.left
           anchors.rightMargin: 10
           maximumValue: 1500
           gaugeStepSize:100
        }

        Tachometer
        {
            id: _diferentTachometer
            size: 200
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            maximumValue: 100
            minimumValue: -100
            gaugeStepSize:10
            value: root.rightRPM - ((_dial.value*0.01)*root.leftRPM)

        }


        Tachometer
        {
            id: _rightTachometer
            size: 150
            value: rightRPM

            anchors.bottom: _diferentTachometer.bottom
            anchors.left: _diferentTachometer.right
            anchors.leftMargin: 10
            maximumValue: 1500
            gaugeStepSize:100

        }


        Tachometer
        {
            id: _leftDynamometer
            size: 150
            value: leftDyno
            stepSize: 0.01
            anchors.top: _diferentTachometer.bottom
            anchors.right: _diferentTachometer.left
            anchors.rightMargin: 75

        }
        Tachometer
        {
            id: _rightDynamometer
            size: 150
            value: rightDyno
            stepSize: 0.01
            anchors.top: _diferentTachometer.bottom
            //anchors.topMargin: 10
            anchors.left: _diferentTachometer.right
            anchors.leftMargin: 75

        }

        StatusIndicator {
            id: statusIndicator
            x: 15
            y: 10
            color: "#4bff00"
            active: root.isRun
        }
        //=============================================================================




        //-----------------------------------------------------------------------------
        //******************************Control elements*******************************
        //-----------------------------------------------------------------------------

        Dial {
            id: _dial
            x: 214
            y: 229
            stepSize: 1
            maximumValue: 100
            minimumValue: -100
            value: defaultOffSet

            Text {
                id: element
                x: 38
                y: 49
                text: qsTr("Off set")
                font.pixelSize: 12
            }

            Text {
                id: _offSet
                anchors.horizontalCenter: _dial.horizontalCenter
                anchors.top: element.bottom
                anchors.topMargin: 4
                color: "#ffffff"
                text: _dial.value
                font.pixelSize: 12
            }
        }

        ToggleButton {
            id: _runButton
            x: 363
            y: 229
            text: qsTr("")
            checked: root.defaultRun

            Text {
                id: element1
                x: 46
                y: 49
                text: qsTr("Run")
                font.pixelSize: 12
            }
        }
        //=============================================================================








        //-----------------------------------------------------------------------------
        //****************************************Labels*******************************
        //-----------------------------------------------------------------------------
        Text {
            id: element2
            x: 309
            y: 216
            text: qsTr("Off set RPM")
            font.pixelSize: 12
        }

        Text {
            id: element4
            x: 498
            y: 39
            text: qsTr("Right RPM")
            font.pixelSize: 12
        }

        Text {
            id: element3
            x: 131
            y: 39
            text: qsTr("Left RPM")
            font.pixelSize: 12
        }

        Text {
            id: element5
            x: 65
            y: 366
            text: qsTr("Left dyno")
            font.pixelSize: 12
        }

        Text {
            id: element6
            x: 561
            y: 366
            text: qsTr("Right dyno")
            font.pixelSize: 12
        }
        //=============================================================================
   }
}
