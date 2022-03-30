#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "calbumroots.h"
#include "calbums.h"
#include "cfoldersortfilterproxymodel.h"
#include "cthumbnailsortfilterproxymodel.h"

#include <QMainWindow>
#include <QCloseEvent>

#include <QStandardItem>
#include <QSqlDatabase>


QT_BEGIN_NAMESPACE
namespace Ui { class cMainWindow; }
QT_END_NAMESPACE

class cMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	cMainWindow(QWidget *parent = nullptr);
	~cMainWindow();

private:
	Ui::cMainWindow*				ui;
	QSqlDatabase					m_dbDigikam;
	QSqlDatabase					m_dbThumbnail;
	cAlbumRootsList*				m_albumRootsList;
	cAlbumsList*					m_albumsList;

	QStandardItemModel*				m_folderViewModel;
	cFolderSortFilterProxyModel*	m_folderSortFilterProxyModel;
	QStandardItemModel*				m_thumbnailViewModel;
	cThumbnailSortFilterProxyModel*	m_thumbnailSortFilterProxyModel;
	QStandardItem*					m_rootItem;

	bool							m_loading;

	void							initUI();
	void							loadData();
	void							displayData();
	void							initSignals();

	QStandardItem*					findDBRootItem(QStandardItemModel* model, QStandardItem* rootItem, cAlbumRoots* albumRoots);
	QStandardItem*					findParentItem(QStandardItemModel* model, QStandardItem* rootItem, cAlbums *albumRoots);
protected:
	void							closeEvent(QCloseEvent* event);

private slots:
	void							onFolderViewItemChanged(QStandardItem* item);
	void							onFolderSelected(const QItemSelection& selection, const QItemSelection& previous);
};
#endif // CMAINWINDOW_H
