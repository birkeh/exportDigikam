#include "calbumroots.h"

#include <QSqlQuery>

#include <QDebug>


cAlbumRoots::cAlbumRoots(const qint32& id, QObject* parent) :
	QObject(parent),
	m_id(id),
	m_label(""),
	m_status(0),
	m_type(0),
	m_identifier(""),
	m_specificPath(""),
	m_item(nullptr),
	m_drive("")
{
}

cAlbumRoots::cAlbumRoots(const qint32& id, const QString& label, const quint32 status, const qint32 type, const QString& identifier, const QString& specificPath, QObject* parent) :
	QObject(parent),
	m_id(id),
	m_label(label),
	m_status(status),
	m_type(type),
	m_identifier(identifier),
	m_specificPath(specificPath)
{
}

cAlbumRoots::~cAlbumRoots()
{
}

qint32 cAlbumRoots::id()
{
	return(m_id);
}

void cAlbumRoots::setLabel(const QString& label)
{
	m_label	= label;
}

QString cAlbumRoots::label()
{
	return(m_label);
}

void cAlbumRoots::setStatus(const qint32& status)
{
	m_status	= status;
}

qint32 cAlbumRoots::status()
{
	return(m_status);
}

void cAlbumRoots::setType(const qint32& type)
{
	m_type	= type;
}

qint32 cAlbumRoots::type()
{
	return(m_type);
}

void cAlbumRoots::setIdentifier(const QString& identifier)
{
	m_identifier	= identifier;
}

QString cAlbumRoots::identifier()
{
	return(m_identifier);
}

void cAlbumRoots::setSpecificPath(const QString& specificPath)
{
	m_specificPath	= specificPath;
}

QString cAlbumRoots::specificPath()
{
	return(m_specificPath);
}

void cAlbumRoots::setItem(QStandardItem* item)
{
	m_item	= item;
}

QStandardItem* cAlbumRoots::item()
{
	return(m_item);
}

void cAlbumRoots::setDrive(const QString& drive)
{
	m_drive	= drive;

	if(m_drive.right(1) == "/")
		m_drive	= m_drive.left(m_drive.length()-1);
}

QString cAlbumRoots::drive()
{
	return(m_drive);
}

cAlbumRootsList::cAlbumRootsList(QSqlDatabase* dbDigikam, QSqlDatabase *dbThumbnail, QObject* parent) :
	QObject(parent),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail)
{
}

cAlbumRootsList::~cAlbumRootsList()
{
}

bool cAlbumRootsList::load(QList<SolidVolumeInfo>* volumes)
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);

	if(!query.exec("SELECT		id, "
				   "			label, "
				   "			status, "
				   "			type, "
				   "			identifier, "
				   "			specificPath "
				   "FROM		AlbumRoots "
				   "ORDER BY	label;"))
	{
		qDebug() << "cAlbumRootList: loading failed.";
		return(false);
	}

	while(query.next())
	{
		cAlbumRoots*	albumRoots	= add(query.value("id").toInt(), query.value("label").toString(), query.value("status").toInt(), query.value("type").toInt(), query.value("identifier").toString(), query.value("specificPath").toString(), this);

		QString	identifier	= albumRoots->identifier().mid(albumRoots->identifier().lastIndexOf("=")+1);
		for(int x = 0;x < volumes->count();x++)
		{
			if(volumes->at(x).uuid == identifier)
				albumRoots->setDrive(volumes->at(x).path);
		}
	}

	return(true);
}

cAlbumRoots* cAlbumRootsList::add(const qint32& id, QObject* parent)
{
	cAlbumRoots*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cAlbumRoots(id, parent);
	append(lpNew);
	return(lpNew);
}

cAlbumRoots* cAlbumRootsList::add(const qint32& id, const QString& label, const quint32 status, const qint32 type, const QString& identifier, const QString& specificPath, QObject* parent)
{
	cAlbumRoots*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cAlbumRoots(id, label, status, type, identifier, specificPath, parent);
	append(lpNew);
	return(lpNew);
}

cAlbumRoots* cAlbumRootsList::find(const qint32& id)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->id() == id)
			return(at(i));
	}

	return(nullptr);
}
