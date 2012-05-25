import QtQuick 1.0
import com.nokia.meego 1.0
import cppComponents 1.0


Rectangle {id: mainFrame

    property int fontSize: 30
    property string fontFamily: "Helvetica"
    property color plotBkColor: "white"
    property color plotFgColor: "black"
    property int plotHeight: height / 2 -plotSetupFrame.height - mainColumn.spacing
    property int buttonGridHeight: height - plotHeight - plotSetupFrame.height - 4 * mainColumn.spacing
    property string plotSetup: "plotter setup"
    property bool buttonsEnabled: true
    property bool plotSliderVisible: false;

    signal pageRequest(string page)
    signal plotRequest(variant painter, variant plotRect)

    function showHelpText(){
        helpBox.open()
    }

    function sendPlotCommand(command){
        calculator.addToExpressionText("PlotCommand: " + command)
    }

    function sendCommand(command){
        calculator.addToExpressionText(command)
    }

    radius: 5

    MessageBox{ id: helpBox
        visualParent: mainFrame
        numberOfLines: 12
        caption: qsTr("How to Setup a Plot")
        captionBackgroundColor: "blue"
        horizontalAlignment: "AlignLeft"

        message:qsTr(
"\nUse the \"Y=\" and \"X=\" buttons to activate linear, 1/x or logarithmic scales.\n\
Set the range for x with the \"From x=\" and \"To x=\" buttons.\n\
Use the \"x\" button to add a variable to the expression to be plotted.\n\
When in \"Bars\" mode, the area between the plotted function and the X-axis\n\
(the numerically calculated definite integral) is indicated too.\n\
Tap on the plot to toggle plot sizes.\n\
Tap on the white rectangle to show or hide a marker.")
    }

    anchors.fill: parent

    Column{ id: mainColumn // the main layout
        anchors.margins: 10
        anchors.fill: parent
        spacing: 10

        PlotDeclarativeItem {id: plot
            x: 0
            y: 0
            width: parent.width
            height: mainFrame.plotHeight
            Behavior on height { PropertyAnimation { duration: 500 } }
            onPlotRequest:{
                mainFrame.plotRequest(painter, plotRect);
            }

            MouseArea{id:plotMouseArea
                anchors.fill: parent
                onClicked: {
                    if(mainFrame.state == "")
                        mainFrame.state = "PlotSquare"
                    else if(mainFrame.state == "PlotSquare")
                        mainFrame.state = "PlotFullScreen"
                    else
                        mainFrame.state = ""
                }
            }
        }

        Slider{id:plotSlider
            width: parent.width
            maximumValue: 99
            minimumValue: 0
            value: 0
            valueIndicatorVisible: true
            stepSize: 1.0
            visible: plotSliderVisible
            onValueChanged: {
                sendPlotCommand("x-Index " + value);
            }
        }

        Rectangle{id:plotSetupFrame
            x: 0
            y: 0
            width: parent.width
            height: 50
            radius: 5
            color: mainFrame.plotBkColor
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    plotSliderVisible = ! plotSliderVisible
                    if(plotSliderVisible)
                        sendPlotCommand("x-Index " + plotSlider.value) //show indicator
                    else sendPlotCommand("x-Index -1") //hide indicator
                }
            }

            Text {
                id: plotSetupText
                text: mainFrame.plotSetup
                font.pixelSize: 20
            }
        }
        Grid{ id: buttonGrid
            columns: (screen.currentOrientation == Screen.Portrait) ? 3 : 5
            rows: (screen.currentOrientation == Screen.Portrait) ? 5 : 3
            spacing: 8
            width: parent.width
            property int buttonWidth: (width - (columns -1) * spacing) / columns
            property int buttonHeight: (buttonGridHeight - rows * spacing) / rows

            function activateInput(text)
            {
                buttonDescription.buttonText= text
                inputGrid.visible = true
                buttonGrid.visible = false
            }

            Button2{id: button00
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:"Std"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    pageRequest(buttonText)
                }
            }
            Button2{id: button01
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:"Fn"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    pageRequest(buttonText)
                }
            }
            Button2{id: button02
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:"?"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    mainFrame.showHelpText()
                }
            }
            Button2{id: button03
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "Y=f(x)" : "From x="
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    if(buttonGrid.rows  == 5)
                        sendPlotCommand(buttonText)
                    else buttonGrid.activateInput(buttonText)
                }
            }


            Button2{id: button04
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "Y=ln(f(x))" : "Line"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
            Button2{id: button05
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "Y=1/f(x)" :"Y=f(x)"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
            Button2{id: button06
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "X=x":"Y=ln(f(x)"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }


            Button2{id: button07
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "X=ln(x)":"Y=1/f(x)"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
            Button2{id: button08
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "X=1/x":"To x="
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    if(buttonGrid.rows  == 5)
                        sendPlotCommand(buttonText)
                    else buttonGrid.activateInput(buttonText)
                }
            }
            Button2{id: button09
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "From x=":"Dots"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    if(buttonGrid.rows  == 5)
                        buttonGrid.activateInput(buttonText)
                    else sendPlotCommand(buttonText)
                    buttonGrid.activateInput(buttonText)
                }
            }
            Button2{id: button10
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "To x=":"X=x"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    if(buttonGrid.rows  == 5)
                        buttonGrid.activateInput(buttonText)
                    else sendPlotCommand(buttonText)
                }
            }
            Button2{id: button011
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "x" :"X=ln(x)"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                   sendCommand(buttonText)
                }
            }
            Button2{id: button12
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "Line" :"X=1/x"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
            Button2{id: button13
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:(buttonGrid.rows  == 5) ? "Dots":"x"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
            Button2{id: button14
                width: parent.buttonWidth
                height: parent.buttonHeight
                buttonText:"Bars"
                fontFamily: mainFrame.fontFamily
                enabled: mainFrame.buttonsEnabled
                onClicked: {
                    sendPlotCommand(buttonText)
                }
            }
        }//end Grid

        Grid{id: inputGrid
            spacing: buttonGrid.spacing
            visible: false
            columns: (screen.currentOrientation == Screen.Portrait) ? 1 : 2
            property int rowWidth: (screen.currentOrientation == Screen.Portrait) ? buttonGrid.width : (buttonGrid.width / 2) - spacing
            property int buttonWidth : (rowWidth / 3) - (2 * spacing)
            Grid{ id: inputRow
                spacing: buttonGrid.spacing
                columns: (screen.currentOrientation == Screen.Portrait) ? 4 : 2

                Button2{id: buttonDescription
                    width: inputGrid.buttonWidth
                    height: buttonGrid.buttonHeight
                    buttonText:"OK"
                    fontFamily: mainFrame.fontFamily
                    enabled: mainFrame.buttonsEnabled

                    function inputOk(){
                        sendPlotCommand(buttonText + " " + textInput.text)
                        inputGrid.visible = false
                        buttonGrid.visible = true
                    }

                    onClicked: inputOk()
                }
                Rectangle{ id: textInputRect
                    color:"white"
                    width: inputGrid.buttonWidth
                    height: buttonGrid.buttonHeight
                    radius: mainFrame.radius
                    border.width: 2
                    onVisibleChanged: {
                        if(visible)
                            textInput.text= ""
                    }

                    TextInput{
                        id: textInput
                        width: parent.width
                        clip: true
                        activeFocusOnPress :false //no keyboard! we need a numpad
                        text: ""
                        font.pixelSize: buttonGrid.buttonHeight/2
                        y: parent.height / 2 - font.pixelSize /2
                    }
                }

                Button2{id: buttonC
                    width: (screen.currentOrientation == Screen.Portrait) ? 0.6 * inputGrid.buttonWidth : inputGrid.buttonWidth
                    height: buttonGrid.buttonHeight
                    buttonText:"C"
                    fontFamily: mainFrame.fontFamily
                    enabled: mainFrame.buttonsEnabled
                    onClicked: {
                        var text = textInput.text
                        textInput.text = ""
                        for(var i = 0; i < text.length -1; i++)
                            textInput.text += text[i]
                    }
                }
                Button2{id: buttonOk
                    width: buttonC.width
                    height: buttonGrid.buttonHeight
                    buttonText:"OK"
                    fontFamily: mainFrame.fontFamily
                    enabled: mainFrame.buttonsEnabled
                    onClicked: {
                        buttonDescription.inputOk()
                    }
                }
 /*               Item{id:spacer
                  height:  2 * inputGrid.spacing
                  width: height
                }*/
            }//end Row

            Numberpad{id: numPad
                height: buttonGrid.buttonHeight * rows + buttonGrid.spacing * (rows -1)
                width: (screen.currentOrientation == Screen.Portrait) ? buttonDescription.width * 3 + 2 * inputGrid.spacing
                                                                      :( mainFrame.width - inputGrid.spacing) * 0.5
                spacing: buttonGrid.spacing
                onButtonPressed: {
                    textInput.text = textInput.text + buttonText
                }
            }
        }//end inputGrid
    }//end Column

    states:[
        State{name: "PlotFullScreen"
            PropertyChanges{target: plot; height:  mainColumn.height - plotSetupFrame.height- mainColumn.spacing}
        },
        State{name: "PlotSquare"
            PropertyChanges{target: plot; height:  (screen.currentOrientation == Screen.Portrait)
                                          ? mainFrame.width
                                          : mainColumn.height - plotSetupFrame.height- mainColumn.spacing}
        }
     ]

}
