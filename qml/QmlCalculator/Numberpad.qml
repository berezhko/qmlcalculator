import QtQuick 1.0
import com.nokia.meego 1.0

Item { id:mainFrame
    property string fontFamily:"Helvetica"
    property bool buttonsEnabled : true
    property int buttonHeight: buttonGrid.buttonHeight
    property int spacing: 10
    property int rows:(screen.currentOrientation == Screen.Portrait) ? 4 : 3

    width: 100
    height: 62

    signal buttonPressed(string buttonText)

    Grid{id: buttonGrid
        columns: (screen.currentOrientation == Screen.Portrait) ? 3 : 4
        rows: (screen.currentOrientation == Screen.Portrait) ? 4 : 3
        spacing: mainFrame.spacing
        width: parent.width
        property int buttonWidth: (width - (columns -1) * spacing) / columns
        property int buttonHeight: (mainFrame.height - (rows - 1) * spacing) / rows

        //First row
        Button2{
            id: button00
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:"1"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }

        Button2{
            id: button01
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:"2"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }

        Button2{
            id: button02
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:"3"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        //Second row
        Button2{
            id: button03
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3) ? "4" : "-"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button04
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "5" :"4"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button05
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "6" : "5"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        //third row
        Button2{
            id: button06
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "7" : "6"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button07
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "8" : "."
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button08
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "9" : "7"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        //fourth row
        Button2{
            id: button09
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "-" : "8"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button10
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "0" :"9"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
        Button2{
            id: button11
            width: parent.buttonWidth
            height: parent.buttonHeight
            buttonText:(buttonGrid.columns == 3)? "." : "0"
            fontFamily: mainFrame.fontFamily
            enabled: mainFrame.buttonsEnabled
            onClicked:{
                buttonPressed(buttonText)
             }
        }
    }//end Grid
}
