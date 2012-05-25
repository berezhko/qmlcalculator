import QtQuick 1.0
import QtMobility.feedback 1.1

//A button that sends a clicked signal.
//Shows a white frame when pressed.
//Use the buttonText property for text on the button.

Rectangle {id: frame
    property string buttonText: "buttonText"
    property string fontFamily: "Nokia Pure Text"
    property double relativeFontHeight : 0.5;
    property color buttonColorTop: "#0d0dff"
    property color buttonColorBottom: "#020202"
    property bool enabled: true
    signal clicked()

    width: 170
    height: 50
    border.width: 2
    border.color: "black"
    radius: 10
    gradient: Gradient {
        id: theGradient
        GradientStop {
            position: 0
            color: state == "" ? buttonColorTop : "#000000"
        }

         GradientStop {
            position: 1
            color: buttonColorBottom
        }
    }

    Text{
       id: buttonText
       text: parent.buttonText
       font.pixelSize: relativeFontHeight * frame.height
       font.bold: true
       font.family: parent.fontFamily
       color: "white"
       verticalAlignment: Text.AlignVCenter
       horizontalAlignment: Text.AlignHCenter
       anchors.fill: parent
    }
    HapticsEffect {
        id: rumbleEffect
        intensity: 1.0
        duration: 20
    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onPressed: {           
            if(frame.enabled)
                frame.state = "Pressed"
            rumbleEffect.start()
        }
        onReleased: {
            frame.state = "" //base state
        }

        onClicked:{
            if(frame.enabled)
            {
              console.log("Button clicked: " + buttonText.text)
              frame.clicked()
            }
        }
    }
    states: [
        State {
            name: ""
            PropertyChanges {
                target: frame
                border.width : 1
                border.color: "black"
             }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: frame
                border.width : 6
                border.color: "white"
             }
        }
    ]
    transitions:[
        Transition {
            from:""; to: "Pressed"
            NumberAnimation { duration: 50; properties: "border.width"; easing.type: Easing.InOutQuad }
    },
        Transition {
            from:"Pressed"; to: ""
            NumberAnimation { duration: 200; properties: "border.width"; easing.type: Easing.InOutQuad }
    }
    ]

}
