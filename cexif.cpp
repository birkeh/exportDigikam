/*!
 \file cexif.cpp

*/

#include "cexif.h"

#include "common.h"

#include <QDebug>
#include <QImage>
#include <QPicture>
#include <QFileInfo>

#include <QSqlDatabase>
#include <QSqlQuery>

#include "cimage.h"

#include <exiv2/exiv2.hpp>


cEXIF::cEXIF(cEXIFTagList* lpEXIFTagList, cEXIFCompressionList* lpEXIFCompressionList, cEXIFLightSourceList* lpEXIFLightSourceList, cEXIFFlashList* lpEXIFFlashList, cIPTCTagList* lpIPTCTagList, cXMPTagList* lpXMPTagList) :
	m_szMimeType(""),
	m_iWidth(0),
	m_iHeight(0),
	m_szFileName(""),
	m_lpEXIFTagList(lpEXIFTagList),
	m_lpEXIFCompressionList(lpEXIFCompressionList),
	m_lpEXIFLightSourceList(lpEXIFLightSourceList),
	m_lpEXIFFlashList(lpEXIFFlashList),
	m_lpIPTCTagList(lpIPTCTagList),
	m_lpXMPTagList(lpXMPTagList)
{
}

bool cEXIF::fromFile(const QString& szFileName, bool withThumbnail)
{
	if(!QFile::exists(szFileName))
		return(false);

	m_exifValueList.clear();
	m_previewList.clear();

	m_szFileName	= "";

	try
	{
		Exiv2::Image::AutoPtr	image		= Exiv2::ImageFactory::open(szFileName.toLocal8Bit().toStdString());
		if(!image.get())
			return(false);

		image->readMetadata();

		Exiv2::ExifData&		exifData	= image->exifData();
		Exiv2::IptcData&		iptcData	= image->iptcData();
		Exiv2::XmpData&			xmpData		= image->xmpData();

		m_szMimeType	= QString::fromStdString(image->mimeType());
		m_iWidth		= image->pixelWidth();
		m_iHeight		= image->pixelHeight();

		m_szFileName	= szFileName;

		if(!iptcData.empty())
		{
			Exiv2::IptcData::const_iterator	end		= iptcData.end();

			for(Exiv2::IptcData::const_iterator i = iptcData.begin(); i != end; ++i)
			{
				cIPTCTag*	lpTag	= m_lpIPTCTagList->find(i->tag());

				if(lpTag)
				{
					cIPTCValue*	lpValue	= m_iptcValueList.add(lpTag);
					if(lpValue)
						lpValue->setValue(QString::fromStdString(i->value().toString()), i->typeId(), i->count());
				}
			}

			Exiv2::PreviewManager			previewManager(*image);
			Exiv2::PreviewPropertiesList	previewPropertiesList	= previewManager.getPreviewProperties();

			for(Exiv2::PreviewPropertiesList::const_iterator i = previewPropertiesList.begin();i != previewPropertiesList.end();i++)
			{
				Exiv2::PreviewImage				previewImage			= previewManager.getPreviewImage(*i);
				QImage							image;
				image.loadFromData(static_cast<const uchar*>(previewImage.pData()), static_cast<qint32>(previewImage.size()));

				m_previewList.append(image);
			}
		}

		if(!exifData.empty())
		{
			Exiv2::ExifData::const_iterator	end			= exifData.end();
			for(Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i)
			{
				cEXIFTag*	lpTag	= m_lpEXIFTagList->find(QString::fromUtf8(i->key().c_str()));

				if(lpTag)
				{
					cEXIFValue*	lpValue	= m_exifValueList.add(lpTag);
					if(lpValue)
						lpValue->setValue(QString::fromStdString(i->value().toString()), i->typeId(), i->count());
				}
			}

			Exiv2::PreviewManager			previewManager(*image);
			Exiv2::PreviewPropertiesList	previewPropertiesList	= previewManager.getPreviewProperties();

			for(Exiv2::PreviewPropertiesList::const_iterator i = previewPropertiesList.begin();i != previewPropertiesList.end();i++)
			{
				Exiv2::PreviewImage				previewImage			= previewManager.getPreviewImage(*i);
				QImage							image;
				image.loadFromData(static_cast<const uchar*>(previewImage.pData()), static_cast<qint32>(previewImage.size()));

				m_previewList.append(image);
			}
		}

		if(!xmpData.empty())
		{
			Exiv2::XmpData::const_iterator	end		= xmpData.end();

			for(Exiv2::XmpData::const_iterator i = xmpData.begin(); i != end; ++i)
			{
				cXMPTag*	lpTag	= m_lpXMPTagList->find(QString::fromStdString(i->key()));

				if(lpTag)
				{
					cXMPValue*	lpValue	= m_xmpValueList.add(lpTag);
					if(lpValue)
						lpValue->setValue(QString::fromStdString(i->value().toString()), i->typeId(), i->count());
				}
			}

			Exiv2::PreviewManager			previewManager(*image);
			Exiv2::PreviewPropertiesList	previewPropertiesList	= previewManager.getPreviewProperties();

			for(Exiv2::PreviewPropertiesList::const_iterator i = previewPropertiesList.begin();i != previewPropertiesList.end();i++)
			{
				Exiv2::PreviewImage				previewImage			= previewManager.getPreviewImage(*i);
				QImage							image;
				image.loadFromData(static_cast<const uchar*>(previewImage.pData()), static_cast<qint32>(previewImage.size()));

				m_previewList.append(image);
			}
		}
	}
	catch (Exiv2::AnyError& e)
	{
		qDebug() << "Caught Exiv2 exception '" << e.what();
		return(false);
	}

	if(!m_previewList.count() && withThumbnail)
	{
		cImage	image;
		if(image.load(szFileName))
		{
			QTransform	rotation;
			int			angle	= 0;

			switch(imageOrientation())
			{
			case 8:
				angle	= 270;
				break;
			case 3:
				angle	= 180;
				break;
			case 6:
				angle	=  90;
				break;
			}

			if(angle != 0)
			{
				rotation.rotate(angle);
				image	= image.transformed(rotation);
			}

			m_thumbnail	= image.scaled(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
	}
	else
	{
		qint32	index		= -1;
		qint32	widthDiff	= std::numeric_limits<qint32>::max();
		qint32	heightDiff	= std::numeric_limits<qint32>::max();

		for(qint32 i = 0;i < m_previewList.count();i++)
		{
			qint32	wd		= m_previewList[i].width() - THUMBNAIL_WIDTH;
			qint32	hd		= m_previewList[i].height() - THUMBNAIL_HEIGHT;

			if(wd >= 0 && hd >= 0)
			{
				if(wd <= widthDiff && hd <= heightDiff)
				{
					index		= i;
					widthDiff	= wd;
					heightDiff	= hd;
				}
			}
		}

		if(index == -1)
		{
			for(qint32 i = 0;i < m_previewList.count();i++)
			{
				qint32	wd		= m_previewList[i].width() - THUMBNAIL_WIDTH;
				qint32	hd		= m_previewList[i].height() - THUMBNAIL_HEIGHT;

				if(abs(wd) <= widthDiff && abs(hd) <= heightDiff)
				{
					index	= i;
					widthDiff	= abs(wd);
					heightDiff	= abs(hd);
				}
			}
		}

		if(index != -1)
		{
			QTransform	rotation;
			int			angle	= 0;

			switch(imageOrientation())
			{
			case 8:
				angle	= 270;
				break;
			case 3:
				angle	= 180;
				break;
			case 6:
				angle	=  90;
				break;
			}

			m_thumbnail	= m_previewList[index].scaled(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);

			if(angle != 0)
			{
				rotation.rotate(angle);
				m_thumbnail	= m_thumbnail.transformed(rotation);
			}
		}
	}

	return(true);
}

QString cEXIF::mimeType()
{
	return(m_szMimeType);
}

qint32 cEXIF::imageWidth()
{
	if(m_iWidth)
		return(m_iWidth);

	qint32	iWidth	= getEXIFTag(0x0100, "Image").value<qint32>();

	if(iWidth)
		return(iWidth);

	return(getXMPTag("Xmp.video.Width").value<qint32>());
}

qint32 cEXIF::imageHeight()
{
	if(m_iHeight)
		return(m_iHeight);

	qint32	iHeight	= getEXIFTag(0x0101, "Image").value<qint32>();

	if(iHeight)
		return(iHeight);

	return(getXMPTag("Xmp.video.Height").value<qint32>());
}

qint16 cEXIF::imageOrientation()
{
	return(getEXIFTag(0x0112, "Image").value<qint16>());
}

QString cEXIF::cameraMake()
{
	QString	szCameraMake	= getEXIFTag(0x010f, "Image").toString();

	if(!szCameraMake.isEmpty())
		return(szCameraMake);

	return(getXMPTag("Xmp.video.MajorBrand").toString());
}

QString cEXIF::cameraModel()
{
	QString	szCameraModel	= getEXIFTag(0x0110, "Image").toString();

	if(!szCameraModel.isEmpty())
		return(szCameraModel);

	return(getXMPTag("Xmp.video.MajorBrand").toString());
}

QDateTime cEXIF::dateTime()
{
	QDateTime	dateTime	= QDateTime::fromString(getEXIFTag(0x9004, "Image").value<QString>(), "yyyy:MM:dd hh:mm:ss");
	if(dateTime.isValid())
		return(dateTime);

	dateTime	= QDateTime::fromString(getXMPTag("Xmp.video.DateTimeDigitized").toString(), "yyyy-MM-ddThh:mm:ssZ");
	if(dateTime.isValid())
		return(dateTime);

	dateTime	= QDateTime::fromString(getEXIFTag(0x0132, "Image").value<QString>(), "yyyy:MM:dd hh:mm:ss");
	if(dateTime.isValid())
		return(dateTime);

	QFileInfo	info(m_szFileName);
	return(info.lastModified());
}

QString cEXIF::fNumber()
{
	return(getEXIFTag(0x829d, "Photo").value<QString>());
}

qint32 cEXIF::iso()
{
	return(getEXIFTag(0x8827, "Photo").value<qint32>());
}

QString cEXIF::flash()
{
	return(m_lpEXIFFlashList->flash(getEXIFTag(0x9209, "Photo").value<qint32>()));
}

qint32 cEXIF::flashID()
{
	return(getEXIFTag(0x9209, "Photo").value<qint32>());
}

qreal cEXIF::focalLength()
{
	return(getEXIFTag(0x920a, "Photo").value<qreal>());
}

QString cEXIF::lensMake()
{
	return(getEXIFTag(0xa433, "Photo").value<QString>());
}

QString cEXIF::lensModel()
{
	return(getEXIFTag(0xa434, "Photo").value<QString>());
}

QString cEXIF::exposureTime()
{
	qreal	value	= getEXIFTag(0x829a, "Photo").value<qreal>();

	if(value == 0.0)
		return("error");

	if(value >= 1.0)
		return(QString("%1 sec").arg(value));

	return(QString("1/%1").arg(1/value));
}

qint32 cEXIF::exposureBias()
{
	return(getEXIFTag(0x9204, "Photo").value<qint32>());
}

QString cEXIF::exifVersion()
{
	return(getEXIFTag(0x9000, "Photo").value<QString>());
}

QDateTime cEXIF::dateTimeOriginal()
{
	return(QDateTime::fromString(getEXIFTag(0x9003, "Photo").value<QString>(), "yyyy:MM:dd hh:mm:ss"));
}

QDateTime cEXIF::dateTimeDigitized()
{
	return(QDateTime::fromString(getEXIFTag(0x9004, "Photo").value<QString>(), "yyyy:MM:dd hh:mm:ss"));
}

qint32 cEXIF::whiteBalance()
{
	return(getEXIFTag(0xa403, "Photo").value<qint32>());
}

qreal cEXIF::focalLength35()
{
	return(getEXIFTag(0xa405, "Photo").value<qreal>());
}

QString cEXIF::gps()
{
	QList<QVariant> nList	= getTagList(0x0002, "GPSInfo");
	QList<QVariant> eList	= getTagList(0x0004, "GPSInfo");

	if(nList.count() != 3)
		return("");
	if(eList.count() != 3)
		return("");

	QString szGPS	= QString("%1 %2° %3' %4\" %5 %6° %7' %8\"").arg(getEXIFTag(0x0001, "GPSInfo").value<QString>()).arg(nList[0].value<QString>()).arg(nList[1].value<QString>()).arg(nList[2].value<QString>()).arg(getEXIFTag(0x0001, "GPSInfo").value<QString>()).arg(eList[0].value<QString>()).arg(eList[1].value<QString>()).arg(eList[2].value<QString>());
	return(szGPS);
}

QString cEXIF::duration()
{
	return(getXMPTag("Xmp.video.Duration").toString());
}

QString cEXIF::fileName()
{
	return(m_szFileName);
}

QList<QImage> cEXIF::previewList()
{
	return(m_previewList);
}

QImage cEXIF::thumbnail()
{
	return(m_thumbnail);
}

QVariant cEXIF::getEXIFTag(qint32 iTAGID, const QString& szIFD)
{
	cEXIFTag*	lpTag	= m_lpEXIFTagList->find(iTAGID, szIFD);

	if(!lpTag)
		return(QVariant());

	cEXIFValue*	lpValue	= m_exifValueList.find(lpTag);

	if(!lpValue)
		return(QVariant());

	return(lpValue->value());
}

QVariant cEXIF::getIPTCTag(qint32 iTAGID)
{
	cIPTCTag*	lpTag	= m_lpIPTCTagList->find(iTAGID);

	if(!lpTag)
		return(QVariant());

	cIPTCValue*	lpValue	= m_iptcValueList.find(lpTag);

	if(!lpValue)
		return(QVariant());

	return(lpValue->value());
}

QVariant cEXIF::getXMPTag(const QString& szTAGName)
{
	cXMPTag*	lpTag	= m_lpXMPTagList->find(szTAGName);

	if(!lpTag)
		return(QVariant());

	cXMPValue*	lpValue	= m_xmpValueList.find(lpTag);

	if(!lpValue)
		return(QVariant());

	return(lpValue->value());
}

QList<QVariant> cEXIF::getTagList(qint32 iTAGID, const QString& szIFD)
{
	cEXIFTag*	lpTag	= m_lpEXIFTagList->find(iTAGID, szIFD);

	if(!lpTag)
		return(QList<QVariant>());

	cEXIFValue*	lpValue	= m_exifValueList.find(lpTag);

	if(!lpValue)
		return(QList<QVariant>());

	return(lpValue->valueList());
}

bool cEXIF::copyTo(const QString& fileName)
{
	try
	{
//		Exiv2::Image::UniquePtr	image		= Exiv2::ImageFactory::open(fileName.toLocal8Bit().toStdString());
		Exiv2::Image::AutoPtr	image		= Exiv2::ImageFactory::open(fileName.toLocal8Bit().toStdString());
		if(!image.get())
			return(false);

		image->readMetadata();

		Exiv2::ExifData&		exifData	= image->exifData();

		for(int x = 0;x < m_exifValueList.count();x++)
		{
			cEXIFValue*	lpEXIFValue	= m_exifValueList[x];

//			if(!lpEXIFValue->exifTag()->m_szKey.startsWith("Exif.Image"))
				exifData[lpEXIFValue->exifTag()->m_szKey.toUtf8().constData()]	= lpEXIFValue->value().toString().toUtf8().constData();
		}

		image->setExifData(exifData);
		image->writeMetadata();
	}
	catch (Exiv2::AnyError& e)
	{
		qDebug() << "Caught Exiv2 exception '" << e.what();
		return(false);
	}

	return(true);
}

cEXIFCompression::cEXIFCompression(const qint32& iID, const QString& szCompression) :
	m_iID(iID),
	m_szCompression(szCompression)
{
}

cEXIFCompressionList::cEXIFCompressionList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
	QSqlQuery	query(*m_dbExportDigikam);

	query.prepare("SELECT id, compression FROM exifCompression;");
	if(!query.exec())
		return;

	while(query.next())
		add(query.value("id").toInt(), query.value("compression").toString());
}

cEXIFCompression* cEXIFCompressionList::add(const qint32& iID, const QString& szCompression)
{
	cEXIFCompression*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFCompression(iID, szCompression);
	append(lpNew);
	return(lpNew);
}

cEXIFCompression* cEXIFCompressionList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFCompression*	lpCompression= at(x);

		if(lpCompression->m_iID == iID)
			return(lpCompression);
	}
	return(nullptr);
}

QString cEXIFCompressionList::compression(const qint32& iID)
{
	cEXIFCompression* lpCompression	= find(iID);
	if(!lpCompression)
		return(QObject::tr("unknown"));
	return(lpCompression->m_szCompression);
}

cEXIFLightSource::cEXIFLightSource(const qint32& iID, const QString& szLightSource) :
	m_iID(iID),
	m_szLightSource(szLightSource)
{
}

cEXIFLightSourceList::cEXIFLightSourceList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
	QSqlQuery	query(*m_dbExportDigikam);

	query.prepare("SELECT id, lightsource FROM exifLightSource;");
	if(!query.exec())
		return;

	while(query.next())
		add(query.value("id").toInt(), query.value("lightsource").toString());
}

cEXIFLightSource* cEXIFLightSourceList::add(const qint32& iID, const QString& szLightSource)
{
	cEXIFLightSource*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFLightSource(iID, szLightSource);
	append(lpNew);
	return(lpNew);
}

cEXIFLightSource* cEXIFLightSourceList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFLightSource*	lpLightSource	= at(x);

		if(lpLightSource->m_iID == iID)
			return(lpLightSource);
	}
	return(nullptr);
}

QString cEXIFLightSourceList::lightSource(const qint32& iID)
{
	cEXIFLightSource* lpLightSource	= find(iID);
	if(!lpLightSource)
		return(QObject::tr("unknown"));
	return(lpLightSource->m_szLightSource);
}

cEXIFFlash::cEXIFFlash(const qint32& iID, const QString& szFlash) :
	m_iID(iID),
	m_szFlash(szFlash)
{
}

cEXIFFlashList::cEXIFFlashList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
	QSqlQuery	query(*m_dbExportDigikam);

	query.prepare("SELECT id, lightsource FROM exifLightSource;");
	if(!query.exec())
		return;

	while(query.next())
		add(query.value("id").toInt(), query.value("lightsource").toString());
}

cEXIFFlash* cEXIFFlashList::add(const qint32& iID, const QString& szFlash)
{
	cEXIFFlash*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFFlash(iID, szFlash);
	append(lpNew);
	return(lpNew);
}

cEXIFFlash* cEXIFFlashList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFFlash*	lpFlash	= at(x);

		if(lpFlash->m_iID == iID)
			return(lpFlash);
	}
	return(nullptr);
}

QString cEXIFFlashList::flash(const qint32& iID)
{
	cEXIFFlash* lpFlash	= find(iID);
	if(!lpFlash)
		return(QObject::tr("unknown"));
	return(lpFlash->m_szFlash);
}

cEXIFTag::cEXIFTag(const qint32& iTAGID, const QString& szTAGName, const QString& szIFD, const QString& szKey, const QString& szType, const QString& szDescription) :
	m_iTAGID(iTAGID),
	m_szTAGName(szTAGName),
	m_szIFD(szIFD),
	m_szKey(szKey),
	m_szType(szType),
	m_szDescription(szDescription)
{
}

cEXIFTagList::cEXIFTagList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
	QSqlQuery	query(*m_dbExportDigikam);

	query.prepare("SELECT tag, ifd, key, type, description FROM exifTags;");
	if(!query.exec())
		return;

	while(query.next())
		add(query.value("tag").toInt(), query.value("key").toString(), query.value("ifd").toString(), query.value("key").toString(), query.value("type").toString(), query.value("description").toString());
}

cEXIFTag* cEXIFTagList::add(const qint32& iTAGID, const QString& szTAGName, const QString& szIFD, const QString& szKey, const QString& szType, const QString& szDescription)
{
	cEXIFTag*	lpNew	= find(iTAGID, szIFD);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFTag(iTAGID, szTAGName, szIFD, szKey, szType, szDescription);
	append(lpNew);
	return(lpNew);
}

cEXIFTag* cEXIFTagList::find(const QString& szKey)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFTag*	lpTag	= at(x);

		if(lpTag->m_szKey == szKey)
			return(lpTag);
	}
	return(nullptr);
}

cEXIFTag* cEXIFTagList::find(const qint32& iTAGID, const QString& szIFD)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFTag*	lpTag	= at(x);

		if(lpTag->m_iTAGID == iTAGID && lpTag->m_szIFD== szIFD)
			return(lpTag);
	}
	return(nullptr);
}

cEXIFTag* cEXIFTagList::find(const QString& szTAG, const QString& szIFD)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFTag*	lpTag	= at(x);

		if(lpTag->m_szTAGName == szTAG && lpTag->m_szIFD== szIFD)
			return(lpTag);
	}
	return(nullptr);
}

cEXIFTag* cEXIFValue::exifTag()
{
	return(m_lpEXIFTag);
}

cEXIFValue::cEXIFValue(cEXIFTag* lpEXIFTag) :
	m_lpEXIFTag(lpEXIFTag)
{
}

QVariant cEXIFValue::convertValue(const QString& szValue, qint32 iTypeId)
{
	QVariant	variant;

	switch(iTypeId)
	{
	case 1:	//byte
	case 3: //short
	case 4: //long
	case 8: //signed short
		variant = QVariant::fromValue(szValue.toLong());
		break;
	case 5: //rational
	case 10: //signed rational
	case 11: //float
		if(szValue.contains("/"))
		{
			QStringList	tmp	= szValue.split("/");
			variant = QVariant::fromValue(tmp[0].toDouble()/tmp[1].toDouble());
		}
		else
			variant	= QVariant::fromValue(szValue.toDouble());
		break;
	default:
		variant	= szValue;
	}
	return(variant);
}

void cEXIFValue::setValue(const QString& szValue, qint32 iTypeId, qint32 iCount)
{
	if(iCount == 1 || iTypeId == 2 || iTypeId == 7)
	{
		m_valueList.append(convertValue(szValue, iTypeId));
		return;
	}

	QStringList	valueList	= szValue.split(" ");

	if(valueList.count() != iCount)
		return;

	for(int x = 0;x < iCount;x++)
		m_valueList.append(convertValue(valueList[x], iTypeId));
}

QVariant cEXIFValue::value()
{
	if(m_valueList.count())
		return(m_valueList[0]);

	return(QVariant());
}

QList<QVariant> cEXIFValue::valueList()
{
	return(m_valueList);
}

cEXIFValueList::cEXIFValueList()
{
}

cEXIFValue* cEXIFValueList::add(cEXIFTag* lpEXIFTag)
{
	cEXIFValue*	lpNew	= find(lpEXIFTag);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFValue(lpEXIFTag);

	append(lpNew);
	return(lpNew);
}

cEXIFValue* cEXIFValueList::find(cEXIFTag* lpEXIFTag)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFValue*	lpValue	= at(x);
		if(lpValue->exifTag() == lpEXIFTag)
			return(lpValue);
	}
	return(nullptr);
}

cIPTCTag::cIPTCTag(const qint32& iTAGID, const QString& szTAGName, const qint32& iTypeID, const QString& szDescription) :
	m_iTAGID(iTAGID),
	m_szTAGName(szTAGName),
	m_iTypeID(iTypeID),
	m_szDescription(szDescription)
{
}

cIPTCTagList::cIPTCTagList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
//	add(0x000b, QObject::tr("ProcessingSoftware"), 1, 2, QObject::tr("The name and version of the software used to post-process the picture."));
}

cIPTCTag* cIPTCTagList::add(const qint32& iTAGID, const QString& szTAGName, const qint32& iTypeID, const QString& szDescription)
{
	cIPTCTag*	lpNew	= find(iTAGID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cIPTCTag(iTAGID, szTAGName, iTypeID, szDescription);
	append(lpNew);
	return(lpNew);
}

cIPTCTag* cIPTCTagList::find(const qint32& iTAGID)
{
	for(int x = 0;x < count();x++)
	{
		cIPTCTag*	lpTag	= at(x);

		if(lpTag->m_iTAGID == iTAGID)
			return(lpTag);
	}
	return(nullptr);
}

cIPTCTag* cIPTCValue::iptcTag()
{
	return(m_lpIPTCTag);
}

cIPTCValue::cIPTCValue(cIPTCTag* lpIPTCTag) :
	m_lpIPTCTag(lpIPTCTag)
{
}

QVariant cIPTCValue::convertValue(const QString& szValue, qint32 iTypeId)
{
	QVariant	variant;

	switch(iTypeId)
	{
	case 1:	//byte
	case 3: //short
	case 4: //long
	case 8: //signed short
		variant = QVariant::fromValue(szValue.toLong());
		break;
	case 5: //rational
	case 10: //signed rational
	case 11: //float
		if(szValue.contains("/"))
		{
			QStringList	tmp	= szValue.split("/");
			variant = QVariant::fromValue(tmp[0].toDouble()/tmp[1].toDouble());
		}
		else
			variant	= QVariant::fromValue(szValue.toDouble());
		break;
	default:
		variant	= szValue;
	}
	return(variant);
}

void cIPTCValue::setValue(const QString& szValue, qint32 iTypeId, qint32 iCount)
{
	if(iCount == 1 || iTypeId == 2 || iTypeId == 7)
	{
		m_valueList.append(convertValue(szValue, iTypeId));
		return;
	}

	QStringList	valueList	= szValue.split(" ");

	if(valueList.count() != iCount)
		return;

	for(int x = 0;x < iCount;x++)
		m_valueList.append(convertValue(valueList[x], iTypeId));
}

QVariant cIPTCValue::value()
{
	if(m_valueList.count())
		return(m_valueList[0]);

	return(QVariant());
}

QList<QVariant> cIPTCValue::valueList()
{
	return(m_valueList);
}

cIPTCValueList::cIPTCValueList()
{
}

cIPTCValue* cIPTCValueList::add(cIPTCTag* lpIPTCTag)
{
	cIPTCValue*	lpNew	= find(lpIPTCTag);

	if(lpNew)
		return(nullptr);

	lpNew	= new cIPTCValue(lpIPTCTag);

	append(lpNew);
	return(lpNew);
}

cIPTCValue* cIPTCValueList::find(cIPTCTag* lpIPTCTag)
{
	for(int x = 0;x < count();x++)
	{
		cIPTCValue*	lpValue	= at(x);
		if(lpValue->iptcTag() == lpIPTCTag)
			return(lpValue);
	}
	return(nullptr);
}

cXMPTag::cXMPTag(const QString& szTAGName, const qint32& iTypeID, const QString& szDescription) :
	m_szTAGName(szTAGName),
	m_iTypeID(iTypeID),
	m_szDescription(szDescription)
{
}

cXMPTagList::cXMPTagList(QSqlDatabase* db) :
	m_dbExportDigikam(db)
{
	QSqlQuery	query(*m_dbExportDigikam);

	query.prepare("SELECT tag, type, description FROM xmpTags;");
	if(!query.exec())
		return;

	while(query.next())
		add(query.value("tag").toString(), query.value("type").toInt(), query.value("description").toString());
}

cXMPTag* cXMPTagList::add(const QString& szTAGName, const qint32& iTypeID, const QString& szDescription)
{
	cXMPTag*	lpNew	= find(szTAGName);

	if(lpNew)
		return(nullptr);

	lpNew	= new cXMPTag(szTAGName, iTypeID, szDescription);
	append(lpNew);
	return(lpNew);
}

cXMPTag* cXMPTagList::find(const QString& szTAGName)
{
	for(int x = 0;x < count();x++)
	{
		cXMPTag*	lpTag	= at(x);

		if(lpTag->m_szTAGName == szTAGName)
			return(lpTag);
	}
	return(nullptr);
}

cXMPTag* cXMPValue::xmpTag()
{
	return(m_lpXMPTag);
}

cXMPValue::cXMPValue(cXMPTag* lpXMPTag) :
	m_lpXMPTag(lpXMPTag)
{
}

QVariant cXMPValue::convertValue(const QString& szValue, qint32 iTypeId)
{
	QVariant	variant;

	switch(iTypeId)
	{
	case 1:	//byte
	case 3: //short
	case 4: //long
	case 8: //signed short
		variant = QVariant::fromValue(szValue.toLong());
		break;
	case 5: //rational
	case 10: //signed rational
	case 11: //float
		if(szValue.contains("/"))
		{
			QStringList	tmp	= szValue.split("/");
			variant = QVariant::fromValue(tmp[0].toDouble()/tmp[1].toDouble());
		}
		else
			variant	= QVariant::fromValue(szValue.toDouble());
		break;
	default:
		variant	= szValue;
	}
	return(variant);
}

void cXMPValue::setValue(const QString& szValue, qint32 iTypeId, qint32 /*iCount*/)
{
	m_valueList.append(convertValue(szValue, iTypeId));
}

QVariant cXMPValue::value()
{
	if(m_valueList.count())
		return(m_valueList[0]);

	return(QVariant());
}

QList<QVariant> cXMPValue::valueList()
{
	return(m_valueList);
}

cXMPValueList::cXMPValueList()
{
}

cXMPValue* cXMPValueList::add(cXMPTag* lpXMPTag)
{
	cXMPValue*	lpNew	= find(lpXMPTag);

	if(lpNew)
		return(nullptr);

	lpNew	= new cXMPValue(lpXMPTag);

	append(lpNew);
	return(lpNew);
}

cXMPValue* cXMPValueList::find(cXMPTag* lpXMPTag)
{
	for(int x = 0;x < count();x++)
	{
		cXMPValue*	lpValue	= at(x);
		if(lpValue->xmpTag() == lpXMPTag)
			return(lpValue);
	}
	return(nullptr);
}
