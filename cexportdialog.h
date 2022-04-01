/*!
 \file cexportdialog.h

*/

#ifndef CEXPORTDIALOG_H
#define CEXPORTDIALOG_H


#include <QDialog>
#include <QAbstractButton>

#include "common.h"


/*
 * altes Verzeichnis
 * neues Verzeichnis, alte Struktur
 * neues Verzeichnis
 *
 * alter Name (umbenennen, wenn gleich)
 * alter Name + _converted
 * alter Name + tag
 * tag
 *
 * überschreiben / fragen / umbenennen
*/

namespace Ui {
class cExportDialog;
}

/*!
 \brief

 \class cExportDialog cexportdialog.h "cexportdialog.h"
*/
class cExportDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	 \brief

	 \fn cExportDialog
	 \param imageFormats
	 \param parent
	*/
	explicit cExportDialog(const QList<IMAGEFORMAT>& imageFormats, QWidget *parent = nullptr);
	/*!
	 \brief

	 \fn ~cExportDialog
	*/
	~cExportDialog();

private:
	Ui::cExportDialog *ui; /*!< TODO: describe */

	/*!
	 \brief

	 \fn accept
	*/
	void							accept();
	/*!
	 \brief

	 \fn reject
	*/
	void							reject();

	/*!
	 \brief

	 \fn saveSettings
	*/
	void							saveSettings();

	/*!
	 \brief

	 \fn initUI
	 \param imageFormats
	*/
	void							initUI(const QList<IMAGEFORMAT>& imageFormats);
	/*!
	 \brief

	 \fn createActions
	*/
	void							createActions();

private slots:
	/*!
	 \brief

	 \fn onQualityChanged
	 \param value
	*/
	void							onQualityChanged(int value);

	/*!
	 \brief

	 \fn onDirectoryMethodChanged
	 \param button
	*/
	void							onDirectoryMethodChanged(QAbstractButton* button);
	/*!
	 \brief

	 \fn onStripLastDirectoryChanged
	 \param button
	*/
	void							onStripLastDirectoryChanged(int state);
	/*!
	 \brief

	 \fn onStripLastDirectoryIfChanged
	 \param button
	*/
	void							onStripLastDirectoryIfChanged(int state);
	/*!
	 \brief

	 \fn onFileMethodChanged
	 \param button
	*/
	void							onFileMethodChanged(QAbstractButton* button);
	/*!
	 \brief

	 \fn onFileNamePlusChanged
	 \param button
	*/
	void							onFileNamePlusChanged(QAbstractButton* button);
	/*!
	 \brief

	 \fn onFileOverwriteMethodChanged
	 \param button
	*/
	void							onFileOverwriteMethodChanged(QAbstractButton* button);
	/*!
	 \brief

	 \fn onFileFormatChanged
	 \param text
	*/
	void							onFileFormatChanged(const QString &text);
	/*!
	 \brief

	 \fn onPathSelect
	*/
	void							onPathSelect();

protected:
};

#endif // CEXPORTDIALOG_H
