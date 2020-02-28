import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import QtQml.Models 2.13
import Qt.labs.settings 1.0

Dialog {
    id: dateDialog
    visible: true
    title: "Choose a date"
    standardButtons: StandardButton.Ok | StandardButton.Close

    Settings
    {
        id: setings
        property int setingsBaudRate : 11
        property string setingsFolderPath: shortcuts.home
        property real setingsWheelRadius : 0.285
        property real setingsShaftRadius : 0.161
    }
    property string currentPort: _port.currentText
    property int currentBaudRate: _baudRate.currentText
    property string folderPath: _FileDialog.folder

    property real wheelRadius: setings.setingsWheelRadius
    property real shaftRadius: setings.setingsShaftRadius

    property var listCom: "None"

    Column{

        Row {

            spacing: 5
            Column{
                Text {
                    id: _portLabel
                    text: qsTr("Set port")
                }

                ComboBox {
                    id: _port
                    model: listCom
                }
            }

            Column{
                Text {
                    id: _baudRateLabel
                    text: qsTr("Set baud rate")
                }

                ComboBox {
                    id: _baudRate
                    model: [110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000]
                    currentIndex: setings.setingsBaudRate
                    onCurrentIndexChanged: setings.setingsBaudRate = currentIndex

                }
            }
        }

        Row
        {

            TextField {
                id: textField

                //placeholderText: qsTr("file name")
                text: setings.setingsFolderPath
            }

            Button {
                id: _openFileDialog

                text: qsTr("file")
                onClicked: _FileDialog.visible = true;
            }
        }

        Row
        {

            spacing: 5
            Column{
                Text {
                    id: _radiusWheel
                    text: qsTr("Wheel Radius")

                }

                TextField {
                    id: textFieldd
                    text: setings.setingsWheelRadius
                    onTextChanged: setings.setingsWheelRadius = text
                }

            }

            Column{
                Text {
                    id: _radiusShaft
                    text: qsTr("Radius of the shaft")
                }

                TextField {
                    id: textFieldd1
                    text: setings.setingsShaftRadius
                    onTextChanged: setings.setingsShaftRadius = text
                }

            }

        }
    }

    FileDialog {
        id: _FileDialog
        title: "Please choose a file"
        folder: setings.setingsFolderPath
        selectFolder: true
        onFolderChanged: setings.setingsFolderPath = folder
        onAccepted: {
            textField.focus = true;
            textField.text = _FileDialog.folder
        }
    }


/*
    Button {
        id: button
        x: 211
        y: -457
        text: qsTr("Button")
    }*/


}
