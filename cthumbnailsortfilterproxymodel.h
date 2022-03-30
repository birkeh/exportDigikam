/*!
 \file cthumbnailfilterproxymodel.h

*/

#ifndef CTHUMBNAILFILTERPROXYMODEL_H
#define CTHUMBNAILFILTERPROXYMODEL_H


#include <QSortFilterProxyModel>


/*!
 \brief

 \class cThumbnailFilterProxyModel cthumbnailfilterproxymodel.h "cthumbnailfilterproxymodel.h"
*/
class cThumbnailSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	cThumbnailSortFilterProxyModel(QObject* parent = nullptr);

	/*!
	 \brief

	 \fn filterPath
	*/
	QString					filterPath();
	/*!
	 \brief

	 \fn setFilterPath
	 \param szPath
	*/
	void					setFilterPath(const QString& szPath);

	/*!
	 \brief

	 \fn titleList
	*/
	QStringList				titleList();
	/*!
	 \brief

	 \fn setTitleList
	 \param titleList
	*/
	void					setTitleList(const QStringList& titleList);

	/*!
	 \brief

	 \fn personList
	*/
	QList<qint32>			personList();
	/*!
	 \brief

	 \fn setPersonList
	 \param personList
	*/
	void					setPersonList(const QList<qint32>& personList, bool bAnd);

	/*!
	 \brief

	 \fn locationList
	*/
	QList<qint32>			locationList();
	/*!
	 \brief

	 \fn setLocationList
	 \param locationList
	*/
	void					setLocationList(const QList<qint32>& locationList, bool bAnd);

	/*!
	 \brief

	 \fn tagList
	*/
	QList<qint32>			tagList();
	/*!
	 \brief

	 \fn setTagList
	 \param tagList
	*/
	void					setTagList(const QList<qint32>& tagList, bool bAnd);

protected:
	/*!
	 \brief

	 \fn filterAcceptsRow
	 \param sourceRow
	 \param sourceParent
	 \return bool
	*/
	bool					filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
	QString					m_szPath;					/*!< TODO: describe */
	QStringList				m_titleList;				/*!< TODO: describe */
	QList<qint32>			m_personList;				/*!< TODO: describe */
	QList<qint32>			m_locationList;				/*!< TODO: describe */
	QList<qint32>			m_tagList;					/*!< TODO: describe */
	bool					m_bPersonAnd;				/*!< TODO: describe */
	bool					m_bLocationAnd;				/*!< TODO: describe */
	bool					m_bTagAnd;					/*!< TODO: describe */
};

#endif // CTHUMBNAILFILTERPROXYMODEL_H
