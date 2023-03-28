#include "ctags.h"

#include <QSqlQuery>

#include <QDebug>


cTags::cTags(const qint32& id, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_pid(0),
	m_name(""),
	m_parent(nullptr),
	m_childList(nullptr),
	m_includeItem(nullptr),
	m_excludeItem(nullptr)
{
}

cTags::cTags(const qint32& id, const qint32& pid, const QString& name, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_pid(pid),
	m_name(name),
	m_parent(nullptr),
	m_childList(nullptr),
	m_includeItem(nullptr),
	m_excludeItem(nullptr)
{
}

qint32 cTags::id()
{
	return(m_id);
}

void cTags::setName(const QString& name)
{
	m_name	= name;
}

QString cTags::name()
{
	return(m_name);
}

void cTags::setPID(const qint32& pid)
{
	m_pid	= pid;
}

qint32 cTags::pid()
{
	return(m_pid);
}

void cTags::setParent(cTags* parent)
{
	m_parent	= parent;
}

cTags* cTags::parent()
{
	return(m_parent);
}

void cTags::setIncludeItem(QStandardItem* includeItem)
{
	m_includeItem	= includeItem;
}

QStandardItem* cTags::includeItem()
{
	return(m_includeItem);
}

void cTags::setExcludeItem(QStandardItem* excludeItem)
{
	m_excludeItem	= excludeItem;
}

QStandardItem* cTags::excludeItem()
{
	return(m_excludeItem);
}

void cTags::addChild(cTags* tags)
{
	if(!m_childList)
		m_childList	= new cTagsList(nullptr);
	m_childList->append(tags);
}

cTagsList* cTags::childList()
{
	return(m_childList);
}

cTagsList::cTagsList(QSqlDatabase* dbDigikam, QObject* parent) :
	QObject(parent),
	m_dbDigikam(dbDigikam)
{
}

cTagsList::~cTagsList()
{
}

bool cTagsList::load()
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);

	if(!query.exec("SELECT		id, "
				   "			pid, "
				   "			name "
				   "FROM		Tags "
				   "ORDER BY	name;"))
	{
		qDebug() << "cTagsList: loading failed.";
		return(false);
	}

	while(query.next())
	{
		add(query.value("id").toInt(), query.value("pid").toInt(), query.value("name").toString(), this);
	}

	processParent();

	return(true);
}

cTags* cTagsList::add(const qint32& id, QObject* parent)
{
	cTags*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cTags(id, parent);
	append(lpNew);
	return(lpNew);
}

cTags* cTagsList::add(const qint32& id, const qint32& pid, const QString& name, QObject* parent)
{
	cTags*	lpNew		= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cTags(id, pid, name, parent);
	append(lpNew);
	return(lpNew);
}

cTags* cTagsList::find(const qint32& id)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->id() == id)
			return(at(i));
	}

	return(nullptr);
}

void cTagsList::processParent()
{
	for(int i = 0;i < count();i++)
	{
		cTags*	tags	= at(i);

		if(tags->pid())
		{
			cTags*	parent	= find(tags->pid());

			if(parent)
			{
				tags->setParent(parent);
				parent->addChild(tags);
			}
		}
	}
}
