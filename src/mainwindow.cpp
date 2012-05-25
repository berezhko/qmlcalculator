#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&calculator, SIGNAL(expressionTextChanged(QString)),
            this, SLOT(slot_Calculator_expressionTextChanged(QString)));
    connect(&calculator, SIGNAL(plotSetupChanged(QString)),
            this, SLOT(slot_Calculator_plotSetupChanged(QString)));
    connect(&calculator, SIGNAL(logTextChanged(QString)),
            this, SLOT(slot_Calculator_logTextChanged(QString)));
    connect(&calculator, SIGNAL(errorMessage(QString, QString)),
            this, SLOT(slot_Calculator_errorMessage(QString, QString)));
    connect(&calculator, SIGNAL(guiStateSuggested(QString)),
            this, SLOT(slot_Caculator_guiStateSuggested(QString)));
    connect(ui->plotWidget, SIGNAL(plotRequest(QPainter*,QRect)),
            &calculator, SLOT(plot(QPainter*,QRect)));

    ui->lineEditPlotSetup->setText(calculator.plotSetup());

    ui->stackedWidget->setCurrentIndex(0);
    ui->framePlotInputColumn->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton0_clicked()
{
    calculator.addToExpressionText("0");
}
void MainWindow::on_pushButton1_clicked()
{
    calculator.addToExpressionText("1");
}
void MainWindow::on_pushButton2_clicked()
{
    calculator.addToExpressionText("2");
}
void MainWindow::on_pushButton3_clicked()
{
    calculator.addToExpressionText("3");
}
void MainWindow::on_pushButton4_clicked()
{
    calculator.addToExpressionText("4");
}
void MainWindow::on_pushButton5_clicked()
{
    calculator.addToExpressionText("5");
}
void MainWindow::on_pushButton6_clicked()
{
    calculator.addToExpressionText("6");
}
void MainWindow::on_pushButton7_clicked()
{
    calculator.addToExpressionText("7");
}
void MainWindow::on_pushButton8_clicked()
{
    calculator.addToExpressionText("8");
}
void MainWindow::on_pushButton9_clicked()
{
    calculator.addToExpressionText("9");
}

void MainWindow::slot_Calculator_expressionTextChanged(const QString & text)
{
    ui->lineEdit->setText(text);
    ui->lineEdit_2->setText(text);
}
void MainWindow::slot_Calculator_logTextChanged(const QString & text)
{
    ui->labelLog->setText(text);
    ui->labelLog_2->setText(text);
}

/**Process the calculator's response
  */
void  MainWindow::slot_Calculator_plotSetupChanged(const QString & text)
{
    ui->lineEditPlotSetup->setText(text);
    ui->plotWidget->update();
}

void MainWindow::on_pushButtonFn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonClear_clicked()
{
    calculator.addToExpressionText("C");
}

void MainWindow::on_pushButtonAllClear_clicked()
{
    calculator.addToExpressionText("AC");
}

/** Operator buttons. Add operator to expression
  */
void MainWindow::on_pushButtonDivide_clicked()
{
    calculator.addToExpressionText("/");
}


void MainWindow::on_pushButtonMultiply_clicked()
{
    calculator.addToExpressionText("*");
}
void MainWindow::on_pushButtonSubtract_clicked()
{
    calculator.addToExpressionText("-");
}
void MainWindow::on_pushButtonAdd_clicked()
{
    calculator.addToExpressionText("+");
}

void MainWindow::on_pushButtonEqual_clicked()
{
    calculator.addToExpressionText("=");
}
void MainWindow::on_pushButtonDot_clicked()
{
    calculator.addToExpressionText(".");
}
void MainWindow::on_pushButtonPlusMinus_clicked()
{
    calculator.addToExpressionText("+/-");
}


//Functions Page

void MainWindow::on_pushButtonStd_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButtonParenthesisOpen_clicked()
{
    calculator.addToExpressionText("(");
}

void MainWindow::on_pushButtonParenthesisClose_clicked()
{
    calculator.addToExpressionText(")");
}

void MainWindow::on_pushButtonE_clicked()
{
    calculator.addToExpressionText("E");
}

void MainWindow::on_pushButtonSin_clicked()
{
    calculator.addToExpressionText("sin");
}

void MainWindow::on_pushButtonCos_clicked()
{
calculator.addToExpressionText("cos");
}

void MainWindow::on_pushButtonTan_clicked()
{
    calculator.addToExpressionText("tan");
}

void MainWindow::on_pushButtonArc_clicked()
{
    calculator.addToExpressionText("arc");
}

void MainWindow::on_pushButtonSinh_clicked()
{
    calculator.addToExpressionText("sinh");
}

void MainWindow::on_pushButtonCosh_clicked()
{
    calculator.addToExpressionText("cosh");
}

void MainWindow::on_pushButtonTanh_clicked()
{
    calculator.addToExpressionText("tanh");
}

void MainWindow::on_pushButtonAr_clicked()
{
    calculator.addToExpressionText("ar");
}

void MainWindow::on_pushButtonExp_clicked()
{
    calculator.addToExpressionText("exp");
}

void MainWindow::on_pushButtonLn_clicked()
{
    calculator.addToExpressionText("ln");
}

void MainWindow::on_pushButtonLog_clicked()
{
    calculator.addToExpressionText("log");
}

void MainWindow::on_pushButtonSqrt_clicked()
{
    calculator.addToExpressionText("sqrt");
}

void MainWindow::on_pushButton1OverX_clicked()
{
    calculator.addToExpressionText("1/x");
}

void MainWindow::on_pushButtonAngleMode_clicked()
{
    calculator.addToExpressionText(ui->pushButtonAngleMode->text());
    ui->labelAngleMode->setText(ui->pushButtonAngleMode->text());
    if(ui->pushButtonAngleMode->text() == "deg")
       ui->pushButtonAngleMode->setText("rad");
    else ui->pushButtonAngleMode->setText("deg");
}

void MainWindow::on_pushButtonPower_clicked()
{
    calculator.addToExpressionText("x^y");
}

void MainWindow::on_pushButtonPlot_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

/**
  * Exit program.
  */
void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString selectedFilter = tr("*.pdf");
    QString fileName = QFileDialog::getOpenFileName
        (this,
         tr("Open File"),
         QDesktopServices::storageLocation ( QDesktopServices::DocumentsLocation),
         tr("OO and LO Documents *.odt;;PDF Documents *.pdf"),
         &selectedFilter);
   if(fileName.isEmpty())
    {
     QMessageBox::information(this,tr("Information"), tr("No file was selected."));
     return;
   }
   calculator.openProtocol(fileName);
}

/**
  * Show the error message.
  */
void MainWindow::slot_Calculator_errorMessage(QString caption, QString msg)
  {
    QMessageBox::critical(this, caption, msg);
  }

/**
  * Change the state of the GUI.
  */
void MainWindow::slot_Caculator_guiStateSuggested(QString state)
{
    if(state == STATE_STANDARD)
        ui->stackedWidget->setCurrentIndex(0);
    else if(state == STATE_FUNCTIONS)
        ui->stackedWidget->setCurrentIndex(1);
    else if(state == STATE_PLOT)
        ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButtonPlotStd_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonPlotFn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButtonPlotHelp_clicked()
{
    QMessageBox::information(this,tr("How to setup a plot"),
tr("\nUse the \"Y=\" and \"X=\" buttons to activate linear, 1/x or logarithmic scales.\n\
Set the range for x with the \"From x\" and \"To x\" buttons.\n\
Set the x distance from point to point with the \"Dx\" button."));
}

void MainWindow::sendPlotCommand(const QString & command)
{
    calculator.addToExpressionText("PlotCommand: " + command);
}

void MainWindow::on_pushButtonY_clicked()
{
        sendPlotCommand("Y=f(x)");
}

void MainWindow::on_pushButtonY1overfx_clicked()
{
        sendPlotCommand("Y=1/f(x)");
}

void MainWindow::on_pushButtonYlnfx_clicked()
{
        sendPlotCommand("Y=ln(f(x))");
}

void MainWindow::on_pushButtonXx_clicked()
{
        sendPlotCommand("X=x");
}

void MainWindow::on_pushButtonX1overx_clicked()
{
        sendPlotCommand("X=1/x");
}

void MainWindow::on_pushButtonXlnx_clicked()
{
        sendPlotCommand("X=ln(x)");
}

void MainWindow::on_pushButtonFromX_clicked()
{
       activatePlotInput("From x=");
}

void MainWindow::on_pushButtonToX_clicked()
{
       activatePlotInput("To x=");
}

void MainWindow::on_pushButtonDx_clicked()
{
        activatePlotInput("Dx=");
}

void MainWindow::on_pushButtonX_clicked()
{
    calculator.addToExpressionText("x");
}

void MainWindow::activatePlotInput(const QString & text)
{
    ui->pushButtonPlotDescription->setText(text);
    ui->framePlotInputColumn->show();
    ui->framePlotButtonGrid->hide();
}

void MainWindow::on_pushButtonPlotDescription_clicked()
{

}

void MainWindow::on_pushButtonPlotC_clicked()
{
    QString text = ui->lineEditPlotNumber->text();
    text = text.left(text.length() -1);
    ui->lineEditPlotNumber->setText(text);
}

void MainWindow::on_pushButtonPlotOk_clicked()
{
    sendPlotCommand(ui->pushButtonPlotDescription->text() + " " + ui->lineEditPlotNumber->text());
    ui->framePlotInputColumn->hide();
    ui->framePlotButtonGrid->show();
}

