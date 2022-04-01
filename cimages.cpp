#include "cimages.h"
#include "calbums.h"

#include <QPainter>
#include <QSqlQuery>

#define THUMBNAIL_WIDTH		160
#define THUMBNAIL_HEIGHT	120


cImages::cImages(const qint32& id, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albums(nullptr),
	m_name(""),
	m_status(0),
	m_category(0),
	m_modificationDate(QDateTime()),
	m_fileSize(0),
	m_uniqueHash(""),
	m_manualOrder(0),
	m_item(nullptr),
	m_thumbnail(nullptr)
{
}

cImages::cImages(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QSqlDatabase *dbThumbnail, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albums(albums),
	m_name(name),
	m_status(status),
	m_category(category),
	m_modificationDate(modificationDate),
	m_fileSize(fileSize),
	m_uniqueHash(uniqueHash),
	m_manualOrder(manualOrder),
	m_dbThumbnail(dbThumbnail),
	m_item(nullptr),
	m_thumbnail(nullptr)
{
}

qint32 cImages::id()
{
	return(m_id);
}

void cImages::setAlbums(cAlbums* albums)
{
	m_albums	= albums;
}

cAlbums* cImages::albums()
{
	return(m_albums);
}

void cImages::setName(const QString& name)
{
	m_name	= name;
}

QString cImages::name()
{
	return(m_name);
}

void cImages::setStatus(qint8& status)
{
	m_status	= status;
}

qint8 cImages::status()
{
	return(m_status);
}

void cImages::setCategory(qint8& category)
{
	m_category	= category;
}

qint8 cImages::category()
{
	return(m_category);
}

void cImages::setModificationDate(const QDateTime modificationDate)
{
	m_modificationDate	= modificationDate;
}

QDateTime cImages::modificationDate()
{
	return(m_modificationDate);
}

void cImages::setFileSize(qint32& fileSize)
{
	m_fileSize	= fileSize;
}

qint32 cImages::fileSize()
{
	return(m_fileSize);
}

void cImages::setUniqueHash(const QString& uniqueHash)
{
	m_uniqueHash	= uniqueHash;
}

QString cImages::uniqueHash()
{
	return(m_uniqueHash);
}

void cImages::setManualOrder(qint32& manualOrder)
{
	m_manualOrder	= manualOrder;
}

qint32 cImages::manualOrder()
{
	return(m_manualOrder);
}

void cImages::setItem(QStandardItem* item)
{
	m_item	= item;
}

QStandardItem* cImages::item()
{
	return(m_item);
}

void cImages::loadThumbnail()
{
	if(!m_dbThumbnail)
		return;

	QSqlQuery	query(*m_dbThumbnail);
	QString		sql	= QString("SELECT		t.id id, "
							  "				t.type type, "
							  "				t.modificationDate modificationDate, "
							  "				t.orientationHint orientationHint, "
							  "				t.data data "
							  "FROM			Thumbnails t "
							  "JOIN			UniqueHashes h ON (t.id = h.thumbId) "
							  "WHERE		h.uniqueHash = '%1';").arg(m_uniqueHash);

	if(!query.exec(sql))
	{
		qDebug() << "cImages: Thumbnail Loading failed.";
		return;
	}

	if(m_thumbnail)
		delete m_thumbnail;

	cImage	thumb;

	if(query.first())
	{
		if(!query.value("data").isNull())
			thumb.loadPGF(query.value("data").toByteArray());

		if(thumb.isNull())
		{
			m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, QImage::Format_RGB16);

			QPainter	painter(m_thumbnail);
			painter.setPen(QPen(Qt::black));
			painter.setBrush(QBrush(Qt::white));
			painter.drawRect(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
			painter.drawLine(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
			painter.drawLine(0, THUMBNAIL_HEIGHT-1, THUMBNAIL_WIDTH-1, 0);
			painter.end();
		}
		else if(thumb.width() != THUMBNAIL_WIDTH || thumb.height() != THUMBNAIL_HEIGHT)
		{
			m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, thumb.format());
			m_thumbnail->fill(Qt::black);

			qreal	sw	= THUMBNAIL_WIDTH/(qreal)thumb.width();
			qreal	sh	= THUMBNAIL_HEIGHT/(qreal)thumb.height();

			qreal	w;
			qreal	h;

			if(sw < sh)
			{
				w	= (qint32)((qreal)thumb.width()*sw);
				h	= (qint32)((qreal)thumb.height()*sw);
			}
			else
			{
				w	= (qint32)((qreal)thumb.width()*sh);
				h	= (qint32)((qreal)thumb.height()*sh);
			}

			QPainter	painter(m_thumbnail);
			painter.drawImage((THUMBNAIL_WIDTH-w)/2, (THUMBNAIL_HEIGHT-h)/2, thumb.scaled(w, h, Qt::KeepAspectRatio));
			painter.end();
		}
		else
		{
			m_thumbnail		= new cImage;
			*m_thumbnail	= thumb;
		}
	}
	else
	{
		m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, QImage::Format_RGB16);

		QPainter	painter(m_thumbnail);
		painter.setPen(QPen(Qt::black));
		painter.setBrush(QBrush(Qt::white));
		painter.drawRect(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
		painter.drawLine(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
		painter.drawLine(0, THUMBNAIL_HEIGHT-1, THUMBNAIL_WIDTH-1, 0);
		painter.end();
	}
}

QImage* cImages::thumbnail()
{
	return(m_thumbnail);
}

cImagesList::cImagesList(QSqlDatabase* dbDigikam, QSqlDatabase *dbThumbnail, cAlbumsList* albumsList, QObject* parent) :
	QObject(parent),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail),
	m_albumsList(albumsList)
{
}

bool cImagesList::load(cAlbums* albums)
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);
	QString		sql	= QString("SELECT	id, "
							  "			album, "
							  "			name, "
							  "			status, "
							  "			category, "
							  "			modificationDate, "
							  "			fileSize, "
							  "			uniqueHash, "
							  "			manualOrder "
							  "FROM		Images "
							  "WHERE	album = %1 "
							  "ORDER BY	name;").arg(albums->id());
	if(!query.exec(sql))
	{
		qDebug() << "Images: loading failed.";
		return(false);
	}

	while(query.next())
		add(query.value("id").toInt(), albums, query.value("name").toString(), query.value("status").toInt(), query.value("category").toInt(), query.value("modificationDate").toDateTime(), query.value("fileSize").toInt(), query.value("uniqueHash").toString(), query.value("manualOrder").toInt(), this);

	return(true);
}

bool cImagesList::load(const qint32 &albumsID)
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);
	QString		sql	= QString("SELECT	id, "
							  "			album, "
							  "			name, "
							  "			status, "
							  "			category, "
							  "			modificationDate, "
							  "			fileSize, "
							  "			uniqueHash, "
							  "			manualOrder "
							  "FROM		Images "
							  "WHERE	album = %1 "
							  "ORDER BY	name;").arg(albumsID);
	if(!query.exec(sql))
	{
		qDebug() << "Images: loading failed.";
		return(false);
	}

	while(query.next())
		add(query.value("id").toInt(), query.value("album").toInt(), query.value("name").toString(), query.value("status").toInt(), query.value("category").toInt(), query.value("modificationDate").toDateTime(), query.value("fileSize").toInt(), query.value("uniqueHash").toString(), query.value("manualOrder").toInt(), this);

	return(true);
}

cImages* cImagesList::add(const qint32& id, QObject* parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cImages(id, parent);

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::add(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QObject *parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cImages(id, albums, name, status, category, modificationDate, fileSize, uniqueHash, manualOrder, m_dbThumbnail, parent);
	lpNew->loadThumbnail();

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::add(const qint32& id, const qint32& albumsID, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QObject *parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	cAlbums*	albums	= m_albumsList->find(albumsID);
	lpNew	= new cImages(id, albums, name, status, category, modificationDate, fileSize, uniqueHash, manualOrder, m_dbThumbnail, parent);
	lpNew->loadThumbnail();

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::find(const qint32& id)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->id() == id)
			return(at(i));
	}
	return(nullptr);
}
