#include "common.h"

#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QSqlError>

#include <QSettings>
#include <QDebug>

#include "cimage.h"
#include "cexportdialog.h"
#include "clogwindow.h"

#include <QImageReader>
#include <QImageWriter>
#include <QSqlQuery>

#include <QMessageBox>
#include <QElapsedTimer>

#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>


#define DBROOT		"C:\\Users\\birkeh\\OneDrive\\__MEDIA__"

void addToExportLog(QString& exportLog, const QString& text)
{
	QDateTime	now	= QDateTime::currentDateTime();

	exportLog.append("    <tr>\n        <td class='time'>" + now.toString("yyyy-mm-dd hh:MM:ss") + "</td>\n        <td>" + text + "</td>\n    </tr>\n");
}

cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow),
	m_progressBar(nullptr),
	m_listToolBar(nullptr),
//	m_refreshAction(nullptr),
	m_actionToolBar(nullptr),
	m_exportAction(nullptr),
	m_stopAction(nullptr),
	m_albumRootsList(nullptr),
	m_albumsList(nullptr),
	m_folderViewModel(nullptr),
	m_folderSortFilterProxyModel(nullptr),
	m_includeViewModel(nullptr),
	m_excludeViewModel(nullptr),
	m_thumbnailViewModel(nullptr),
	m_thumbnailSortFilterProxyModel(nullptr),
	m_rootItem(nullptr),
	m_exifTAGList(nullptr),
	m_exifCompressionList(nullptr),
	m_exifLightSourceList(nullptr),
	m_exifFlashList(nullptr),
	m_iptcTagList(nullptr),
	m_xmpTagList(nullptr),
	m_loading(false),
	m_working(false),
	m_stopIt(false),
	m_exportLog("")
{
	initUI();
	createActions();

	m_volumes	= actuallyListVolumes();

	setImageFormats();
	loadData();
	initSignals();
}

cMainWindow::~cMainWindow()
{
	_DELETE_(m_progressBar);
	_DELETE_(m_listToolBar);
	_DELETE_(m_refreshAction);
	_DELETE_(m_actionToolBar);
	_DELETE_(m_exportAction);
	_DELETE_(m_stopAction);
	_DELETE_(m_albumRootsList);
	_DELETE_(m_albumsList);
	_DELETE_(m_excludeViewModel);
	_DELETE_(m_includeViewModel);
	_DELETE_(m_folderViewModel);
	_DELETE_(m_folderSortFilterProxyModel);
	_DELETE_(m_thumbnailViewModel);
	_DELETE_(m_thumbnailSortFilterProxyModel);
	_DELETE_(m_rootItem);
	_DELETE_(m_exifTAGList);
	_DELETE_(m_exifCompressionList);
	_DELETE_(m_exifLightSourceList);
	_DELETE_(m_exifFlashList);
	_DELETE_(m_iptcTagList);
	_DELETE_(m_xmpTagList);

	if(m_dbThumbnail.isOpen())
		m_dbThumbnail.close();

	if(m_dbDigikam.isOpen())
		m_dbDigikam.close();

	if(m_dbExportDigikam.isOpen())
		m_dbExportDigikam.close();

	delete ui;
}

void cMainWindow::closeEvent(QCloseEvent *event)
{
	QSettings	settings;
	settings.setValue("main/width", QVariant::fromValue(size().width()));
	settings.setValue("main/height", QVariant::fromValue(size().height()));
	settings.setValue("main/x", QVariant::fromValue(x()));
	settings.setValue("main/y", QVariant::fromValue(y()));
	if(this->isMaximized())
		settings.setValue("main/maximized", QVariant::fromValue(true));
	else
		settings.setValue("main/maximized", QVariant::fromValue(false));

	QList<qint32>	sizes	= ui->m_splitter->sizes();

	for(int x = 0;x < sizes.count();x++)
		settings.setValue(QString("main/splitter%1").arg(x+1), QVariant::fromValue(sizes[x]));

//	m_filterPanel->saveSettings();

	event->accept();
}

void cMainWindow::initUI()
{
	ui->setupUi(this);

	QIcon::setThemeName("TangoMFK");

	m_folderViewModel				= new QStandardItemModel;
	m_folderSortFilterProxyModel	= new cFolderSortFilterProxyModel(this);
	ui->m_folderView->setModel(m_folderSortFilterProxyModel);
	m_folderSortFilterProxyModel->setSourceModel(m_folderViewModel);

	m_thumbnailViewModel			= new QStandardItemModel;
	m_thumbnailSortFilterProxyModel	= new cThumbnailSortFilterProxyModel(this);
	ui->m_thumbnailView->setModel(m_thumbnailSortFilterProxyModel);
	m_thumbnailSortFilterProxyModel->setSourceModel(m_thumbnailViewModel);

	m_includeViewModel				= new QStandardItemModel;
	ui->m_includeView->setModel(m_includeViewModel);

	m_excludeViewModel				= new QStandardItemModel;
	ui->m_excludeView->setModel(m_excludeViewModel);

	QSettings	settings;

	if(!settings.value("main/maximized").toBool())
	{
		qint32		iX		= settings.value("main/x", QVariant::fromValue(-1)).toInt();
		qint32		iY		= settings.value("main/y", QVariant::fromValue(-1)).toInt();
		qint32		iWidth	= settings.value("main/width", QVariant::fromValue(-1)).toInt();
		qint32		iHeight	= settings.value("main/height", QVariant::fromValue(-1)).toInt();

		if(iWidth != -1 && iHeight != -1)
			resize(iWidth, iHeight);
		if(iX != -1 && iY != -1)
			move(iX, iY);
	}

	qint32		iWidth1	= settings.value("main/splitter1", QVariant::fromValue(-1)).toInt();
	qint32		iWidth2	= settings.value("main/splitter2", QVariant::fromValue(-1)).toInt();
	qint32		iWidth3	= settings.value("main/splitter3", QVariant::fromValue(-1)).toInt();

	ui->m_splitter->setSizes(QList<int>() << iWidth1 << iWidth2 << iWidth3);

	m_progressBar			= new QProgressBar(this);
	m_progressBar->setVisible(false);
	ui->m_statusBar->addPermanentWidget(m_progressBar);
}

void cMainWindow::createActions()
{
	setToolButtonStyle(Qt::ToolButtonFollowStyle);

	createMenuActions();
	createContextActions();
}

void cMainWindow::createMenuActions()
{
	m_listToolBar	= addToolBar("list");

	const QIcon	refreshIcon			= QIcon::fromTheme("view-refresh");
	m_refreshAction					= m_listToolBar->addAction(refreshIcon, tr("&Refresh"), this, &cMainWindow::onRefreshList);
	m_refreshAction->setShortcut(Qt::Key_F5);


	m_actionToolBar	= addToolBar("action");

	const QIcon	startIcon			= QIcon::fromTheme("media-playback-start");
	m_exportAction					= m_actionToolBar->addAction(startIcon, tr("&Export"), this, &cMainWindow::onExport);
	m_exportAction->setShortcut(Qt::CTRL | Qt::Key_R);

	const QIcon	stopIcon			= QIcon::fromTheme("process-stop");
	m_stopAction					= m_actionToolBar->addAction(stopIcon, tr("&Stop"), this, &cMainWindow::onStop);
}

void cMainWindow::createContextActions()
{
}

void cMainWindow::setImageFormats()
{
	QList<QByteArray>	readList	= QImageReader::supportedImageFormats();
	QList<QByteArray>	writeList	= QImageWriter::supportedImageFormats();

	m_dbExportDigikam	= QSqlDatabase::addDatabase("QSQLITE", "exportDigikam");
	m_dbExportDigikam.setHostName("localhost");
	m_dbExportDigikam.setDatabaseName("exportDigikam.db");

	if(!m_dbExportDigikam.open())
		return;

	QSqlQuery	query(m_dbExportDigikam);

	query.prepare("SELECT shortname, description, extension FROM imageFormat;");
	if(!query.exec())
	{
		qDebug() << "reading from exportDigikam.db failed. " << query.lastError();
		return;
	}

	while(query.next())
		addImageFormat(query.value("shortname").toString(), query.value("description").toString(), query.value("extension").toString(), readList, writeList);

	m_exifTAGList			= new cEXIFTagList(&m_dbExportDigikam);
	m_exifCompressionList	= new cEXIFCompressionList(&m_dbExportDigikam);
	m_exifLightSourceList	= new cEXIFLightSourceList(&m_dbExportDigikam);
	m_exifFlashList			= new cEXIFFlashList(&m_dbExportDigikam);
	m_iptcTagList			= new cIPTCTagList(&m_dbExportDigikam);
	m_xmpTagList			= new cXMPTagList(&m_dbExportDigikam);
}

void cMainWindow::addImageFormat(const QString& shortName, const QString& description, const QString& extension, QList<QByteArray>& readList, QList<QByteArray>& writeList)
{
	bool	r	= readList.contains(QByteArray(shortName.toUtf8()));
	bool	w	= writeList.contains(QByteArray(shortName.toUtf8()));

	if(QString(shortName).isEmpty())
		r	= true;

	IMAGEFORMAT	i;
	i.shortName		= shortName;
	i.description	= description;
	i.extension		= extension;
	i.read			= r;
	i.write			= w;
	m_imageFormats.append(i);
}

void cMainWindow::loadData()
{
	m_includeViewModel->clear();
	m_excludeViewModel->clear();
	m_thumbnailViewModel->clear();
	m_folderViewModel->clear();

//	QElapsedTimer	timer;

//	timer.start();

	if(m_albumRootsList)
		delete m_albumRootsList;

	if(m_albumsList)
		delete m_albumsList;

	if(!m_dbDigikam.isOpen())
	{
		m_dbDigikam		= QSqlDatabase::addDatabase("QSQLITE", "digikam4");
		m_dbDigikam.setDatabaseName(QString(DBROOT) + QString("\\digikam4.db"));
		if(!m_dbDigikam.open())
		{
			qDebug() << "Digikam: DB Open failed. " << m_dbDigikam.lastError().text();
			return;
		}
	}

	if(!m_dbThumbnail.isOpen())
	{
		m_dbThumbnail	= QSqlDatabase::addDatabase("QSQLITE", "thumbnails-digikam");
		m_dbThumbnail.setDatabaseName(DBROOT + QString("\\thumbnails-digikam.db"));
		if(!m_dbThumbnail.open())
		{
			qDebug() << "Thumbnail: DB Open failed. " << m_dbDigikam.lastError().text();
			return;
		}
	}

	m_tagsList			= new cTagsList(&m_dbDigikam);
	if(!m_tagsList)
	{
		qDebug() << "Tags List: out of memory.";
		return;
	}

	if(!m_tagsList->load())
	{
		qDebug() << "Tags List: load failed.";
		return;
	}

	m_albumRootsList	= new cAlbumRootsList(&m_dbDigikam, &m_dbThumbnail, this);
	if(!m_albumRootsList)
	{
		qDebug() << "Album Root List: out of memory.";
		return;
	}

	if(!m_albumRootsList->load(&m_volumes))
	{
		qDebug() << "Album Root List: load failed.";
		return;
	}

	m_albumsList		= new cAlbumsList(&m_dbDigikam, &m_dbThumbnail, m_albumRootsList, this);
	if(!m_albumsList)
	{
		qDebug() << "Album List: out of memory.";
		return;
	}

	if(!m_albumsList->load())
	{
		qDebug() << "Album List: load failed.";
		return;
	}

//	qDebug() << "Loading took " << timer.elapsed() << "ms";

	displayData();
	ui->m_folderView->expandAll();
}

void cMainWindow::displayData()
{
	m_loading	= true;

	m_includeViewModel->clear();
	m_excludeViewModel->clear();

	for(int x = 0;x < m_tagsList->count();x++)
	{
		cTags*		lpTags	= m_tagsList->at(x);

		if(!lpTags->parent())
		{
			QStandardItem*	lpInclude		= new QStandardItem(lpTags->name());
			lpInclude->setData(QVariant::fromValue(lpTags), Qt::UserRole+1);
			lpInclude->setCheckable(true);

			QStandardItem*	lpExclude		= new QStandardItem(lpTags->name());
			lpExclude->setData(QVariant::fromValue(lpTags), Qt::UserRole+1);
			lpExclude->setCheckable(true);

			lpTags->setIncludeItem(lpInclude);
			lpTags->setExcludeItem(lpExclude);

			m_includeViewModel->appendRow(lpInclude);
			m_excludeViewModel->appendRow(lpExclude);

			addChildren(lpTags, lpInclude, lpExclude);
		}
	}

	m_thumbnailViewModel->clear();
	m_folderViewModel->clear();

	m_rootItem	= new QStandardItem("library");
	m_folderViewModel->appendRow(m_rootItem);

	for(int x = 0;x < m_albumRootsList->count();x++)
	{
		QStandardItem*	lpItem		= new QStandardItem(m_albumRootsList->at(x)->label());
		lpItem->setData(QVariant::fromValue(m_albumRootsList->at(x)), Qt::UserRole+1);
		lpItem->setCheckable(true);
		m_albumRootsList->at(x)->setItem(lpItem);
		m_rootItem->appendRow(lpItem);
	}

	for(int x = 0;x < m_albumsList->count();x++)
	{
		cAlbums*		lpAlbums	= m_albumsList->at(x);

		QStandardItem*	lpItem		= new QStandardItem(lpAlbums->path());
		lpItem->setData(QVariant::fromValue(lpAlbums), Qt::UserRole+1);
		lpItem->setCheckable(true);

		QStandardItem*	lpRootItem	= findDBRootItem(m_folderViewModel, m_rootItem, lpAlbums->albumRoots());

		if(lpAlbums->parentAlbums())
			lpRootItem	= lpAlbums->parentAlbums()->item();

		if(!lpRootItem)
			delete(lpItem);
		else
		{
			lpAlbums->setItem(lpItem);
			lpRootItem->appendRow(lpItem);
		}
	}

	m_loading	= false;
}

void cMainWindow::addChildren(cTags* tags, QStandardItem* include, QStandardItem* exclude)
{
	cTagsList*	tagsList	= tags->childList();

	if(!tagsList)
		return;

	for(int i = 0;i < tagsList->count();i++)
	{
		cTags*	tags	= tagsList->at(i);

		QStandardItem*	lpInclude		= new QStandardItem(tags->name());
		lpInclude->setData(QVariant::fromValue(tags), Qt::UserRole+1);
		lpInclude->setCheckable(true);

		QStandardItem*	lpExclude		= new QStandardItem(tags->name());
		lpExclude->setData(QVariant::fromValue(tags), Qt::UserRole+1);
		lpExclude->setCheckable(true);

		tags->setIncludeItem(lpInclude);
		tags->setExcludeItem(lpExclude);

		include->appendRow(lpInclude);
		exclude->appendRow(lpExclude);

		addChildren(tags, lpInclude, lpExclude);
	}
}

QStandardItem* cMainWindow::findDBRootItem(QStandardItemModel* model, QStandardItem *rootItem, cAlbumRoots* albumRoots)
{
	QModelIndex	parent;

	if(rootItem)
		parent = rootItem->index();

	for(int x = 0;x < model->rowCount(parent);x++)
	{
		QStandardItem*	item		= model->itemFromIndex(model->index(x, 0, parent));
		if(item->data(Qt::UserRole+1).value<cAlbumRoots*>() == albumRoots)
			return(item);
	}

	return(nullptr);
}

QStandardItem* cMainWindow::findParentItem(QStandardItemModel* model, QStandardItem *rootItem, cAlbums* albumRoots)
{
	QModelIndex	parent;

	if(rootItem)
		parent = rootItem->index();

	for(int x = 0;x < model->rowCount(parent);x++)
	{
		QStandardItem*	item		= model->itemFromIndex(model->index(x, 0, parent));

		if(item->data(Qt::UserRole+1).value<cAlbums*>() == albumRoots)
			return(item);

		if(item->hasChildren())
		{
			item	= findParentItem(model, item, albumRoots);
			if(item)
				return(item);
		}
	}

	return(nullptr);
}

void cMainWindow::initSignals()
{
	connect(m_folderViewModel,						&QStandardItemModel::itemChanged,		this,	&cMainWindow::onFolderViewItemChanged);
	connect(m_includeViewModel,						&QStandardItemModel::itemChanged,		this,	&cMainWindow::onIncludeViewItemChanged);
	connect(m_excludeViewModel,						&QStandardItemModel::itemChanged,		this,	&cMainWindow::onExcludeViewItemChanged);
	connect(ui->m_folderView->selectionModel(),		&QItemSelectionModel::selectionChanged,	this,	&cMainWindow::onFolderSelected);
	connect(ui->m_thumbnailView->selectionModel(),	&QItemSelectionModel::selectionChanged,	this,	&cMainWindow::onThumbnailSelected);
}

void cMainWindow::onFolderViewItemChanged(QStandardItem* item)
{
	Qt::CheckState	state	=	item->checkState();
	QModelIndex		parent	=	item->index();

	cAlbums*		albums		= item->data().value<cAlbums*>();
	cImagesList*	imagesList	= nullptr;

	if(albums)
		imagesList	= albums->imagesList();

	if(imagesList)
	{
		for(int y = 0;y < imagesList->count();y++)
		{
			cImages*		images		= imagesList->at(y);
			QStandardItem*	imagesItem	= images->item();
			if(imagesItem)
				imagesItem->setCheckState(state);
		}
	}

	for(int x = 0;x < m_folderViewModel->rowCount(parent);x++)
	{
		QStandardItem*	curItem	= m_folderViewModel->itemFromIndex(m_folderViewModel->index(x, 0, parent));
		if(!curItem)
			continue;

		curItem->setCheckState(state);

		albums		= curItem->data().value<cAlbums*>();
		imagesList	= albums->imagesList();

		if(!imagesList)
			continue;
		if(!imagesList->count())
			continue;

		for(int y = 0;y < imagesList->count();y++)
		{
			cImages*		images		= imagesList->at(y);
			QStandardItem*	imagesItem	= images->item();
			if(imagesItem)
				imagesItem->setCheckState(state);
		}
	}
}

void cMainWindow::onIncludeViewItemChanged(QStandardItem* item)
{
	Qt::CheckState	state	=	item->checkState();
	QModelIndex		parent	=	item->index();

	for(int x = 0;x < m_includeViewModel->rowCount(parent);x++)
	{
		QStandardItem*	curItem	= m_includeViewModel->itemFromIndex(m_includeViewModel->index(x, 0, parent));
		if(!curItem)
			continue;

		curItem->setCheckState(state);
	}
}

void cMainWindow::onExcludeViewItemChanged(QStandardItem* item)
{
	Qt::CheckState	state	=	item->checkState();
	QModelIndex		parent	=	item->index();

	for(int x = 0;x < m_excludeViewModel->rowCount(parent);x++)
	{
		QStandardItem*	curItem	= m_excludeViewModel->itemFromIndex(m_excludeViewModel->index(x, 0, parent));
		if(!curItem)
			continue;

		curItem->setCheckState(state);
	}
}

void cMainWindow::onFolderSelected(const QItemSelection& /*selection*/, const QItemSelection& /*previous*/)
{
	if(m_loading)
		return;

	m_thumbnailViewModel->clear();
	ui->m_information->clear();

	if(!ui->m_folderView->selectionModel()->selectedIndexes().count())
		return;

	QModelIndex		index		= ui->m_folderView->selectionModel()->selectedIndexes()[0];
	if(!index.isValid())
		return;

	cAlbums*		albums		= m_folderSortFilterProxyModel->data(index, Qt::UserRole+1).value<cAlbums*>();
	Qt::CheckState	state		= albums->item()->checkState();
	bool			loaded		= albums->loadImages();
	cImagesList*	imagesList	= albums->imagesList();
	if(!imagesList)
		return;

	for(int x = 0;x < imagesList->count();x++)
	{
		cImages*		images	= imagesList->at(x);
		if(!images)
			continue;

		QStandardItem*	item	= images->item();
		if(loaded)
			item->setCheckState(state);

		m_thumbnailViewModel->appendRow(item);
	}
}

void cMainWindow::onThumbnailSelected(const QItemSelection& /*selection*/, const QItemSelection& /*previous*/)
{
	QModelIndex		index		= ui->m_thumbnailView->selectionModel()->selectedIndexes()[0];
	if(!index.isValid())
		return;

	cImages*		images		= m_thumbnailSortFilterProxyModel->data(index, Qt::UserRole+1).value<cImages*>();

	if(images)
	{
		QString	information;
		information	= "<!DOCTYPE html>\n";
		information.append("<html>\n");
		information.append("	<head>\n");
		information.append("		<style>\n");
		information.append("			body       { background-color: white; color: black; }\n");
		information.append("			h1         { color: black; }\n");
		information.append("			p          { color: black; }\n");
		information.append("			.time      { color: darkgrey; }\n");
		information.append("			.title     { color: darkblue; font-weight: bold; }\n");
		information.append("			.option    { color: darkmagenta; font-style: italic; }\n");
		information.append("			.optionok  { color: green; font-style: italic; }\n");
		information.append("			.optionnok { color: red; font-style: italic; }\n");
		information.append("			td         { color: black; }\n");
		information.append("			table      { width: 100%; }\n");
		information.append("		</style>\n");
		information.append("	</head>\n");
		information.append("	<body>\n");
		information.append("		<table style='width:100%'>\n");
		information.append("			<tr><td span=2 class='title'>File properties</td></tr>");
		information.append(QString("			<tr><td><i>File:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->name()));
		information.append(QString("			<tr><td><i>Folder:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->albums()->path()));
		information.append(QString("			<tr><td><i>Date:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->modificationDate().toString()));
		information.append(QString("			<tr><td><i>Size:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->fileSize()));
		information.append("			<tr><td span=2 class='title'>Image properties</td></tr>\n");
		information.append(QString("			<tr><td><i>Type:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageInformation().format));
		information.append(QString("			<tr><td><i>Size:</i></td><td align=\"right\">%1x%2</td></tr>\n").arg(images->imageInformation().width).arg(images->imageInformation().height));
		information.append(QString("			<tr><td><i>Color depth:</i></td><td align=\"right\">%1 bpp</td></tr>\n").arg(images->imageInformation().colorDepth));
		information.append("			<tr><td span=2 class='title'>Shoot properties</td></tr>\n");
		information.append(QString("			<tr><td><i>Camera:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageMetadata().model));
		information.append(QString("			<tr><td><i>Taken:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageInformation().creationDate.toString()));
		information.append(QString("			<tr><td><i>Lens:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageMetadata().lens));
		information.append(QString("			<tr><td><i>Aparture:</i></td><td align=\"right\">F%1</td></tr>\n").arg(images->imageMetadata().aparture));
		information.append(QString("			<tr><td><i>Focal length:</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageMetadata().focalLength));
		information.append(QString("			<tr><td><i>Focal length (35mm):</i></td><td align=\"right\">%1</td></tr>\n").arg(images->imageMetadata().focalLength35));
		information.append(QString("			<tr><td><i>Exposure time:</i></td><td align=\"right\">1/%1</td></tr>\n").arg(1/images->imageMetadata().exposureTime));
		information.append(QString("			<tr><td><i>Sensitivity:</i></td><td align=\"right\">ISO %1</td></tr>\n").arg(images->imageMetadata().sensitivity));
		information.append("		</table>\n");
		information.append("	</body>\n");
		m_exportLog.append("</html>\n");

		ui->m_information->setText(information);
	}
	else
		ui->m_information->clear();
}

void cMainWindow::onRefreshList()
{
	loadData();
}

void cMainWindow::onExport()
{
	bool	hasExport	= false;

	for(int x = 0;x < m_albumsList->count();x++)
	{
		cAlbums*	albums	= m_albumsList->at(x);
		QStandardItem*	item	= albums->item();

		if(item)
		{
			if(item->checkState() == Qt::Checked)
			{
				hasExport	= true;
				goto done;
			}
		}

		if(albums)
		{
			cImagesList*	imagesList	= albums->imagesList();

			if(imagesList)
			{
				for(int y = 0;y < imagesList->count();y++)
				{
					cImages*	images	= albums->imagesList()->at(y);

					if(images)
					{
						QStandardItem*	item	= images->item();
						if(item)
						{
							if(item->checkState() == Qt::Checked)
							{
								hasExport	= true;
								goto done;
							}
						}
					}
				}
			}
		}
	}

done:
	if(!hasExport)
	{
		QMessageBox::information(this, "Export", tr("No files selected for export."));
		return;
	}

	cExportDialog	exportDialog(m_imageFormats, this);
	if(exportDialog.exec() == QDialog::Rejected)
		return;

	m_exportInclude.clear();
	m_exportExclude.clear();

	for(int i = 0;i < m_tagsList->count();i++)
	{
		cTags*			tags		= m_tagsList->at(i);
		QStandardItem*	included	= tags->includeItem();
		QStandardItem*	excluded	= tags->excludeItem();

		if(included)
		{
			if(included->checkState())
				m_exportInclude.append(tags->id());
		}

		if(excluded)
		{
			if(excluded->checkState())
				m_exportExclude.append(tags->id());
		}
	}

	m_working	= true;
	m_stopIt	= false;
//	setActionEnabled(false, false, false, false, false, true);
	doExport();
//	setActionEnabled(true, true, true, true, true, false);
	m_working	= false;
}

void cMainWindow::onStop()
{
	m_stopIt	= true;
}

void cMainWindow::doExport()
{
	EXPORTSETTINGS	exportSettings;
	OVERWRITE		overwrite	= OVERWRITE_ASK;

	m_exportLog	= "<!DOCTYPE html>\n";
	m_exportLog.append("<html>\n");
	m_exportLog.append("<head>\n");
	m_exportLog.append("<style>\n");
	m_exportLog.append("body       { background-color: white; color: black; }\n");
	m_exportLog.append("h1         { color: black; }\n");
	m_exportLog.append("p          { color: black; }\n");
	m_exportLog.append(".time      { color: darkgrey; }");
	m_exportLog.append(".title     { color: darkblue; font-weight: bold; }");
	m_exportLog.append(".option    { color: darkmagenta; font-style: italic; }");
	m_exportLog.append(".optionok  { color: green; font-style: italic; }");
	m_exportLog.append(".optionnok { color: red; font-style: italic; }");
	m_exportLog.append("td         { color: black; }\n");
	m_exportLog.append("</style>\n");
	m_exportLog.append("</head>\n");
	m_exportLog.append("<body>\n");

	addToExportLog(m_exportLog, "<span class='title'>Start Export</span>");

	getExportSettings(exportSettings);

	qint32	fileCount	= 0;
	for(int x = 0;x < m_albumsList->count();x++)
	{
		if(m_stopIt)
			break;

		cAlbums*		albums	= m_albumsList->at(x);

		if(albums)
		{
			if(m_stopIt)
				break;

			cImagesList*	imagesList	= albums->imagesList();

			if(imagesList)
			{
				for(int y = 0;y < imagesList->count();y++)
				{
					if(m_stopIt)
						break;

					cImages*	images	= albums->imagesList()->at(y);

					if(images)
					{
						QStandardItem*	item	= images->item();
						if(item)
						{
							if(item->checkState() == Qt::Checked)
								fileCount++;
						}
					}
				}
			}
		}
	}

	addToExportLog(m_exportLog, QString("Files to export: <span class='option'>%1</span>").arg(fileCount));

	m_progressBar->setRange(0, fileCount);
	m_progressBar->setValue(0);
	m_progressBar->setVisible(true);

	qint32	curFile	= 0;
	for(int x = 0;x < m_albumsList->count();x++)
	{
		cAlbums*		albums	= m_albumsList->at(x);

		if(albums)
		{
			cImagesList*	imagesList	= albums->imagesList();

			if(imagesList)
			{
				for(int y = 0;y < imagesList->count();y++)
				{
					cImages*	images	= albums->imagesList()->at(y);

					if(images)
					{
						QStandardItem*	item	= images->item();
						if(item)
						{
							if(item->checkState() == Qt::Checked)
							{
								if(m_stopIt)
									break;

								cAlbums*	albums	= images->albums();
								QString		path	= albums->albumRoots()->drive() + albums->albumRoots()->specificPath() + "/" + albums->relativePath();
								QString		file	= path + "/" + images->name();

								ui->m_statusBar->showMessage(QString(tr("checking file %1...")).arg(file));
								m_progressBar->setValue(curFile);
								qApp->processEvents();

								cEXIF	exif(m_exifTAGList, m_exifCompressionList, m_exifLightSourceList, m_exifFlashList, m_iptcTagList, m_xmpTagList);
								if(exif.fromFile(file, false))
								{
									overwrite	= exportFile(exportSettings, &exif, images, overwrite);
									curFile++;
								}
							}
						}
					}
				}
			}
		}
	}

	m_progressBar->setVisible(false);
	ui->m_statusBar->showMessage(tr("export done."), 3000);

	addToExportLog(m_exportLog, "<b>done.</b>");
	m_exportLog.append("</body>\n");
	m_exportLog.append("</html>\n");

	cLogWindow	logWindow;

	logWindow.setText(m_exportLog);
	logWindow.exec();
}

void cMainWindow::getExportSettings(EXPORTSETTINGS& exportSettings)
{
	QSettings	settings;
	QString		tmp;

	tmp		= settings.value("export/directoryMethod", QVariant::fromValue(QString("keepDirectory"))).toString();

	addToExportLog(m_exportLog, "<b>Settings:</b>");

	exportSettings.directory			= settings.value("export/destinationPath", QVariant::fromValue(QString(""))).toString();
	exportSettings.keepStructure		= settings.value("export/keepStructure", QVariant::fromValue(false)).toBool();
	exportSettings.directoryTag			= settings.value("export/destinationPathTag", QVariant::fromValue(QString(""))).toString();

	if(tmp == "newDirectoryTag")
	{
		exportSettings.directoryMethod	= DIRECTORY_METHOD_TAG;
		addToExportLog(m_exportLog, "Directory Name Method: <span class='option'>rename Directory by TAG</span>");
		addToExportLog(m_exportLog, " - used TAG: <span class='option'>" + exportSettings.directoryTag + "</span>");
	}
	else if(tmp == "newDirectory")
	{
		exportSettings.directoryMethod	= DIRECTORY_METHOD_NEW;
		addToExportLog(m_exportLog, "Directory Name Method: <span class='option'>create new directory</span>");
		addToExportLog(m_exportLog, " - used Directory: <span class='option'>" + exportSettings.directory + "</span>");
	}
	else
	{
		exportSettings.directoryMethod	= DIRECTORY_METHOD_KEEP;
		addToExportLog(m_exportLog, "Directory Name Method: <span class='option'>keep directory</span>");
	}

	addToExportLog(m_exportLog, QString("Keep old Structure: <span class='option'>%1</span>").arg(exportSettings.keepStructure ? "yes" : "no"));

	bool	stripLastFolder		= settings.value("export/stripLastDirectory", QVariant::fromValue(false)).toBool();
	bool	stripLastFolderIf	= settings.value("export/stripLastDirectoryIf", QVariant::fromValue(false)).toBool();

	if(stripLastFolder)
	{
		if(stripLastFolderIf)
		{
			exportSettings.stripLastFolderMethod	= STRIP_LAST_FOLDER_METHOD::STRIP_LAST_FOLDER_IF;
			exportSettings.stripLastFolderIfList	= settings.value("export/stripLastDirectoryIfList", QVariant::fromValue(QString(""))).toString().split("\n");
			addToExportLog(m_exportLog, "Strip last folder: <span class='option'>yes</span>");
			addToExportLog(m_exportLog, QString(" - folders to strip: <span class='option'>%1</span>").arg(exportSettings.stripLastFolderIfList.join(", ")));
		}
		else
		{
			exportSettings.stripLastFolderMethod	= STRIP_LAST_FOLDER_METHOD::STRIP_LAST_FOLDER;
			addToExportLog(m_exportLog, "Strip last folder: <span class='option'>yes</span>");
		}
	}
	else
	{
		exportSettings.stripLastFolderMethod	= STRIP_LAST_FOLDER_METHOD::STRIP_LAST_FOLDER_NO;
		addToExportLog(m_exportLog, "Strip last folder: <span class='option'>no</span>");
	}

	tmp		= settings.value("export/filenamePlus", QVariant::fromValue(QString("converted"))).toString();

	if(tmp == "TAG")
		exportSettings.fileAdd			= FILE_ADD_TAG;
	else
		exportSettings.fileAdd			= FILE_ADD_CONVERTED;

	exportSettings.fileTag				= settings.value("export/fileTag", QVariant::fromValue(QString(""))).toString();

	tmp		= settings.value("export/fileMethod", QVariant::fromValue(QString("keepFilename"))).toString();

	if(tmp == "newFilename")
	{
		exportSettings.fileMethod		= FILE_METHOD_RENAME;
		addToExportLog(m_exportLog, "File Name Method: <span class='option'>FILE_METHOD_RENAME</span>");

		if(exportSettings.fileAdd == FILE_ADD_TAG)
			addToExportLog(m_exportLog, " - used TAG: <span class='option'>" + exportSettings.fileTag + "</span>");
		else
			addToExportLog(m_exportLog, " - add <span class='option'>'_converted'</span>");
	}
	else
	{
		exportSettings.fileMethod		= FILE_METHOD_KEEP;
		addToExportLog(m_exportLog, "File Name Method: <span class='option'>FILE_METHOD_KEEP</span>");
	}


	tmp		= settings.value("export/overwrite", QVariant::fromValue(QString("ask"))).toString();

	if(tmp == "no")
	{
		exportSettings.overwriteMethod	= OVERWRITE_METHOD_NOEXPORT;
		addToExportLog(m_exportLog, "Existing File Overwrite Mode: <span class='option'>do not export existing file</span>");
	}
	else if(tmp == "overwrite")
	{
		exportSettings.overwriteMethod	= OVERWRITE_METHOD_OVERWRITE;
		addToExportLog(m_exportLog, "Existing File Overwrite Mode: <span class='option'>overwrite existing file</span>");
	}
	else if(tmp == "rename")
	{
		exportSettings.overwriteMethod	= OVERWRITE_METHOD_RENAME;
		addToExportLog(m_exportLog, "Existing File Overwrite Mode: <span class='option'>rename existing file</span>");
	}
	else
	{
		exportSettings.overwriteMethod	= OVERWRITE_METHOD_ASK;
		addToExportLog(m_exportLog, "Existing File Overwrite Mode: <span class='option'>ask for overwrite</span>");
	}

	exportSettings.copyEXIF				= settings.value("export/copyEXIF", QVariant::fromValue(true)).toBool();
	exportSettings.fileFormat			= settings.value("export/fileFormat").toString();
	exportSettings.quality				= settings.value("export/quality", QVariant::fromValue(50)).toInt();

	addToExportLog(m_exportLog, "File Format: <span class='option'>" + exportSettings.fileFormat + "</span>");
	addToExportLog(m_exportLog, "Default Output Quality: <span class='option'>" + QString::number(exportSettings.quality) + "</span>");
}

OVERWRITE cMainWindow::exportFile(const EXPORTSETTINGS& exportSettings, cEXIF* lpExif, cImages* images, OVERWRITE overwrite)
{
	addToExportLog(m_exportLog, "Loading file: <span class='option'>" + lpExif->fileName() + "</span>");

	if(m_exportExclude.count() || m_exportInclude.count())
	{
		QSqlQuery		query(m_dbDigikam);
		QString			sql			= QString("SELECT	tagId "
											  "FROM		ImageTags "
											  "WHERE	imageId=%1;").arg(images->id());

		bool			doExport	= false;
		QList<qint32>	idList;

		if(query.exec(sql))
		{
			while(query.next())
				idList.append(query.value("tagId").toInt());
		}

		if(!m_exportInclude.count())
			doExport	= true;
		else
		{
			for(int i = 0;i < idList.count();i++)
			{
				if(m_exportInclude.contains(idList.at(i)))
				{
					doExport	= true;
					break;
				}
			}
		}

		if(doExport)
		{
			for(int i = 0;i < idList.count();i++)
			{
				if(m_exportExclude.contains(idList.at(i)))
				{
					doExport	= false;
					break;
				}
			}
		}

		if(!doExport)
		{
			addToExportLog(m_exportLog, "Skipped due to exclude/include list");
			return(overwrite);
		}
	}

	QString		destPath;
	QString		destFile;
	QFileInfo	fileInfo(lpExif->fileName());
	QString		extension	= exportSettings.fileFormat;
	bool		isMovie		= false;

	extension	= extension.mid(extension.lastIndexOf(".")+1);
	extension	= extension.left(extension.length()-1);

	if(!images->videoMetadata().videoCodec.isEmpty())
	{
		extension	= lpExif->fileName().mid(lpExif->fileName().lastIndexOf(".")+1);
		isMovie	= true;
	}

	switch(exportSettings.directoryMethod)
	{
	case DIRECTORY_METHOD_NEW:
		destPath	= exportSettings.directory;
		if(exportSettings.keepStructure)
		{
			destPath.append(images->albums()->relativePath());

			if(exportSettings.stripLastFolderMethod != STRIP_LAST_FOLDER_NO)
			{
				bool	strip		= true;

				if(exportSettings.stripLastFolderMethod == STRIP_LAST_FOLDER_IF)
				{
					QString	lastFolder	= destPath.mid(destPath.lastIndexOf("/")+1);
					if(!exportSettings.stripLastFolderIfList.contains(lastFolder, Qt::CaseInsensitive))
						strip	= false;
				}

				if(strip)
					destPath	= destPath.left(destPath.lastIndexOf("/"));
			}
		}
		break;
	case DIRECTORY_METHOD_KEEP:
		destPath	= fileInfo.absolutePath();
		break;
	case DIRECTORY_METHOD_TAG:
		destPath	= replaceTags(exportSettings.directoryTag, lpExif, extension);
		break;
	}

	switch(exportSettings.fileMethod)
	{
	case FILE_METHOD_KEEP:
		destFile	= fileInfo.completeBaseName() + "." + extension;
		break;
	case FILE_METHOD_RENAME:
		switch(exportSettings.fileAdd)
		{
		case FILE_ADD_CONVERTED:
			destFile	= fileInfo.completeBaseName() + "_converted" + "." + extension;
			break;
		case FILE_ADD_TAG:
			destFile	= replaceTags(exportSettings.fileTag, lpExif, extension, false) + "." + extension;
			break;
		}
		break;
	}

	destFile.prepend(destPath + "/");
	QFileInfo	destInfo(destFile);

	addToExportLog(m_exportLog, "Destination: <span class='option'>" + destFile + "</span>");

	if(destInfo.exists() && overwrite != OVERWRITE_ALL)
	{
		switch(exportSettings.overwriteMethod)
		{
		case OVERWRITE_METHOD_ASK:
			{
				QMessageBox::StandardButton	ret	= QMessageBox::question(this, tr("File Exists"), QString(tr("File %1 exists. Do you want to overwrite?")).arg(destFile), QMessageBox::Yes | QMessageBox::No | QMessageBox::YesAll | QMessageBox::NoAll);
				if(ret == QMessageBox::YesAll)
				{
					overwrite	= OVERWRITE_ALL;
					addToExportLog(m_exportLog, "New Option: <span class='optionok'>overwrite all</span>");
				}
				else if(ret == QMessageBox::NoAll)
				{
					overwrite	= OVERWRITE_NONE;
					addToExportLog(m_exportLog, "New Option: <span class='optionnok'>overwrite none</span>");
					addToExportLog(m_exportLog, "<span class='optionnok'>aborting, destination file exists</span>");
					return(overwrite);
				}
				else if(ret == QMessageBox::No)
				{
					addToExportLog(m_exportLog, "<span class='optionnok'>aborting, destination file exists</span>");
					return(overwrite);
				}
				else
					addToExportLog(m_exportLog, "<span class='optionok'>file exists, overwrite</span>");
			}
			break;
		case OVERWRITE_METHOD_RENAME:
			destFile	= findFreeFileName(destFile);
			if(destFile.isEmpty())
			{
				QMessageBox::information(this, tr("File Export"), QString("%1 cannot be renamed.").arg(lpExif->fileName()));
				addToExportLog(m_exportLog, "<span class='optionnok'>aborting, file cannot be renamed</span>");
				return(overwrite);
			}
			addToExportLog(m_exportLog, "renaming to <span class='optionok'>" + destFile + "</span>");
			break;
		case OVERWRITE_METHOD_OVERWRITE:
			break;
		case OVERWRITE_METHOD_NOEXPORT:
			addToExportLog(m_exportLog, "<span class='optionnok'>aborting, destination file exists</span>");
			return(overwrite);
			break;
		}
	}
	else if(destInfo.exists() && (overwrite == OVERWRITE_NONE || exportSettings.overwriteMethod == OVERWRITE_METHOD_NOEXPORT))
	{
		addToExportLog(m_exportLog, "<span class='optionnok'>aborting, destination file exists</span>");
		return(overwrite);
	}

	ui->m_statusBar->showMessage(QString(tr("loading %1...")).arg(lpExif->fileName()));
	qApp->processEvents();

	if(!isMovie)
	{
		cImage			image(lpExif->fileName());
		if(!image.isNull())
		{
			if(!image.isRaw() || !fileInfo.suffix().compare("cr3", Qt::CaseInsensitive))
			{
				QTransform	rotation;
				int			angle	= 0;

				switch(images->imageInformation().orientation)
				{
				case 8:
					angle	= 270;
					break;
				case 3:
					angle	= 180;
					break;
				case 6:
					angle	=  90;
					break;
				}

				if(angle != 0)
				{
					rotation.rotate(angle);
					image	= image.transformed(rotation);
				}
			}

			addToExportLog(m_exportLog, "Loading file: <span class='optionok'>successful</span>");

			QFileInfo	info(destFile);

			ui->m_statusBar->showMessage(QString(tr("converting to %1...")).arg(destFile));
			qApp->processEvents();

			QDir		dir;

			if(dir.mkpath(info.absolutePath()))
			{
				QImageWriter	writer(destFile);

				addToExportLog(m_exportLog, "Writing file: <span class='option'>" + destFile + "</span>");
				writer.setQuality(exportSettings.quality);
				if(writer.write(image))
				{
					addToExportLog(m_exportLog, "Writing file: <span class='optionok'>successful</span>");

					if(exportSettings.copyEXIF)
					{
						if(lpExif->copyTo(destFile))
							addToExportLog(m_exportLog, "Copy EXIF: <span class='optionok'>successful</span>");
						else
							addToExportLog(m_exportLog, "Copy EXIF: <span class='optionnok'>error</span>");
					}
				}
				else
					addToExportLog(m_exportLog, "Writing file: <span class='optionnok'>error: " + writer.errorString() + "</span>");
			}
		}
	}
	else
	{
		addToExportLog(m_exportLog, "Loading file: <span class='optionok'>no image</span>");

		QFileInfo	info(destFile);

		ui->m_statusBar->showMessage(QString(tr("copying to %1...")).arg(destFile));
		qApp->processEvents();

		QDir		dir;

		if(dir.mkpath(info.absolutePath()))
		{
			if(QFile::copy(lpExif->fileName(), destFile))
				addToExportLog(m_exportLog, "Writing file: <span class='optionok'>successful</span>");
			else
				addToExportLog(m_exportLog, "Copy EXIF: <span class='optionnok'>error</span>");
		}
	}

	return(overwrite);
}

QString cMainWindow::replaceTags(const QString& path, cEXIF* lpExif, const QString& extension, bool directory)
{
	QString		dest	= path;
	QFileInfo	fileInfo(lpExif->fileName());
	QString		model	= lpExif->cameraModel().replace("/", "_").replace("\\", "_").replace(":", "_");
	QString		maker	= lpExif->cameraMake().replace("/", "_").replace("\\", "_").replace(":", "_");

	if(model.isEmpty())
		model	= "UNKNOWN";

	if(maker.isEmpty())
		maker	= "UNKNOWN";

	if(directory)
		dest	= dest.replace("%o", fileInfo.absolutePath());
	else
		dest	= dest.replace("%o", fileInfo.fileName());

	dest		= dest.replace("%y", lpExif->dateTime().toString("yyyy"));
	dest		= dest.replace("%m", lpExif->dateTime().toString("MM"));
	dest		= dest.replace("%d", lpExif->dateTime().toString("dd"));
	dest		= dest.replace("%H", lpExif->dateTime().toString("hh"));
	dest		= dest.replace("%M", lpExif->dateTime().toString("mm"));
	dest		= dest.replace("%S", lpExif->dateTime().toString("ss"));
	dest		= dest.replace("%c", maker);
	dest		= dest.replace("%l", model);
	dest		= dest.replace("%t", extension);

	dest		= dest.replace("\\", "/");

	return(dest);
}

QString cMainWindow::findFreeFileName(const QString& fileName)
{
	QFileInfo	fileInfo(fileName);
	QString		newName;
	int			number	= 1;

	for(;;)
	{
		newName	= fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + "-" + QStringLiteral("%1").arg(number, 3, 10, QLatin1Char('0')) + "." + fileInfo.suffix();

		if(!QFileInfo::exists(newName))
			return(newName);

		number++;
	}
}
