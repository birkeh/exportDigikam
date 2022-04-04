#ifndef CLOGWINDOW_H
#define CLOGWINDOW_H

#include <QDialog>

namespace Ui {
class cLogWindow;
}

class cLogWindow : public QDialog
{
	Q_OBJECT

public:
	explicit cLogWindow(QWidget *parent = nullptr);
	~cLogWindow();

	void				setText(const QString& text);

private:
	Ui::cLogWindow*		ui;
};

#endif // CLOGWINDOW_H
