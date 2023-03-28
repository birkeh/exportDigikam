#ifndef CTAGS_H
#define CTAGS_H


#include "common.h"

#include <QString>
#include <QList>
#include <QStandardItem>

#include <QSqlDatabase>

#include <QObject>

#include <QMetaType>


class cTagsList;

class cTags : public QObject
{
	Q_OBJECT

public:
	cTags(const qint32& id, QObject *parent = nullptr);
	cTags(const qint32& id, const qint32& pid, const QString& name, QObject *parent = nullptr);

	qint32			id();

	void			setName(const QString& name);
	QString			name();

	void			setPID(const qint32& pid);
	qint32			pid();

	void			setParent(cTags* parent);
	cTags*			parent();

	void			setIncludeItem(QStandardItem* includeItem);
	QStandardItem*	includeItem();

	void			setExcludeItem(QStandardItem* excludeItem);
	QStandardItem*	excludeItem();

	void			addChild(cTags* tags);
	cTagsList*		childList();
private:
	qint32			m_id;
	qint32			m_pid;
	QString			m_name;
	cTags*			m_parent;
	cTagsList*		m_childList;
	QStandardItem*	m_includeItem;
	QStandardItem*	m_excludeItem;
};

Q_DECLARE_METATYPE(cTags*)

class cTagsList : public QObject, public QList<cTags*>
{
	Q_OBJECT

public:
	cTagsList(QSqlDatabase* dbDigikam, QObject* parent = nullptr);
	~cTagsList();

	bool			load();
	cTags*			add(const qint32& id, QObject* parent = nullptr);
	cTags*			add(const qint32& id, const qint32& pid, const QString& name, QObject* parent = nullptr);
	cTags*			find(const qint32& id);
	void			processParent();

private:
	QSqlDatabase*	m_dbDigikam;
};

#endif // CTAGS_H
