/*!
 \file cthumbnailfilterproxymodel.cpp

*/

#include "cthumbnailsortfilterproxymodel.h"
//#include "cpicture.h"


cThumbnailSortFilterProxyModel::cThumbnailSortFilterProxyModel(QObject* parent) :
	QSortFilterProxyModel (parent)
{
}

void cThumbnailSortFilterProxyModel::setFilterPath(const QString &szPath)
{
	m_szPath	= szPath;
	m_szPath.replace("\\", "/");

	invalidateFilter();
}

QString cThumbnailSortFilterProxyModel::filterPath()
{
	return(m_szPath);
}

QStringList cThumbnailSortFilterProxyModel::titleList()
{
	return(m_titleList);
}

void cThumbnailSortFilterProxyModel::setTitleList(const QStringList& titleList)
{
	m_titleList		= titleList;

	invalidateFilter();
}

QList<qint32> cThumbnailSortFilterProxyModel::personList()
{
	return(m_personList);
}

void cThumbnailSortFilterProxyModel::setPersonList(const QList<qint32>& personList, bool bAnd)
{
	m_personList	= personList;
	m_bPersonAnd	= bAnd;

	invalidateFilter();
}

QList<qint32> cThumbnailSortFilterProxyModel::locationList()
{
	return(m_locationList);
}

void cThumbnailSortFilterProxyModel::setLocationList(const QList<qint32>& locationList, bool bAnd)
{
	m_locationList	= locationList;
	m_bLocationAnd	= bAnd;

	invalidateFilter();
}

QList<qint32> cThumbnailSortFilterProxyModel::tagList()
{
	return(m_tagList);
}

void cThumbnailSortFilterProxyModel::setTagList(const QList<qint32>& tagList, bool bAnd)
{
	m_tagList	= tagList;
	m_bTagAnd	= bAnd;

	invalidateFilter();
}

bool cThumbnailSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
//	bool		bValid		= false;

//	QModelIndex	index0		= sourceModel()->index(sourceRow, 0, sourceParent);
//	cPicture*	lpPicture	= sourceModel()->data(index0, Qt::UserRole+1).value<cPicture*>();

//	if(!lpPicture)
//		return(false);

//	if(m_szPath.isEmpty())
//		bValid	= true;
//	else
//	{
//		QString		szPath		= lpPicture->filePath();

//		if(!szPath.startsWith(m_szPath))
//			return(false);

//		if(szPath.length() == m_szPath.length())
//			bValid	= true;

//		if(szPath.mid(m_szPath.length(), 1) == "/")
//			bValid	= true;
//	}

//	if(!bValid)
//		return(false);

//	if(!m_titleList.contains(lpPicture->title()) && !m_titleList.isEmpty())
//		return(false);

//	if(!m_personList.isEmpty())
//	{
//		if(!lpPicture->personList().contains(m_personList, m_bPersonAnd))
//			return(false);
//	}

//	if(!m_locationList.isEmpty())
//	{
//		if(!lpPicture->locationList().contains(m_locationList, m_bLocationAnd))
//			return(false);
//	}

//	if(!m_tagList.isEmpty())
//	{
//		if(!lpPicture->tagList().contains(m_tagList, m_bTagAnd))
//			return(false);
//	}

	return(true);
}
