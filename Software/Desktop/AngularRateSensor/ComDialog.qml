import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import QtQml.Models 2.13

Dialog {
    id: dateDialog
    visible: true
    title: "Choose a date"
    standardButtons: StandardButton.Ok | StandardButton.Close


    property string currentPort: _port.currentText
    property int currentBaudRate: _baudRate.currentText
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
                    currentIndex: 11

                }
            }
        }

        Row
        {

            TextField {
                id: textField

                placeholderText: qsTr("file name")
            }

            Button {
                id: _openFileDialog

                text: qsTr("file")
                onClicked: _FileDialog.visible = true;
            }
        }
    }

    FileDialog {
        id: _FileDialog
        title: "Please choose a file"
        folder: shortcuts.home

        onAccepted: {
                textField.focus = true   ;
            textField.text = _FileDialog.fileUrl
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
