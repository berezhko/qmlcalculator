import QtQuick 1.0
import com.nokia.meego 1.0


import cppComponents 1.0
PageStackWindow { id: appWindow

    initialPage: mainPage

 Page { id: mainPage
     property bool buttonsEnabled: true
     property int fontSize: 30
     property string nextAngleMode: calculator.textRad();
     property string angleMode : calculator.textDeg();
     property string fontFamily: "Nokia Pure Text"
     property color textColor : "black"
     property color textBkColor : "white"
     property color borderColor : "#0000A0"

     function plot(painter, plotRect)
     {
         mainFrame.plot(painter, plotRect)
     }

    MessageBox{ id: messageBox
        property bool returnToPlotPage: false
        visualParent: rectangleLog
        fontSize: mainPage.fontSize
        numberOfLines : (screen.currentOrientation == Screen.Portrait) ? 2 : 1
        onVisibleChanged: {
            console.log("onVisibleChanged: " + status)
            if(status == DialogStatus.Open)
                mainPage.buttonsEnabled = false
            else mainPage.buttonsEnabled = true
        }
        onAccepted:{
            mainPage.buttonsEnabled = true
            if(returnToPlotPage){
                returnToPlotPage = false
                calculator.switchState("Plot")
            }
        }
        onRejected: onAccepted()
    }

    Rectangle{ id: mainFrame
        anchors.fill: parent
        radius: 5
        color: "#008080"
        border.color: mainPage.borderColor

        function plot(painter, plotRect){
            console.log("plot requested")
            calculator.plot(painter, plotRect)
        }

        Calculator { id: calculator
            plotPenWidth: 2
            plotFontSize: mainPage.fontSize
            plotFontFamily: mainPage.fontFamily
            onExpressionTextChanged:{
                textExpression.text = expressionText
                console.log(expressionText)
            }
            onPlotSetupChanged:{
                plotPage.plotSetup = plotSetup
            }
            onLogTextChanged:{
                textLog.text = logText
            }

            onErrorMessage:{
                messageBox.caption = caption
                messageBox.message = msg
                messageBox.open()
                console.log("error message received:" + msg)
            }

            onAngleModeChanged:
            {
                mainPage.nextAngleMode = nextAngleMode
                mainPage.angleMode = angleMode
            }

            function switchState(state){
                console.log("State requested: " + state)
                if(state === "Plot"){
                    appWindow.pageStack.push(plotPage)
                    messageBox.visualParent = plotPage
                 }
                else {
                    if(state === "Functions")
                        mainFrame.state = "Functions"
                    else if(state ==="Standard")
                        mainFrame.state = ""
                    else if(state ==="PlotError")
                    {
                        mainFrame.state = ""
                        messageBox.returnToPlotPage = true
                    }
                    if(pageStack.currentPage == plotPage)
                    {
                      appWindow.pageStack.pop()
                      messageBox.visualParent = rectangleLog
                    }
                }
            }

            onGuiStateSuggested:{
                switchState(state)
             }

        }

        Column{ // the main layout
            anchors.margins: 10
            anchors.fill: parent
            spacing: 10

            Rectangle{ id: rectangleLog
                width: parent.width
                anchors.margins: 10
                height: parent.height / 3
                radius: 5
                border.width: 3
                border.color: mainPage.borderColor
                color : mainPage.textBkColor
                clip: true
                Image{id: fhLogo
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: (textLog.text == "") ? "fh-swf.jpg" : "fh-swf-watermark.jpg"
                    x: parent.width * 0.15
                    y: parent.height * 0.1
                    width : parent.width * 0.7
                    height : parent.height * 0.7
                }
                Text {
                    id: textLog
                    width: parent.width - parent.spacing
                    height: parent.height
                    color: mainPage.textColor
                    text: ""
                    font.bold: true
                    font.family: mainPage.fontFamily
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignBottom
                    anchors.fill: parent
                    anchors.rightMargin: 3
                    anchors.bottomMargin: 3
                    font.pixelSize: mainPage.fontSize
                }
            }

            Item{ id: expressionRow
                height : 40
                width: parent.width
                anchors.margins: 0
                Rectangle {id: rectangleAngleModeIndicator
                    width: 0
                    anchors.left: parent.left
                    anchors.top: parent.top
                    height: 40
                    radius: 5
                    border.width: 3
                    color: mainPage.textBkColor
                    border.color: mainPage.borderColor
                    clip: true

                    Text { id: textAngleModeIndicator
                        width: parent.width - parent.spacing
                        height: parent.height
                        color: mainPage.textColor
                        text: mainPage.angleMode
                        font.bold: true
                       // font.family: mainPage.fontFamily
                        font.pixelSize: mainPage.fontSize
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                    }
                }
                Rectangle {id: rectangleExpression
                    width: parent.width - rectangleAngleModeIndicator.width
                    anchors.top: parent.top
                    anchors.right: parent.right
                    height: 40
                    radius: 5
                    color: mainPage.textBkColor
                    border.width: 3
                    border.color: mainPage.borderColor
                    clip: true

                    Text { id: textExpression
                        color: mainPage.textColor
                        text: qsTr("")
                        font.bold: true
                        font.family: mainPage.fontFamily
                        font.pixelSize: mainPage.fontSize
                        horizontalAlignment: Text.AlignRight
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 3
                   }
                }
            }

            Grid{ //the buttons layout
                id: grid
                columns: (screen.currentOrientation === Screen.Portrait) ? 4 : 7
                rows: (screen.currentOrientation === Screen.Portrait) ? 5 : 3
                spacing: 8
                width: parent.width
                property int buttonWidth: (width - (columns -1) * spacing) / columns
                property int buttonHeight: (parent.height - rectangleExpression.height - rectangleLog.height - (rows + 2) * spacing) / rows

                //First row
                Button2{
                    id: button00
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:"Fn"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked:{
                        if(buttonText == "Fn")
                            mainFrame.state = "Functions"
                        else if(buttonText =="Plot")
                            mainFrame.state = "Plot"
                        else if(buttonText =="Std")
                            mainFrame.state = ""
                    }
                }

                Button2{
                    id: button01
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "C" :"1"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: {
                        if(buttonText == "Plot")
                            appWindow.pageStack.push(plotPage)
                        else calculator.addToExpressionText(buttonText)
                    }
                }

                Button2{
                    id: button02
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "AC": "2"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button03
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "/" : "3"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                //second row
                Button2{
                    id: button04
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "1": "."
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button05
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "2" : "/"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button06
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "3" : "*"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{ //second row on 7
                    id: button07
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "*" : "C"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                //third row
                Button2{
                    id: button08
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "4" :"4"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button09
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "5" : "5"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button10
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "6" :"6"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button11
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "-" :"+/-"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                //fourth row
                Button2{
                    id: button12
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "7" :"+"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button13
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "8" :"-"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{ //third row on 7
                    id: button14
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "9" :"AC"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                Button2{
                    id: button15
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "+" : "7"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

                //last row
                Button2{
                    id: button16
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "0" : "8"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }
                Button2{
                    id: button17
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "." : "9"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }
                Button2{
                    id: button18
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "+/-" :"0"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }
                Button2{
                    id: button19
                    width: parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "=" : "1/x"
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }
                Button2{//use as filler on 7*3 layout
                    id: button20
                    width: (grid.columns == 4) ? 0 : parent.buttonWidth
                    height: parent.buttonHeight
                    buttonText:(grid.columns == 4) ? "" : "="
                    fontFamily: mainPage.fontFamily
                    enabled: mainPage.buttonsEnabled
                    onClicked: calculator.addToExpressionText(buttonText)
                }

            }
        }

        states: [
            State { name: "Functions"
                PropertyChanges { target: rectangleAngleModeIndicator; width: 60; visible:true} //show it
                PropertyChanges {target: rectangleExpression; width: parent.width - rectangleAngleModeIndicator.width - 5}

                PropertyChanges {target: button00; buttonText: (grid.columns == 4) ? "Std": "Std" }
                PropertyChanges {target: button01; buttonText: (grid.columns == 4) ? "Plot" : "Plot"}
                PropertyChanges {target: button02; buttonText: (grid.columns == 4) ? "(" : "E"}
                PropertyChanges {target: button03; buttonText: (grid.columns == 4) ? ")" : "("}

                PropertyChanges {target: button04; buttonText: (grid.columns == 4) ? "1/x" : ")"}
                PropertyChanges {target: button05; buttonText: (grid.columns == 4) ? "x^y" : "1/x"}
                PropertyChanges {target: button06; buttonText: (grid.columns == 4) ? calculator.textSqrt() : calculator.textSqrt()}
                PropertyChanges {target: button07; buttonText: (grid.columns == 4) ? "E" : "sin"}//row 2 on 7

                PropertyChanges {target: button08; buttonText: (grid.columns == 4) ? "sin" : "cos"}
                PropertyChanges {target: button09; buttonText: (grid.columns == 4) ? "cos" : "tan" }
                PropertyChanges {target: button10; buttonText: (grid.columns == 4) ? "tan" : "arc"}
                PropertyChanges {target: button11; buttonText: (grid.columns == 4) ? "arc" : mainPage.nextAngleMode}

                PropertyChanges {target: button12; buttonText: (grid.columns == 4) ? "sinh" : calculator.textPi()}
                PropertyChanges {target: button13; buttonText: (grid.columns == 4) ? "cosh" : "x^y"}
                PropertyChanges {target: button14; buttonText: (grid.columns == 4) ? "tanh" : "sinh"} //row 3 on 7
                PropertyChanges {target: button15; buttonText: (grid.columns == 4) ? "ar" :  "coshh" }
                PropertyChanges {target: button16; buttonText: (grid.columns == 4) ? mainPage.nextAngleMode: "tanh" }
                PropertyChanges {target: button17; buttonText: (grid.columns == 4) ? "exp" : "ar" }
                PropertyChanges {target: button18; buttonText: (grid.columns == 4) ? "ln" :"exp"}
                PropertyChanges {target: button19; buttonText: (grid.columns == 4) ? "log" :"ln"}
                PropertyChanges {target: button20; buttonText: (grid.columns == 4) ? "" : "log"}
            }
        ]//end states
    }//end mainFrame
  }//end mainPage

 Page{id: plotPage
     property string plotSetup: qsTr("No plot available")
     function updatePlot()
     {
         plot.updatePlot()
     }

    PlotPage{id:plot
        plotSetup: plotPage.plotSetup
        color: mainFrame.color
        fontSize: mainPage.fontSize
        fontFamily: mainPage.fontFamily
        anchors.fill:parent
        onPlotRequest: {
            mainPage.plot(painter, plotRect)
        }

        onPageRequest: {
            console.log("pageRequest:" + page)
            appWindow.pageStack.pop()
            if(page === qsTr("Fn"))
                mainFrame.state = "Functions"
            else if(page ==="Plot")
                mainFrame.state = "Plot"
            else if(page ==="Std")
                mainFrame.state = ""
        }
    }//end plotPage
 }
} //end appWindow
