/*!
 \file common.h

*/

#ifndef COMMON_H
#define COMMON_H


#include <QDebug>


#define THUMBNAIL_WIDTH		160
#define THUMBNAIL_HEIGHT	120


#ifdef __GNUC__
	#define myDebug qDebug() << __FILE__ << "(" << __LINE__ << ") - " << __PRETTY_FUNCTION__ << ":"
#elif __MINGW32__
	#define myDebug qDebug() << __FILE__ << "(" << __LINE__ << ") - " << __PRETTY_FUNCTION__ << ":"
#else
	#define myDebug qDebug() << __FILE__ << "(" << __LINE__ << ") - " << __FUNCTION__ << ":"
#endif


/*!
 \brief

 \class tagIMAGEFORMAT common.h "common.h"
*/
typedef struct tagIMAGEFORMAT
{
	QString	shortName;		/*!< TODO: describe */
	QString	description;	/*!< TODO: describe */
	QString	extension;		/*!< TODO: describe */
	bool	read;			/*!< TODO: describe */
	bool	write;			/*!< TODO: describe */
/*!
 \brief

 \typedef IMAGEFORMAT*/
} IMAGEFORMAT;


/*!
 \brief

 \fn generateReadList
 \param imageFormats
 \return QString
*/
QString	generateReadList(const QList<IMAGEFORMAT>& imageFormats);
/*!
 \brief

 \fn generateWriteList
 \param imageFormats
 \return QString
*/
QString	generateWriteList(const QList<IMAGEFORMAT>& imageFormats);


#endif // COMMON_H
