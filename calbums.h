#ifndef CALBUMS_H
#define CALBUMS_H


#include "calbumroots.h"
#include "cimages.h"

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QList>
#include <QStandardItem>

#include <QObject>

#include <QMetaType>


class cAlbums : public QObject
{
	Q_OBJECT

public:
	cAlbums(const qint32& id, cAlbumsList* albumsList, QSqlDatabase* dbDigikam, QSqlDatabase* m_dbThumbnail, QObject *parent = nullptr);
	cAlbums(const qint32& id, cAlbumRoots* albumRoots, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, cAlbumsList* albumsList, QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, QObject *parent = nullptr);

	qint32			id();

	void			setAlbumRoots(cAlbumRoots* albumRoots);
	cAlbumRoots*	albumRoots();

	void			setRelativePath(const QString& relativePath);
	QString			relativePath();

	QString			path();

	void			setDate(const QDate& date);
	QDate			date();

	void			setCaption(const QString& caption);
	QString			caption();

	void			setCollection(const QString& collection);
	QString			collection();

	void			setModificationDate(const QDateTime modificationDate);
	QDateTime		modificationDate();

	void			setParentAlbums(cAlbums* root);
	cAlbums*		parentAlbums();

	void			loadImages();
	cImagesList*	imagesList();

	void			setItem(QStandardItem* item);
	QStandardItem*	item();

private:
	qint32			m_id;
	cAlbumRoots*	m_albumRoots;
	QString			m_relativePath;
	QDate			m_date;
	QString			m_caption;
	QString			m_collection;
	QDateTime		m_modificationDate;
	cAlbums*		m_parentAlbums;
	cImagesList*	m_imagesList;
	cAlbumsList*	m_albumsList;
	QSqlDatabase*	m_dbDigikam;
	QSqlDatabase*	m_dbThumbnail;
	QStandardItem*	m_item;
};

Q_DECLARE_METATYPE(cAlbums*)

class cAlbumsList : public QObject, public QList<cAlbums*>
{
	Q_OBJECT

public:
	cAlbumsList(QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, cAlbumRootsList* albumRootsList, QObject* parent = nullptr);

	bool				load();
	cAlbums*			add(const qint32& id, QObject* parent = nullptr);
	cAlbums*			add(const qint32& id, cAlbumRoots* albumRoots, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, QObject *parent = nullptr);
	cAlbums*			add(const qint32& id, const qint32& albumRootsID, const QString& relativePath, const QDate& date, const QString& caption, const QString& collection, const QDateTime& modificationDate, QObject *parent = nullptr);
	cAlbums*			find(const qint32& id);
	cAlbums*			find(const QString& relativePath);

private:
	QSqlDatabase*		m_dbDigikam;
	QSqlDatabase*		m_dbThumbnail;
	cAlbumRootsList*	m_albumRootsList;
};

Q_DECLARE_METATYPE(cAlbumsList*)

#endif // CALBUMS_H
