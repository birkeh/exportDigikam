/*!
 \file cexportdialog.cpp

*/

#include "cexportdialog.h"
#include "ui_cexportdialog.h"

#include <QSettings>
#include <QCloseEvent>
#include <QImageWriter>
#include <QFileDialog>


cExportDialog::cExportDialog(const QList<IMAGEFORMAT>& imageFormats, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cExportDialog)
{
	initUI(imageFormats);
	createActions();
}

cExportDialog::~cExportDialog()
{
	delete ui;
}

void cExportDialog::accept()
{
	saveSettings();
	QDialog::accept();
}

void cExportDialog::reject()
{
	saveSettings();
	QDialog::reject();
}

void cExportDialog::saveSettings()
{
	QSettings	settings;
	settings.setValue("export/width", QVariant::fromValue(size().width()));
	settings.setValue("export/height", QVariant::fromValue(size().height()));
	settings.setValue("export/x", QVariant::fromValue(x()));
	settings.setValue("export/y", QVariant::fromValue(y()));


	if(ui->m_lpNewDirectoryTag->isChecked())
		settings.setValue("export/directoryMethod", QVariant::fromValue(QString("newDirectoryTag")));
	else if(ui->m_lpNewDirectory->isChecked())
		settings.setValue("export/directoryMethod", QVariant::fromValue(QString("newDirectory")));
	else
		settings.setValue("export/directoryMethod", QVariant::fromValue(QString("keepDirectory")));

	settings.setValue("export/destinationPath", QVariant::fromValue(ui->m_lpDestinationPath->text()));
	settings.setValue("export/keepStructure", QVariant::fromValue(ui->m_lpKeepStructure->isChecked()));
	settings.setValue("export/destinationPathTag", QVariant::fromValue(ui->m_lpDestinationPathTag->text()));


	if(ui->m_lpNewFilename->isChecked())
		settings.setValue("export/fileMethod", QVariant::fromValue(QString("newFilename")));
	else
		settings.setValue("export/fileMethod", QVariant::fromValue(QString("keepFilename")));

	if(ui->m_lpFilenameTag->isChecked())
		settings.setValue("export/filenamePlus", QVariant::fromValue(QString("TAG")));
	else
		settings.setValue("export/filenamePlus", QVariant::fromValue(QString("converted")));

	settings.setValue("export/fileTag", QVariant::fromValue(ui->m_lpFileTag->text()));


	if(ui->m_lpFileNoExport->isChecked())
		settings.setValue("export/overwrite", QVariant::fromValue(QString("no")));
	else if(ui->m_lpFileOverwrite->isChecked())
		settings.setValue("export/overwrite", QVariant::fromValue(QString("overwrite")));
	else if(ui->m_lpFileRename->isChecked())
		settings.setValue("export/overwrite", QVariant::fromValue(QString("rename")));
	else
		settings.setValue("export/overwrite", QVariant::fromValue(QString("ask")));

	settings.setValue("export/copyEXIF", QVariant::fromValue(ui->m_lpCopyEXIF->isChecked()));
	settings.setValue("export/fileFormat", QVariant::fromValue(ui->m_lpFileFormat->currentText()));
	settings.setValue("export/quality", QVariant::fromValue(ui->m_lpQuality->value()));

	settings.setValue("export/stripLastDirectory", QVariant::fromValue(ui->m_lpStripLastDirectory->isChecked()));
	settings.setValue("export/stripLastDirectoryIf", QVariant::fromValue(ui->m_lpStripLastDirectoryIf->isChecked()));
	settings.setValue("export/stripLastDirectoryIfList", QVariant::fromValue(ui->m_lpStripLastDirectoryIfList->toPlainText()));
}

void cExportDialog::initUI(const QList<IMAGEFORMAT>& imageFormats)
{
	QSettings	settings;

	ui->setupUi(this);

	qint32		iX		= settings.value("export/x", QVariant::fromValue(-1)).toInt();
	qint32		iY		= settings.value("export/y", QVariant::fromValue(-1)).toInt();
	qint32		iWidth	= settings.value("export/width", QVariant::fromValue(-1)).toInt();
	qint32		iHeight	= settings.value("export/height", QVariant::fromValue(-1)).toInt();

	if(iWidth != -1 && iHeight != -1)
		resize(iWidth, iHeight);
	if(iX != -1 && iY != -1)
		move(iX, iY);

	QString	directoryMethod	= settings.value("export/directoryMethod", QVariant::fromValue(QString("keepDirectory"))).toString();

	if(directoryMethod == "newDirectoryTag")
	{
		ui->m_lpKeepDirectory->setChecked(false);
		ui->m_lpNewDirectory->setChecked(false);
		ui->m_lpNewDirectoryTag->setChecked(true);

		ui->m_lpDestinationPath->setEnabled(false);
		ui->m_lpDestinationPathBrowse->setEnabled(false);
	}
	else if(directoryMethod == "newDirectory")
	{
		ui->m_lpKeepDirectory->setChecked(false);
		ui->m_lpNewDirectory->setChecked(true);
		ui->m_lpNewDirectoryTag->setChecked(false);

		ui->m_lpDestinationPathTag->setEnabled(false);
	}
	else
	{
		ui->m_lpKeepDirectory->setChecked(true);
		ui->m_lpNewDirectory->setChecked(false);
		ui->m_lpNewDirectoryTag->setChecked(false);

		ui->m_lpDestinationPath->setEnabled(false);
		ui->m_lpDestinationPathBrowse->setEnabled(false);
		ui->m_lpDestinationPathTag->setEnabled(false);

		ui->m_lpKeepStructure->setEnabled(false);
	}

	ui->m_lpStripLastDirectory->setChecked(settings.value("export/stripLastDirectory", QVariant::fromValue(false)).toBool());
	ui->m_lpStripLastDirectoryIf->setChecked(settings.value("export/stripLastDirectoryIf", QVariant::fromValue(false)).toBool());
	ui->m_lpStripLastDirectoryIfList->setText(settings.value("export/stripLastDirectoryIfList", QVariant::fromValue(QString(""))).toString());

	onStripLastDirectoryChanged(ui->m_lpStripLastDirectory->checkState());

	ui->m_lpDestinationPath->setText(settings.value("export/destinationPath", QVariant::fromValue(QString(""))).toString());
	ui->m_lpKeepStructure->setChecked(settings.value("export/keepStructure", QVariant::fromValue(false)).toBool());
	ui->m_lpDestinationPathTag->setText(settings.value("export/destinationPathTag", QVariant::fromValue(QString(""))).toString());

	QString	fileMethod		= settings.value("export/fileMethod", QVariant::fromValue(QString("keepFilename"))).toString();

	if(fileMethod == "newFilename")
	{
		ui->m_lpKeepFilename->setChecked(false);
		ui->m_lpNewFilename->setChecked(true);

		ui->m_lpFilenamePlusConverted->setEnabled(true);
		ui->m_lpFilenameTag->setEnabled(true);
		ui->m_lpFileTag->setEnabled(true);
	}
	else
	{
		ui->m_lpKeepFilename->setChecked(true);
		ui->m_lpNewFilename->setChecked(false);

		ui->m_lpFilenamePlusConverted->setEnabled(false);
		ui->m_lpFilenameTag->setEnabled(false);
		ui->m_lpFileTag->setEnabled(false);
	}

	QString	filenamePlus	= settings.value("export/filenamePlus", QVariant::fromValue(QString("converted"))).toString();

	if(filenamePlus == "TAG")
	{
		ui->m_lpFilenamePlusConverted->setChecked(false);
		ui->m_lpFilenameTag->setChecked(true);
	}
	else
	{
		ui->m_lpFilenamePlusConverted->setChecked(true);
		ui->m_lpFilenameTag->setChecked(false);

		ui->m_lpFileTag->setEnabled(false);
	}

	ui->m_lpFileTag->setText(settings.value("export/fileTag", QVariant::fromValue(QString(""))).toString());

	QString	fileOverwrite	= settings.value("export/overwrite", QVariant::fromValue(QString("ask"))).toString();

	if(fileOverwrite == "no")
	{
		ui->m_lpFileNoExport->setChecked(true);
		ui->m_lpFileAsk->setChecked(false);
		ui->m_lpFileRename->setChecked(false);
		ui->m_lpFileOverwrite->setChecked(false);
	}
	else if(fileOverwrite == "overwrite")
	{
		ui->m_lpFileNoExport->setChecked(false);
		ui->m_lpFileAsk->setChecked(false);
		ui->m_lpFileRename->setChecked(false);
		ui->m_lpFileOverwrite->setChecked(true);
	}
	else if(fileOverwrite == "rename")
	{
		ui->m_lpFileNoExport->setChecked(false);
		ui->m_lpFileAsk->setChecked(false);
		ui->m_lpFileRename->setChecked(true);
		ui->m_lpFileOverwrite->setChecked(false);
	}
	else
	{
		ui->m_lpFileNoExport->setChecked(false);
		ui->m_lpFileAsk->setChecked(true);
		ui->m_lpFileRename->setChecked(false);
		ui->m_lpFileOverwrite->setChecked(false);
	}

	for(int x = 0;x < imageFormats.count();x++)
	{
		IMAGEFORMAT	i	= imageFormats[x];

		if(i.write)
			ui->m_lpFileFormat->addItem(i.description + " (" + i.extension + ")");
	}
	ui->m_lpFileFormat->setCurrentText(settings.value("export/fileFormat").toString());
	(ui->m_lpCopyEXIF->setChecked(settings.value("export/copyEXIF", QVariant::fromValue(true)).toBool()));
	ui->m_lpQuality->setValue(settings.value("export/quality", QVariant::fromValue(50)).toInt());
	ui->m_lpQualityValue->setText(QString::number(ui->m_lpQuality->value()));

	ui->m_lpPathTagHelp->setText("<table>"
								 "<tr><td>%o</td><td>original directory</td></tr>"
								 "<tr><td>%y</td><td>year of picture taken</td></tr>"
								 "<tr><td>%m</td><td>month of picture taken</td></tr>"
								 "<tr><td>%d</td><td>day of picture taken</td></tr>"
								 "<tr><td>%H</td><td>hour of picture taken</td></tr>"
								 "<tr><td>%M</td><td>minute of picture taken</td></tr>"
								 "<tr><td>%S</td><td>second of picture taken</td></tr>"
								 "<tr><td>%c</td><td>camera manufacturer</td></tr>"
								 "<tr><td>%l</td><td>camera model</td></tr>"
								 "<tr><td>%t</td><td>type of created picture</td></tr>"
								 "</table>");
	ui->m_lpFileTagHelp->setText("<table>"
								 "<tr><td>%o</td><td>original filename (without extension)</td></tr>"
								 "<tr><td>%y</td><td>year of picture taken</td></tr>"
								 "<tr><td>%m</td><td>month of picture taken</td></tr>"
								 "<tr><td>%d</td><td>day of picture taken</td></tr>"
								 "<tr><td>%H</td><td>hour of picture taken</td></tr>"
								 "<tr><td>%M</td><td>minute of picture taken</td></tr>"
								 "<tr><td>%S</td><td>second of picture taken</td></tr>"
								 "<tr><td>%c</td><td>camera manufacturer</td></tr>"
								 "<tr><td>%l</td><td>camera model</td></tr>"
								 "<tr><td>%t</td><td>type of created picture</td></tr>"
								 "</table>");

	onFileFormatChanged(ui->m_lpFileFormat->currentText());
}

void cExportDialog::createActions()
{
	QSettings	settings;

	connect(ui->m_lpQuality,				&QSlider::valueChanged,											this,	&cExportDialog::onQualityChanged);

	connect(ui->m_lpDirectoryMethod,		QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),	this,	&cExportDialog::onDirectoryMethodChanged);
	connect(ui->m_lpFilenameMethod,			QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),	this,	&cExportDialog::onFileMethodChanged);
	connect(ui->m_lpFilenameAdd,			QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),	this,	&cExportDialog::onFileNamePlusChanged);
	connect(ui->m_lpFileOverwriteMethod,	QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),	this,	&cExportDialog::onFileOverwriteMethodChanged);

	connect(ui->m_lpStripLastDirectory,		&QCheckBox::stateChanged,										this,	&cExportDialog::onStripLastDirectoryChanged);
	connect(ui->m_lpStripLastDirectoryIf,	&QCheckBox::stateChanged,										this,	&cExportDialog::onStripLastDirectoryIfChanged);

	connect(ui->m_lpFileFormat,				&QComboBox::currentTextChanged,									this,	&cExportDialog::onFileFormatChanged);

	connect(ui->m_lpDestinationPathBrowse,	&QPushButton::clicked,											this,	&cExportDialog::onPathSelect);
}

void cExportDialog::onQualityChanged(int value)
{
	ui->m_lpQualityValue->setNum(value);
}

void cExportDialog::onDirectoryMethodChanged(QAbstractButton* /*button*/)
{
	if(ui->m_lpNewDirectoryTag->isChecked())
	{
		ui->m_lpDestinationPath->setEnabled(false);
		ui->m_lpDestinationPathBrowse->setEnabled(false);
		ui->m_lpKeepStructure->setEnabled(false);
		ui->m_lpDestinationPathTag->setEnabled(true);
	}
	else if(ui->m_lpNewDirectory->isChecked())
	{
		ui->m_lpDestinationPath->setEnabled(true);
		ui->m_lpDestinationPathBrowse->setEnabled(true);
		ui->m_lpKeepStructure->setEnabled(true);
		ui->m_lpDestinationPathTag->setEnabled(false);
	}
	else
	{
		ui->m_lpDestinationPath->setEnabled(false);
		ui->m_lpDestinationPathBrowse->setEnabled(false);
		ui->m_lpKeepStructure->setEnabled(false);
		ui->m_lpDestinationPathTag->setEnabled(false);
	}
}

void cExportDialog::onFileMethodChanged(QAbstractButton* /*button*/)
{
	if(ui->m_lpNewFilename->isChecked())
	{
		ui->m_lpFilenamePlusConverted->setEnabled(true);
		ui->m_lpFilenameTag->setEnabled(true);
		ui->m_lpFilenamePlusConverted->setEnabled(true);
		ui->m_lpFilenameTag->setEnabled(true);
		onFileNamePlusChanged(0);
	}
	else
	{
		ui->m_lpFilenamePlusConverted->setEnabled(false);
		ui->m_lpFilenameTag->setEnabled(false);
		ui->m_lpFileTag->setEnabled(false);
	}
}

void cExportDialog::onFileNamePlusChanged(QAbstractButton* /*button*/)
{
	if(ui->m_lpNewFilename->isChecked() && ui->m_lpFilenameTag->isChecked())
		ui->m_lpFileTag->setEnabled(true);
	else
		ui->m_lpFileTag->setEnabled(false);
}

void cExportDialog::onFileOverwriteMethodChanged(QAbstractButton* /*button*/)
{
	//NOTHING
}

void cExportDialog::onFileFormatChanged(const QString &text)
{
	QString			ext	= text.mid(text.lastIndexOf("*.")+1).replace(")", "");
	QImageWriter	writer("test" + ext);

	if(writer.supportsOption(QImageIOHandler::Quality))
	{
		ui->m_lpQuality->setEnabled(true);
		ui->m_lpQualityValue->setEnabled(true);
	}
	else
	{
		ui->m_lpQuality->setEnabled(false);
		ui->m_lpQualityValue->setEnabled(false);
	}
}

void cExportDialog::onPathSelect()
{
	QSettings	settings;
	QString		szPath	= settings.value("export/path", QDir::homePath()).toString();
	szPath	= QFileDialog::getExistingDirectory(this, tr("Export Directory"), szPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(szPath.isEmpty())
		return;

	ui->m_lpDestinationPath->setText(szPath);
}

void cExportDialog::onStripLastDirectoryChanged(int state)
{
	if(state == Qt::Unchecked)
	{
		ui->m_lpStripLastDirectoryIf->setEnabled(false);
		ui->m_lpStripLastDirectoryIfList->setEnabled(false);
	}
	else
	{
		ui->m_lpStripLastDirectoryIf->setEnabled(true);
		onStripLastDirectoryIfChanged(ui->m_lpStripLastDirectoryIf->checkState());
	}
}

void cExportDialog::onStripLastDirectoryIfChanged(int state)
{
	if(state == Qt::Unchecked)
	{
		ui->m_lpStripLastDirectoryIfList->setEnabled(false);
	}
	else
	{
		ui->m_lpStripLastDirectoryIfList->setEnabled(true);
	}
}

/*
				QImageWriter	writer(destFile);

				qDebug() << "QImageIOHandler::Size: " << writer.supportsOption(QImageIOHandler::Size);
				qDebug() << "QImageIOHandler::ClipRect: " << writer.supportsOption(QImageIOHandler::ClipRect);
				qDebug() << "QImageIOHandler::ScaledSize: " << writer.supportsOption(QImageIOHandler::ScaledSize);
				qDebug() << "QImageIOHandler::ScaledClipRect: " << writer.supportsOption(QImageIOHandler::ScaledClipRect);
				qDebug() << "QImageIOHandler::Description: " << writer.supportsOption(QImageIOHandler::Description);
				qDebug() << "QImageIOHandler::CompressionRatio: " << writer.supportsOption(QImageIOHandler::CompressionRatio);
				qDebug() << "QImageIOHandler::Gamma: " << writer.supportsOption(QImageIOHandler::Gamma);
				qDebug() << "QImageIOHandler::Quality: " << writer.supportsOption(QImageIOHandler::Quality);
				qDebug() << "QImageIOHandler::Name: " << writer.supportsOption(QImageIOHandler::Name);
				qDebug() << "QImageIOHandler::SubType: " << writer.supportsOption(QImageIOHandler::SubType);
				qDebug() << "QImageIOHandler::IncrementalReading: " << writer.supportsOption(QImageIOHandler::IncrementalReading);
				qDebug() << "QImageIOHandler::Endianness: " << writer.supportsOption(QImageIOHandler::Endianness);
				qDebug() << "QImageIOHandler::Animation: " << writer.supportsOption(QImageIOHandler::Animation);
				qDebug() << "QImageIOHandler::BackgroundColor: " << writer.supportsOption(QImageIOHandler::BackgroundColor);
				qDebug() << "QImageIOHandler::ImageFormat: " << writer.supportsOption(QImageIOHandler::ImageFormat);
				qDebug() << "QImageIOHandler::SupportedSubTypes: " << writer.supportsOption(QImageIOHandler::SupportedSubTypes);
				qDebug() << "QImageIOHandler::OptimizedWrite: " << writer.supportsOption(QImageIOHandler::OptimizedWrite);
				qDebug() << "QImageIOHandler::ProgressiveScanWrite: " << writer.supportsOption(QImageIOHandler::ProgressiveScanWrite);
				qDebug() << "QImageIOHandler::ImageTransformation: " << writer.supportsOption(QImageIOHandler::ImageTransformation);
				qDebug() << "QImageIOHandler::TransformedByDefault: " << writer.supportsOption(QImageIOHandler::TransformedByDefault);
*/
