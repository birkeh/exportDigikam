/*!
 \file cfoldersortfilterproxymodel.h

*/

#ifndef CFOLDERSORTFILTERPROXYMODEL_H
#define CFOLDERSORTFILTERPROXYMODEL_H


#include <QSortFilterProxyModel>


/*!
 \brief

 \class cFolderSortFilterProxyModel cfoldersortfilterproxymodel.h "cfoldersortfilterproxymodel.h"
*/
class cFolderSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	cFolderSortFilterProxyModel(QObject* parent = nullptr);

protected:
	bool		lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // CFOLDERSORTFILTERPROXYMODEL_H
