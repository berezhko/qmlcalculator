import QtQuick 1.0
import com.nokia.meego 1.0


Dialog{ id: messageBox

    property string caption : "Caption Text"
    property string message : "Message text"
    property string horizontalAlignment:"AlignHCenter"
    property int fontSize : 22
    property int numberOfLines: 1
    property color captionBackgroundColor: "red"

    title: Rectangle {
        id: titleRectangle
        height: fontSize * 1.5
        width: visualParent.width
        anchors.horizontalCenter: parent.horizontalCenter
        color: captionBackgroundColor
        Text {
            id: captionText
            anchors.fill: parent
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            font.pixelSize: messageBox.fontSize
            color: "white"
            text: caption
        }
    }

    content:Rectangle {
        id: messageRectangle
        height: fontSize * 1.5 * numberOfLines
        width: visualParent.width
        anchors.top: title.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        color:"black"
        Text {
            id: text
            font.pixelSize: messageBox.fontSize
            anchors.fill: parent
            color: "white"
            horizontalAlignment: messageBox.horizontalAlignment
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: message
        }
    }

    buttons: ButtonRow {
        id: buttonRow
        style: ButtonStyle { }
        anchors.top: content.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        Button {text: "OK"; onClicked: messageBox.accept()}
    }
}

