/*!
 \file common.h

*/

#ifndef COMMON_H
#define COMMON_H


#include <solid/solidnamespace.h>
#include <solid/camera.h>
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/devicenotifier.h>
#include <solid/predicate.h>
#include <solid/storageaccess.h>
#include <solid/storagedrive.h>
#include <solid/storagevolume.h>

#include <QDateTime>
#include <QDebug>


#define THUMBNAIL_WIDTH		160
#define THUMBNAIL_HEIGHT	120

#define _DELETE_(x)			{if(x) delete(x); x = nullptr;}

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

typedef struct tagIMAGECOMMENT
{
	qint32		id;
	qint32		imageid;
	qint32		type;
	QString		language;
	QString		author;
	QDateTime	date;
	QString		comment;
} IMAGECOMMENT;

typedef struct tagIMAGECOPYRIGHT
{
	qint32		id;
	qint32		imageid;
	QString		property;
	QString		value;
	QString		extraValue;

} IMAGECOPYRIGHT;

typedef struct tagIMAGEINFORMATION
{
	qint32		imageid;
	qint32		rating;
	QDateTime	creationDate;
	QDateTime	digitizationDate;
	qint32		orientation;
	qint32		width;
	qint32		height;
	QString		format;
	qint32		colorDepth;
	qint32		colorModel;
} IMAGEINFORMATION;

typedef struct tagIMAGEMETADATA
{
	qint32		imageid;
	QString		make;
	QString		model;
	QString		lens;
	qreal		aparture;
	qreal		focalLength;
	qreal		focalLength35;
	qreal		exposureTime;
	qint32		exposureProgram;
	qint32		exposureMode;
	qint32		sensitivity;
	qint32		flash;
	qint32		whiteBalance;
	qint32		whiteBalanceColorTemperature;
	qint32		meteringMode;
	qreal		subjectDistance;
	qint32		subjectDistanceCategory;
} IMAGEMETADATA;

typedef struct tagIMAGEPOSITIONS
{
	qint32		imageid;
	QString		latitude;
	qreal		latitideNumber;
	QString		longitude;
	qreal		longitudeNumber;
	qreal		altitude;
	qreal		orientation;
	qreal		tilt;
	qreal		roll;
	qreal		accuracy;
	QString		description;
} IMAGEPOSITIONS;

typedef struct tagIMAGEPROPERTIES
{
	qint32		imageid;
	QString		property;
	QString		value;
} IMAGEPROPERTIES;

typedef struct tagVIDEOMETADATA
{
	qint32		imageid;
	QString		aspectRation;
	QString		audioBitRate;
	QString		audioChannelType;
	QString		audioCompressor;
	QString		duration;
	QString		frameRate;
	qint32		exposureProgram;
	QString		videoCodec;
} VIDEOMETADATA;

class Q_DECL_HIDDEN SolidVolumeInfo
{

public:

	SolidVolumeInfo()
		: isRemovable  (false),
		  isOpticalDisc(false),
		  isMounted    (false)
	{
	}

	bool isNull() const
	{
		return path.isNull();
	}

public:

	QString udi;            ///< Solid device UDI of the StorageAccess device
	QString path;           ///< mount path of volume, with trailing slash
	QString uuid;           ///< UUID as from Solid
	QString label;          ///< volume label (think of CDs)
	bool    isRemovable;    ///< may be removed
	bool    isOpticalDisc;  ///< is an optical disk device as CD/DVD/BR
	bool    isMounted;      ///< is mounted on File System.
};

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

QList<SolidVolumeInfo> actuallyListVolumes();


#endif // COMMON_H
