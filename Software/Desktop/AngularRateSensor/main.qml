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

    property alias leftRPM:     _leftTachometer.value
    property alias rightRPM:    _rightTachometer.value

    property alias leftDyno:    _leftDynamometer.value
    property alias rightDyno:   _rightDynamometer.value

    property alias diferentRPM: _diferentTachometer.value

    property alias  offSet:     _dial.value

    onOffSetChanged: controller.setWheelRadiusOffSet(offSet)



    ComDialog
    {
        id: _comDialog
        // visible:true
        onRejected: Qt.quit()


        onAccepted: {
            root.visible = true


            controller.driveTransmissionInfo(_comDialog.shaftRadius,_comDialog.wheelRadius)

            if(!controller.run(_comDialog.currentPort,_comDialog.currentBaudRate, _comDialog.folderPath))
            {
                messageDialog.visible = true
            }
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

        onReadyRead:
        {
            root.leftRPM = data.leftRPM;
            root.rightRPM = data.rightRPM;

            root.leftDyno = data.leftDyno;
            root.rightDyno = data.rightDyno;

            root.diferentRPM = data.diferentRPM

        }

    }



    Component.onCompleted:
    {
        _comDialog.visible = true

        var listPorts = controller.portInfo();
        _comDialog.listCom= listPorts
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
            gaugeStepSize: 10
            stepSize: 1

            value: diferentRPM



           /* property real rC: 0.285
            property real rV: 0.161
            property real delta: rC*(_dial.value*0.01)

            onDeltaChanged: console.log("Delta " +delta)
            onValueChanged: {
               // console.log("left " + (root.leftRPM * (rC+delta)))

                //console.log("Right " +root.rightRPM*rV)

                console.log("Right " +value)
            }

          //  property var RC:     0.278
           // value: root.rightRPM - ((_dial.value*0.01)*root.leftRPM)


          //  property real curentVlue: ((root.leftRPM * (rC+delta)) / (root.rightRPM*rV)) -1

            property real curentVlue: ((root.rightRPM*rV - root.leftRPM * (rC+delta) ) / (root.rightRPM*rV))

            onCurentVlueChanged:
            {
                value = controller.windowFilter(curentVlue)*100;
            }
*/

            //value:


            //   value:  (root.rightRPM * (rC+(rC*((_dial.value)*0.01))))/(root.leftRPM*rV) *100

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

            anchors.top: _diferentTachometer.bottom
            anchors.right: _diferentTachometer.left
            anchors.rightMargin: 75


            maximumValue: 5000
            minimumValue: -5000
            gaugeStepSize:1000

        }
        Tachometer
        {
            id: _rightDynamometer
            size: 150
            value: rightDyno

            anchors.top: _diferentTachometer.bottom
            //anchors.topMargin: 10
            anchors.left: _diferentTachometer.right
            anchors.leftMargin: 75
            gaugeStepSize:1000
            maximumValue: 10000
            minimumValue: -10000

        }

        StatusIndicator {
            id: statusIndicator
            x: 15
            y: 10
            color: "#4bff00"
            active: controller.isRun
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
            value: 0
            onValueChanged:
            {
                controller.offSetDiferentRPM = value
            }

            Text {
                id: element
                x: _dial.width/2 - element.width/2
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
            checked: controller.isRun

            onClicked:
            {
                controller.isRun = !controller.isRun
            }
            Text {
                id: labelButton
                x: _runButton.width/2 - labelButton.width/2
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
