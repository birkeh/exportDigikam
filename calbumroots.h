#ifndef CALBUMROOTS_H
#define CALBUMROOTS_H


#include "common.h"

#include <QString>
#include <QList>
#include <QStandardItem>

#include <QSqlDatabase>

#include <QObject>

#include <QMetaType>


class cAlbumRoots : public QObject
{
	Q_OBJECT

public:
	cAlbumRoots(const qint32& id, QObject* parent = nullptr);
	cAlbumRoots(const qint32& id, const QString& label, const quint32 status, const qint32 type, const QString& identifier, const QString& specificPath, QObject* parent = nullptr);
	~cAlbumRoots();

	qint32			id();

	void			setLabel(const QString& label);
	QString			label();

	void			setStatus(const qint32& status);
	qint32			status();

	void			setType(const qint32& type);
	qint32			type();

	void			setIdentifier(const QString& identifier);
	QString			identifier();

	void			setSpecificPath(const QString &specificPath);
	QString			specificPath();

	void			setItem(QStandardItem* item);
	QStandardItem*	item();

	void			setDrive(const QString &drive);
	QString			drive();

private:
	qint32			m_id;
	QString			m_label;
	quint32			m_status;
	qint32			m_type;
	QString			m_identifier;
	QString			m_specificPath;
	QStandardItem*	m_item;
	QString			m_drive;
};

Q_DECLARE_METATYPE(cAlbumRoots*)

class cAlbumRootsList : public QObject, public QList<cAlbumRoots*>
{
	Q_OBJECT

public:
	cAlbumRootsList(QSqlDatabase* dbDigikam, QSqlDatabase* dbThumbnail, QObject* parent = nullptr);
	~cAlbumRootsList();

	bool					load(QList<SolidVolumeInfo>* volumes);
	cAlbumRoots*			add(const qint32 &id, QObject* parent = nullptr);
	cAlbumRoots*			add(const qint32& id, const QString& label, const quint32 status, const qint32 type, const QString& identifier, const QString& specificPath, QObject* parent = nullptr);
	cAlbumRoots*			find(const qint32& id);

private:
	QSqlDatabase*			m_dbDigikam;
	QSqlDatabase*			m_dbThumbnail;
};

Q_DECLARE_METATYPE(cAlbumRootsList*)

#endif // CALBUMROOTS_H
