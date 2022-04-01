#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QSqlError>

#include <QSettings>
#include <QDebug>

#include "cimage.h"


cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow),
	m_progressBar(nullptr),
	m_listToolBar(nullptr),
	m_refreshAction(nullptr),
	m_actionToolBar(nullptr),
	m_exportAction(nullptr),
	m_stopAction(nullptr),
	m_albumRootsList(nullptr),
	m_folderViewModel(nullptr),
	m_folderSortFilterProxyModel(nullptr),
	m_thumbnailViewModel(nullptr),
	m_thumbnailSortFilterProxyModel(nullptr),
	m_loading(false)
{
	initUI();
	createActions();
	loadData();
	initSignals();
}

cMainWindow::~cMainWindow()
{
	if(m_albumRootsList)
		delete m_albumRootsList;

	if(m_dbThumbnail.isOpen())
		m_dbThumbnail.close();

	if(m_dbDigikam.isOpen())
		m_dbDigikam.close();

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

	m_thumbnailViewModel	= new QStandardItemModel;
	m_thumbnailSortFilterProxyModel	= new cThumbnailSortFilterProxyModel(this);
	ui->m_thumbnailView->setModel(m_thumbnailSortFilterProxyModel);
	m_thumbnailSortFilterProxyModel->setSourceModel(m_thumbnailViewModel);

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

void cMainWindow::loadData()
{
	m_thumbnailViewModel->clear();
	m_folderViewModel->clear();

	if(m_albumRootsList)
		delete m_albumRootsList;

	if(m_albumsList)
		delete m_albumsList;

	if(!m_dbDigikam.isOpen())
	{
		m_dbDigikam		= QSqlDatabase::addDatabase("QSQLITE", "digikam4");
		m_dbDigikam.setDatabaseName("C:\\Temp\\__DIGIKAM__\\digikam4.db");
		if(!m_dbDigikam.open())
		{
			qDebug() << "Digikam: DB Open failed. " << m_dbDigikam.lastError().text();
			return;
		}
	}

	if(!m_dbThumbnail.isOpen())
	{
		m_dbThumbnail	= QSqlDatabase::addDatabase("QSQLITE", "thumbnails-digikam");
		m_dbThumbnail.setDatabaseName("C:\\Temp\\__DIGIKAM__\\thumbnails-digikam.db");
		if(!m_dbThumbnail.open())
		{
			qDebug() << "Thumbnail: DB Open failed. " << m_dbDigikam.lastError().text();
			return;
		}
	}

	m_albumRootsList	= new cAlbumRootsList(&m_dbDigikam, &m_dbThumbnail, this);
	if(!m_albumRootsList)
	{
		qDebug() << "Album Root List: out of memory.";
		return;
	}

	if(!m_albumRootsList->load())
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

	displayData();
	ui->m_folderView->expandAll();
}

void cMainWindow::displayData()
{
	m_loading	= true;

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

		lpAlbums->setItem(lpItem);
		lpRootItem->appendRow(lpItem);
	}

	m_loading	= false;
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
	connect(m_folderViewModel,					&QStandardItemModel::itemChanged,		this,	&cMainWindow::onFolderViewItemChanged);
	connect(ui->m_folderView->selectionModel(),	&QItemSelectionModel::selectionChanged,	this,	&cMainWindow::onFolderSelected);
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

void cMainWindow::onFolderSelected(const QItemSelection& /*selection*/, const QItemSelection& /*previous*/)
{
	if(m_loading)
		return;

	m_thumbnailViewModel->clear();

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

		QIcon			icon	= QIcon(QPixmap::fromImage(*images->thumbnail()));
		QStandardItem*	item	= new QStandardItem(icon, images->name());
		item->setTextAlignment(Qt::AlignCenter);
		item->setData(QVariant::fromValue(images));
		item->setCheckable(true);
		if(loaded)
			item->setCheckState(state);
		images->setItem(item);
		m_thumbnailViewModel->appendRow(item);
	}
}

void cMainWindow::onRefreshList()
{
	loadData();
}

void cMainWindow::onExport()
{
}

void cMainWindow::onStop()
{
}
