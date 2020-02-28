import QtQuick 2.0
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4


CircularGauge
{
    id: root
    property int size: 250
    property double gaugeStepSize: 50
    width: size
    height: size

    stepSize: 1
    maximumValue: 500

    style: CircularGaugeStyle {

        labelStepSize: root.gaugeStepSize


        background:  Rectangle
        {
            anchors.fill: parent
            radius: root.size/2

            color: "black"
            Text {
                id: speedText
                font.pixelSize:root.size/8
                z:2
                text: root.value
                color: "white"
                horizontalAlignment: Text.AlignRight
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom

                //anchors.bottom:  root.bottom
                //anchors.topMargin: 200


            }
        }
    }
}
