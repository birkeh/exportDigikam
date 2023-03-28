/*!
 \file cexif.h

*/

#ifndef CEXIF_H
#define CEXIF_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QImage>

#include <QMetaType>
#include <QList>

#include <QSqlDatabase>


/*!
 \brief

 \class cEXIFCompression cexif.h "cexif.h"
*/
class cEXIFCompression
{
public:
	cEXIFCompression(const qint32& iID, const QString& szCompression);

	qint32		m_iID;								/*!< TODO: describe */
	QString		m_szCompression;					/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFCompression*)

/*!
 \brief

 \class cEXIFCompressionList cexif.h "cexif.h"
*/
class cEXIFCompressionList : public QList<cEXIFCompression*>
{
public:
	cEXIFCompressionList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szCompression
	 \return cEXIFCompression
	*/
	cEXIFCompression*	add(const qint32& iID, const QString& szCompression);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFCompression
	*/
	cEXIFCompression*	find(const qint32& iID);

	/*!
	 \brief

	 \fn compression
	 \param iID
	 \return QString
	*/
	QString				compression(const qint32& iID);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

//Q_DECLARE_METATYPE(cEXIFCompressionList)

/*!
 \brief

 \class cEXIFLightSource cexif.h "cexif.h"
*/
class cEXIFLightSource
{
public:
	cEXIFLightSource(const qint32& iID, const QString& szLightSource);

	qint32		m_iID;					/*!< TODO: describe */
	QString		m_szLightSource;		/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFLightSource*)

/*!
 \brief

 \class cEXIFLightSourceList cexif.h "cexif.h"
*/
class cEXIFLightSourceList : public QList<cEXIFLightSource*>
{
public:
	cEXIFLightSourceList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szLightSource
	 \return cEXIFLightSource
	*/
	cEXIFLightSource*	add(const qint32& iID, const QString& szLightSource);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFLightSource
	*/
	cEXIFLightSource*	find(const qint32& iID);

	/*!
	 \brief

	 \fn lightSource
	 \param iID
	 \return QString
	*/
	QString			lightSource(const qint32& iID);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

/*!
 \brief

 \class cEXIFFlash cexif.h "cexif.h"
*/
class cEXIFFlash
{
public:
	cEXIFFlash(const qint32& iID, const QString& szFlash);

	qint32		m_iID;					/*!< TODO: describe */
	QString		m_szFlash;				/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFFlash*)

/*!
 \brief

 \class cEXIFFlashList cexif.h "cexif.h"
*/
class cEXIFFlashList : public QList<cEXIFFlash*>
{
public:
	cEXIFFlashList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szFlash
	 \return cEXIFFlash
	*/
	cEXIFFlash*		add(const qint32& iID, const QString& szFlash);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFFlash
	*/
	cEXIFFlash*		find(const qint32& iID);

	/*!
	 \brief

	 \fn flash
	 \param iID
	 \return QString
	*/
	QString			flash(const qint32& iID);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

/**
 * @brief
 *
 */
class cEXIFTag
{
public:
	cEXIFTag(const qint32& iTAGID, const QString& szTAGName, const QString& szIFD, const QString& szKey, const QString& szType, const QString& szDescription);

	qint32		m_iTAGID;					/*!< TODO: describe */
	QString		m_szTAGName;				/*!< TODO: describe */
	QString		m_szIFD;					/*!< TODO: describe */
	QString		m_szKey;					/*!< TODO: describe */
	QString		m_szType;					/*!< TODO: describe */
	QString		m_szDescription;			/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFTag*)

/*!
 \brief

 \class cEXIFTagList cexif.h "cexif.h"
*/
class cEXIFTagList : public QList<cEXIFTag*>
{
public:
	cEXIFTagList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param iTAGID
	 \param szTAGName
	 \param szIFD
	 \param szType
	 \param szDescription
	 \return cEXIFTag
	*/
	cEXIFTag*		add(const qint32& iTAGID, const QString& szTAGName, const QString& szIFD, const QString& szKey, const QString& szType, const QString& szDescription);
	/*!
	 \brief

	 \fn find
	 \param szKey
	 \return cEXIFTag
	*/
	cEXIFTag*		find(const QString& szKey);
	/*!
	 \brief

	 \fn find
	 \param iTAGID
	 \param szIFD
	 \return cEXIFTag
	*/
	cEXIFTag*		find(const qint32& iTAGID, const QString& szIFD);
	/*!
	 \brief

	 \fn find
	 \param szTAG
	 \param szIFD
	 \return cEXIFTag
	*/
	cEXIFTag*		find(const QString& szTAG, const QString& szIFD);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

/*!
 \brief

*/
class cEXIFValue
{
public:
	cEXIFValue(cEXIFTag* lpEXIFTag);

	/*!
	 \brief

	 \fn exifTag
	 \return cEXIFTag
	*/
	cEXIFTag*		exifTag();
	/*!
	 \brief

	 \fn setValue
	 \param szValue
	 \param iTypeId
	 \param iCount
	*/
	void			setValue(const QString& szValue, qint32 iTypeId, qint32 iCount = 1);
	/*!
	 \brief

	 \fn value
	 \return QVariant
	*/
	QVariant		value();
	/*!
	 \brief

	 \fn valueList
	 \return QList<QVariant>
	*/
	QList<QVariant>	valueList();

private:
	cEXIFTag*		m_lpEXIFTag;				/*!< TODO: describe */
	QList<QVariant>	m_valueList;				/*!< TODO: describe */

	/*!
	 \brief

	 \fn convertValue
	 \param szValue
	 \param iTypeId
	 \return QVariant
	*/
	QVariant		convertValue(const QString& szValue, qint32 iTypeId);
};

Q_DECLARE_METATYPE(cEXIFValue*)

/*!
 \brief

 \class cEXIFValueList cexif.h "cexif.h"
*/
class cEXIFValueList : public QList<cEXIFValue*>
{
public:
	cEXIFValueList();

	/*!
	 \brief

	 \fn add
	 \param lpEXIFTag
	 \return cEXIFValue
	*/
	cEXIFValue*		add(cEXIFTag* lpEXIFTag);
	/*!
	 \brief

	 \fn find
	 \param lpEXIFTag
	 \return cEXIFValue
	*/
	cEXIFValue*		find(cEXIFTag* lpEXIFTag);
};

/**
 * @brief
 *
 */
class cIPTCTag
{
public:
	cIPTCTag(const qint32& iTAGID, const QString& szTAGName, const qint32& iTypeID, const QString& szDescription);

	qint32		m_iTAGID;					/*!< TODO: describe */
	QString		m_szTAGName;				/*!< TODO: describe */
	qint32		m_iTypeID;					/*!< TODO: describe */
	QString		m_szDescription;			/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cIPTCTag*)

/*!
 \brief

 \class cIPTCTagList cexif.h "cexif.h"
*/
class cIPTCTagList : public QList<cIPTCTag*>
{
public:
	cIPTCTagList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param iTAGID
	 \param szTAGName
	 \param iTypeID
	 \param szDescription
	 \return cIPTCTag
	*/
	cIPTCTag*		add(const qint32& iTAGID, const QString& szTAGName, const qint32& iTypeID, const QString& szDescription);
	/*!
	 \brief

	 \fn find
	 \param iTAGID
	 \return cIPTCTag
	*/
	cIPTCTag*		find(const qint32& iTAGID);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

/*!
 \brief

*/
class cIPTCValue
{
public:
	cIPTCValue(cIPTCTag* lpIPTCTag);

	/*!
	 \brief

	 \fn iptcTag
	 \return cIPTCTag
	*/
	cIPTCTag*		iptcTag();
	/*!
	 \brief

	 \fn setValue
	 \param szValue
	 \param iTypeId
	 \param iCount
	*/
	void			setValue(const QString& szValue, qint32 iTypeId, qint32 iCount = 1);
	/*!
	 \brief

	 \fn value
	 \return QVariant
	*/
	QVariant		value();
	/*!
	 \brief

	 \fn valueList
	 \return QList<QVariant>
	*/
	QList<QVariant>	valueList();

private:
	cIPTCTag*		m_lpIPTCTag;				/*!< TODO: describe */
	QList<QVariant>	m_valueList;				/*!< TODO: describe */

	/*!
	 \brief

	 \fn convertValue
	 \param szValue
	 \param iTypeId
	 \return QVariant
	*/
	QVariant		convertValue(const QString& szValue, qint32 iTypeId);
};

Q_DECLARE_METATYPE(cIPTCValue*)

/*!
 \brief

 \class cIPTCValueList cexif.h "cexif.h"
*/
class cIPTCValueList : public QList<cIPTCValue*>
{
public:
	cIPTCValueList();

	/*!
	 \brief

	 \fn add
	 \param lpIPTCTag
	 \return cIPTCValue
	*/
	cIPTCValue*		add(cIPTCTag* lpIPTCTag);
	/*!
	 \brief

	 \fn find
	 \param lpIPTCTag
	 \return cIPTCValue
	*/
	cIPTCValue*		find(cIPTCTag* lpIPTCTag);
};

/**
 * @brief
 *
 */
class cXMPTag
{
public:
	cXMPTag(const QString& szTAGName, const qint32& iTypeID, const QString& szDescription);

	QString		m_szTAGName;				/*!< TODO: describe */
	qint32		m_iTypeID;					/*!< TODO: describe */
	QString		m_szDescription;			/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cXMPTag*)

/*!
 \brief

 \class cXMPTagList cexif.h "cexif.h"
*/
class cXMPTagList : public QList<cXMPTag*>
{
public:
	cXMPTagList(QSqlDatabase* db);

	/*!
	 \brief

	 \fn add
	 \param szTAGName
	 \param iTypeID
	 \param szDescription
	 \return cXMPTag
	*/
	cXMPTag*		add(const QString& szTAGName, const qint32& iTypeID, const QString& szDescription);
	/*!
	 \brief

	 \fn find
	 \param szTAGName
	 \return cXMPTag
	*/
	cXMPTag*		find(const QString& szTAGName);

private:
	QSqlDatabase*		m_dbExportDigikam;
};

/*!
 \brief

*/
class cXMPValue
{
public:
	cXMPValue(cXMPTag* lpXMPTag);

	/*!
	 \brief

	 \fn exifTag
	 \return cXMPTag
	*/
	cXMPTag*		xmpTag();
	/*!
	 \brief

	 \fn setValue
	 \param szValue
	 \param iTypeId
	 \param iCount
	*/
	void			setValue(const QString& szValue, qint32 iTypeId, qint32 iCount = 1);
	/*!
	 \brief

	 \fn value
	 \return QVariant
	*/
	QVariant		value();
	/*!
	 \brief

	 \fn valueList
	 \return QList<QVariant>
	*/
	QList<QVariant>	valueList();

private:
	cXMPTag*		m_lpXMPTag;					/*!< TODO: describe */
	QList<QVariant>	m_valueList;				/*!< TODO: describe */

	/*!
	 \brief

	 \fn convertValue
	 \param szValue
	 \param iTypeId
	 \return QVariant
	*/
	QVariant		convertValue(const QString& szValue, qint32 iTypeId);
};

Q_DECLARE_METATYPE(cXMPValue*)

/*!
 \brief

 \class cXMPValueList cexif.h "cexif.h"
*/
class cXMPValueList : public QList<cXMPValue*>
{
public:
	cXMPValueList();

	/*!
	 \brief

	 \fn add
	 \param lpXMPTag
	 \return cXMPValue
	*/
	cXMPValue*		add(cXMPTag* lpXMPTag);
	/*!
	 \brief

	 \fn find
	 \param lpXMPTag
	 \return cXMPValue
	*/
	cXMPValue*		find(cXMPTag* lpXMPTag);
};

/*!
 \brief

 \class cEXIF cexif.h "cexif.h"
*/
class cEXIF
{
public:
	cEXIF(cEXIFTagList* lpEXIFTagList, cEXIFCompressionList* lpEXIFCompressionList, cEXIFLightSourceList* lpEXIFLightSourceList, cEXIFFlashList* lpEXIFFlashList, cIPTCTagList* lpIPTCTagList, cXMPTagList* lpXMPTagList);

	/*!
	 \brief

	 \fn fromFile
	 \param szFileName
	 \param withThumbnail
	 \return bool
	*/
	bool					fromFile(const QString& szFileName, bool withThumbnail = true);

	/*!
	 \brief

	 \fn mimeType
	 \return QString
	*/
	QString					mimeType();
	/*!
	 \brief

	 \fn imageWidth
	 \return qint32
	*/
	qint32					imageWidth();
	/*!
	 \brief

	 \fn imageHeight
	 \return qint32
	*/
	qint32					imageHeight();
	/*!
	 \brief

	 \fn imageOrientation
	 \return qint16
	*/
	qint16					imageOrientation();
	/*!
	 \brief

	 \fn cameraMake
	 \return QString
	*/
	QString					cameraMake();
	/*!
	 \brief

	 \fn cameraModel
	 \return QString
	*/
	QString					cameraModel();
	/*!
	 \brief

	 \fn dateTime
	 \return QDateTime
	*/
	QDateTime				dateTime();
	/*!
	 \brief

	 \fn fNumber
	 \return QString
	*/
	QString					fNumber();
	/*!
	 \brief

	 \fn iso
	 \return qint32
	*/
	qint32					iso();
	/*!
	 \brief

	 \fn flash
	 \return QString
	*/
	QString					flash();
	/*!
	 \brief

	 \fn flashID
	 \return qint32
	*/
	qint32					flashID();
	/*!
	 \brief

	 \fn focalLength
	 \return qreal
	*/
	qreal					focalLength();
	/*!
	 \brief

	 \fn lensMake
	 \return QString
	*/
	QString					lensMake();
	/*!
	 \brief

	 \fn lensModel
	 \return QString
	*/
	QString					lensModel();
	/*!
	 \brief

	 \fn exposureTime
	 \return QString
	*/
	QString					exposureTime();
	/*!
	 \brief

	 \fn exposureBias
	 \return qint32
	*/
	qint32					exposureBias();
	/*!
	 \brief

	 \fn exifVersion
	 \return QString
	*/
	QString					exifVersion();
	/*!
	 \brief

	 \fn dateTimeOriginal
	 \return QDateTime
	*/
	QDateTime				dateTimeOriginal();
	/*!
	 \brief

	 \fn dateTimeDigitized
	 \return QDateTime
	*/
	QDateTime				dateTimeDigitized();
	/*!
	 \brief

	 \fn whiteBalance
	 \return qint32
	*/
	qint32					whiteBalance();
	/*!
	 \brief

	 \fn focalLength35
	 \return qreal
	*/
	qreal					focalLength35();
	/*!
	 \brief

	 \fn gps
	 \return QString
	*/
	QString					gps();
	/*!
	 \brief

	 \fn duration
	 \return QString
	*/
	QString					duration();
	/*!
	 \brief

	 \fn fileName
	 \return QString
	*/
	QString					fileName();
	/*!
	 \brief

	 \fn previewList
	 \return QList<QImage>
	*/
	QList<QImage>			previewList();
	/*!
	 \brief

	 \fn thumbnail
	 \return QImage
	*/
	QImage					thumbnail();

	/*!
	 \brief

	 \fn copyTo
	 \param fileName
	*/
	bool copyTo(const QString& fileName);

private:
	cEXIFValueList			m_exifValueList;				/*!< TODO: describe */
	cIPTCValueList			m_iptcValueList;				/*!< TODO: describe */
	cXMPValueList			m_xmpValueList;					/*!< TODO: describe */
	QString					m_szMimeType;					/*!< TODO: describe */
	qint32					m_iWidth;						/*!< TODO: describe */
	qint32					m_iHeight;						/*!< TODO: describe */
	QString					m_szFileName;					/*!< TODO: describe */
	QList<QImage>			m_previewList;					/*!< TODO: describe */
	QImage					m_thumbnail;					/*!< TODO: describe */

	cEXIFTagList*			m_lpEXIFTagList;				/*!< TODO: describe */
	cEXIFCompressionList*	m_lpEXIFCompressionList;		/*!< TODO: describe */
	cEXIFLightSourceList*	m_lpEXIFLightSourceList;		/*!< TODO: describe */
	cEXIFFlashList*			m_lpEXIFFlashList;				/*!< TODO: describe */

	cIPTCTagList*			m_lpIPTCTagList;				/*!< TODO: describe */

	cXMPTagList*			m_lpXMPTagList;					/*!< TODO: describe */

	/*!
	 \brief

	 \fn getEXIFTag
	 \param iTAGID
	 \param szIFD
	 \return QVariant
	*/
	QVariant				getEXIFTag(qint32 iTAGID, const QString& szIFD);
	/*!
	 \brief

	 \fn getIPTCTag
	 \param iTAGID
	 \return QVariant
	*/
	QVariant				getIPTCTag(qint32 iTAGID);
	/*!
	 \brief

	 \fn getXMPTag
	 \param szTAGName
	 \return QVariant
	*/
	QVariant				getXMPTag(const QString& szTAGName);
	/*!
	 \brief

	 \fn getTagList
	 \param iTAGID
	 \param iIFDID
	 \return QList<QVariant>
	*/
	QList<QVariant>			getTagList(qint32 iTAGID, const QString& szIFD);
};

Q_DECLARE_METATYPE(cEXIF*)

#endif // CEXIF_H
