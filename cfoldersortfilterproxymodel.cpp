/*!
 \file cfoldersortfilterproxymodel.cpp

*/

#include "cfoldersortfilterproxymodel.h"


cFolderSortFilterProxyModel::cFolderSortFilterProxyModel(QObject* parent) :
	QSortFilterProxyModel (parent)
{
}

bool cFolderSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant	leftData	= sourceModel()->data(left);
	QVariant	rightData	= sourceModel()->data(right);

	return(leftData.toString() < rightData.toString());
}
