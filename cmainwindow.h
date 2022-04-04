#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "common.h"

#include "calbumroots.h"
#include "calbums.h"
#include "cfoldersortfilterproxymodel.h"
#include "cthumbnailsortfilterproxymodel.h"

#include <QMainWindow>
#include <QCloseEvent>

#include <QStandardItem>
#include <QSqlDatabase>

#include <QProgressBar>
#include <QToolBar>


QT_BEGIN_NAMESPACE
namespace Ui { class cMainWindow; }
QT_END_NAMESPACE


enum DIRECTORY_METHOD
{
	DIRECTORY_METHOD_KEEP		= 1,
	DIRECTORY_METHOD_NEW		= 2,
	DIRECTORY_METHOD_TAG		= 3,
};

enum FILE_METHOD
{
	FILE_METHOD_KEEP			= 1,
	FILE_METHOD_RENAME			= 2,
};

enum STRIP_LAST_FOLDER_METHOD
{
	STRIP_LAST_FOLDER_NO		= 1,
	STRIP_LAST_FOLDER			= 2,
	STRIP_LAST_FOLDER_IF		= 3,
};

enum OVERWRITE_METHOD
{
	OVERWRITE_METHOD_NOEXPORT	= 1,
	OVERWRITE_METHOD_ASK		= 2,
	OVERWRITE_METHOD_RENAME		= 3,
	OVERWRITE_METHOD_OVERWRITE	= 4,
};

enum FILE_ADD
{
	FILE_ADD_CONVERTED			= 1,
	FILE_ADD_TAG				= 2,
};

typedef struct tagEXPORTSETTINGS
{
	DIRECTORY_METHOD			directoryMethod;
	QString						directory;
	bool						keepStructure;
	QString						directoryTag;
	STRIP_LAST_FOLDER_METHOD	stripLastFolderMethod;
	QStringList					stripLastFolderIfList;
	FILE_METHOD					fileMethod;
	FILE_ADD					fileAdd;
	QString						fileTag;
	OVERWRITE_METHOD			overwriteMethod;
	bool						copyEXIF;
	QString						fileFormat;
	int							quality;
} EXPORTSETTINGS;

enum OVERWRITE
{
	OVERWRITE_ASK		= 0,
	OVERWRITE_NONE		= 1,
	OVERWRITE_ALL		= 2,
};


class cMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	cMainWindow(QWidget *parent = nullptr);
	~cMainWindow();

private:
	Ui::cMainWindow*				ui;

	QProgressBar*					m_progressBar;

//	QToolBar*						m_fileToolBar;
//	QAction*						m_openFileAction;
//	QAction*						m_openDirectoryAction;

	QToolBar*						m_listToolBar;
	QAction*						m_refreshAction;

	QToolBar*						m_actionToolBar;
	QAction*						m_exportAction;
	QAction*						m_stopAction;

	QSqlDatabase					m_dbDigikam;
	QSqlDatabase					m_dbThumbnail;
	cAlbumRootsList*				m_albumRootsList;
	cAlbumsList*					m_albumsList;

	QStandardItemModel*				m_folderViewModel;
	cFolderSortFilterProxyModel*	m_folderSortFilterProxyModel;
	QStandardItemModel*				m_thumbnailViewModel;
	cThumbnailSortFilterProxyModel*	m_thumbnailSortFilterProxyModel;
	QStandardItem*					m_rootItem;

	QList<IMAGEFORMAT>				m_imageFormats;

	bool							m_loading;
	bool							m_working;
	bool							m_stopIt;

	QString							m_exportLog;

	void							initUI();
	void							createActions();
	void							createMenuActions();
	void							createContextActions();
	void							setImageFormats();
	void							addImageFormat(const QString& shortName, const QString& description, const QString& extension, QList<QByteArray>& readList, QList<QByteArray>& writeList);
	void							loadData();
	void							displayData();
	void							initSignals();

	QStandardItem*					findDBRootItem(QStandardItemModel* model, QStandardItem* rootItem, cAlbumRoots* albumRoots);
	QStandardItem*					findParentItem(QStandardItemModel* model, QStandardItem* rootItem, cAlbums *albumRoots);

	void							doExport();
	void							getExportSettings(EXPORTSETTINGS& exportSettings);
protected:
	void							closeEvent(QCloseEvent* event);

private slots:
	void							onRefreshList();
	void							onExport();
	void							onStop();
	void							onFolderViewItemChanged(QStandardItem* item);
	void							onFolderSelected(const QItemSelection& selection, const QItemSelection& previous);
	void							onThumbnailSelected(const QItemSelection& selection, const QItemSelection& previous);
};
#endif // CMAINWINDOW_H
