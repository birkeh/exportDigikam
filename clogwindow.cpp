#include "clogwindow.h"
#include "ui_clogwindow.h"


cLogWindow::cLogWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cLogWindow)
{
	ui->setupUi(this);
}

cLogWindow::~cLogWindow()
{
	delete ui;
}

void cLogWindow::setText(const QString& text)
{
	ui->m_lpLog->setHtml(text);
}
