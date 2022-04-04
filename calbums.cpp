#include "calbums.h"

#include <QSqlQuery>

#include <QDebug>


cAlbums::cAlbums(const qint32& id, cAlbumsList* albumsList, QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albumRoots(nullptr),
	m_relativePath(""),
	m_date(QDate()),
	m_caption(""),
	m_collection(""),
	m_modificationDate(QDateTime()),
	m_parentAlbums(nullptr),
	m_imagesList(nullptr),
	m_albumsList(albumsList),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail),
	m_item(nullptr)
{
}

cAlbums::cAlbums(const qint32& id, cAlbumRoots* albumRoots, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, cAlbumsList* albumsList, QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albumRoots(albumRoots),
	m_relativePath(relativePath),
	m_date(date),
	m_caption(caption),
	m_collection(collection),
	m_modificationDate(modificationDate),
	m_parentAlbums(nullptr),
	m_imagesList(nullptr),
	m_albumsList(albumsList),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail),
	m_item(nullptr)
{
}

qint32 cAlbums::id()
{
	return(m_id);
}

void cAlbums::setAlbumRoots(cAlbumRoots* albumRoots)
{
	m_albumRoots	= albumRoots;
}

cAlbumRoots* cAlbums::albumRoots()
{
	return(m_albumRoots);
}

void cAlbums::setRelativePath(const QString& relativePath)
{
	m_relativePath	= relativePath;
}

QString cAlbums::relativePath()
{
	return(m_relativePath);
}

QString cAlbums::path()
{
	QString	path	= m_relativePath.mid(m_relativePath.lastIndexOf("/")+1);
	return(path);
}

void cAlbums::setDate(const QDate& date)
{
	m_date	= date;
}

QDate cAlbums::date()
{
	return(m_date);
}

void cAlbums::setCaption(const QString& caption)
{
	m_caption	= caption;
}

QString cAlbums::caption()
{
	return(m_caption);
}

void cAlbums::setCollection(const QString& collection)
{
	m_collection	 = collection;
}

QString cAlbums::collection()
{
	return(m_collection);
}

void cAlbums::setModificationDate(const QDateTime modificationDate)
{
	m_modificationDate	= modificationDate;
}

QDateTime cAlbums::modificationDate()
{
	return(m_modificationDate);
}

void cAlbums::setParentAlbums(cAlbums *parentAlbums)
{
	m_parentAlbums	= parentAlbums;
}

cAlbums* cAlbums::parentAlbums()
{
	return(m_parentAlbums);
}

bool cAlbums::loadImages(bool loadThumbnails)
{
	if(!m_imagesList)
	{
		m_imagesList	= new cImagesList(m_dbDigikam, m_dbThumbnail, m_albumsList, this);
		m_imagesList->load(loadThumbnails, this);
		return(true);
	}
	else
		m_imagesList->loadThumbnails();

	return(false);
}

cImagesList* cAlbums::imagesList()
{
	return(m_imagesList);
}

void cAlbums::setItem(QStandardItem* item)
{
	m_item	= item;
}

QStandardItem* cAlbums::item()
{
	return(m_item);
}

cAlbumsList::cAlbumsList(QSqlDatabase* dbDigikam, QSqlDatabase *dbThumbnail, cAlbumRootsList* albumRootsList, QObject* parent) :
	QObject(parent),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail),
	m_albumRootsList(albumRootsList)
{
}

bool cAlbumsList::load()
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);

	if(!query.exec("SELECT		id, "
				   "			albumRoot, "
				   "			relativePath, "
				   "			date, "
				   "			caption, "
				   "			collection, "
				   "			modificationDate "
				   "FROM		Albums "
				   "ORDER BY	relativePath;"))
	{
		qDebug() << "cAlbumsList: loading failed.";
		return(false);
	}

	while(query.next())
		add(query.value("id").toInt(), query.value("albumRoot").toInt(), query.value("relativePath").toString(), query.value("date").toDate(), query.value("caption").toString(), query.value("collection").toString(), query.value("modificationDate").toDateTime(), this);

	return(true);
}

cAlbums* cAlbumsList::add(const qint32& id, QObject* parent)
{
	cAlbums*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cAlbums(id, this, m_dbDigikam, m_dbThumbnail, parent);
	lpNew->loadImages();

	append(lpNew);
	return(lpNew);
}

cAlbums* cAlbumsList::add(const qint32& id, cAlbumRoots* albumRoots, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, QObject *parent)
{
	cAlbums*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cAlbums(id, albumRoots, relativePath, date, caption, collection, modificationDate, this, m_dbDigikam, m_dbThumbnail, parent);
	lpNew->loadImages();

	QString	parentAlbum	= relativePath.left(relativePath.lastIndexOf("/"));
	if(!parentAlbum.isEmpty())
		lpNew->setParentAlbums(find(parentAlbum));

	append(lpNew);
	return(lpNew);
}

cAlbums* cAlbumsList::add(const qint32& id, const qint32& albumRootsID, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, QObject *parent)
{
	cAlbums*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	cAlbumRoots*	albumRoots = m_albumRootsList->find(albumRootsID);
	lpNew	= new cAlbums(id, albumRoots, relativePath, date, caption, collection, modificationDate, this, m_dbDigikam, m_dbThumbnail, parent);
	lpNew->loadImages(false);

	QString	parentAlbum = relativePath.left(relativePath.lastIndexOf("/"));

	if(!parentAlbum.isEmpty())
		lpNew->setParentAlbums(find(parentAlbum));
	else if(relativePath.length() > 1)
		lpNew->setParentAlbums(at(0));

	append(lpNew);
	return(lpNew);
}

cAlbums* cAlbumsList::find(const qint32& id)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->id() == id)
			return(at(i));
	}
	return(nullptr);
}

cAlbums* cAlbumsList::find(const QString& relativePath)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->relativePath() == relativePath)
			return(at(i));
	}
	return(nullptr);
}
