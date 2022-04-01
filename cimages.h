#ifndef CIMAGES_H
#define CIMAGES_H


#include "cimage.h"

#include <QString>
#include <QDateTime>
#include <QList>
#include <QStandardItem>

#include <QSqlDatabase>

#include <QObject>

#include <QMetaType>


class cAlbums;
class cAlbumsList;

class cImages : public QObject
{
	Q_OBJECT
public:
	cImages(const qint32& id, QObject *parent = nullptr);
	cImages(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QSqlDatabase* dbThumbnail, QObject *parent = nullptr);

	qint32			id();

	void			setAlbums(cAlbums* albums);
	cAlbums*		albums();

	void			setName(const QString& name);
	QString			name();

	void			setStatus(qint8& status);
	qint8			status();

	void			setCategory(qint8& category);
	qint8			category();

	void			setModificationDate(const QDateTime modificationDate);
	QDateTime		modificationDate();

	void			setFileSize(qint32& fileSize);
	qint32			fileSize();

	void			setUniqueHash(const QString& uniqueHash);
	QString			uniqueHash();

	void			setManualOrder(qint32& manualOrder);
	qint32			manualOrder();

	void			setItem(QStandardItem* item);
	QStandardItem*	item();

	void			loadThumbnail();
	QImage*			thumbnail();

private:
	qint32			m_id;
	cAlbums*		m_albums;
	QString			m_name;
	qint8			m_status;
	qint8			m_category;
	QDateTime		m_modificationDate;
	qint32			m_fileSize;
	QString			m_uniqueHash;
	qint32			m_manualOrder;
	QSqlDatabase*	m_dbThumbnail;
	QStandardItem*	m_item;
	cImage*			m_thumbnail;

signals:

};

Q_DECLARE_METATYPE(cImages*)

class cImagesList : public QObject, public QList<cImages*>
{
	Q_OBJECT

public:
	cImagesList(QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, cAlbumsList* albumsList, QObject* parent = nullptr);

	bool			load(cAlbums* albums);
	bool			load(const qint32& albumsID);
	cImages*		add(const qint32& id, QObject* parent = nullptr);
	cImages*		add(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QObject *parent = nullptr);
	cImages*		add(const qint32& id, const qint32& albumsID, const QString& name, const qint8& status, const qint8& category, const QDateTime &modificationDate, const qint32 &fileSize, const QString& uniqueHash, const qint32 &manualOrder, QObject *parent = nullptr);
	cImages*		find(const qint32& id);

private:
	QSqlDatabase*	m_dbDigikam;
	QSqlDatabase*	m_dbThumbnail;
	cAlbumsList*	m_albumsList;
};

#endif // CIMAGES_H
