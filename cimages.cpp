#include "cimages.h"
#include "calbums.h"

#include <QPainter>
#include <QSqlQuery>

#define THUMBNAIL_WIDTH		160
#define THUMBNAIL_HEIGHT	120


cImages::cImages(const qint32& id, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albums(nullptr),
	m_name(""),
	m_status(0),
	m_category(0),
	m_modificationDate(QDateTime()),
	m_fileSize(0),
	m_uniqueHash(""),
	m_manualOrder(0),
	m_item(nullptr),
	m_thumbnail(nullptr)
{
}

cImages::cImages(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, QSqlDatabase *dbThumbnail, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_albums(albums),
	m_name(name),
	m_status(status),
	m_category(category),
	m_modificationDate(modificationDate),
	m_fileSize(fileSize),
	m_uniqueHash(uniqueHash),
	m_manualOrder(manualOrder),
	m_dbThumbnail(dbThumbnail),
	m_item(nullptr),
	m_thumbnail(nullptr)
{
}

qint32 cImages::id()
{
	return(m_id);
}

void cImages::setAlbums(cAlbums* albums)
{
	m_albums	= albums;
}

cAlbums* cImages::albums()
{
	return(m_albums);
}

void cImages::setName(const QString& name)
{
	m_name	= name;
}

QString cImages::name()
{
	return(m_name);
}

void cImages::setStatus(qint8& status)
{
	m_status	= status;
}

qint8 cImages::status()
{
	return(m_status);
}

void cImages::setCategory(qint8& category)
{
	m_category	= category;
}

qint8 cImages::category()
{
	return(m_category);
}

void cImages::setModificationDate(const QDateTime modificationDate)
{
	m_modificationDate	= modificationDate;
}

QDateTime cImages::modificationDate()
{
	return(m_modificationDate);
}

void cImages::setFileSize(qint32& fileSize)
{
	m_fileSize	= fileSize;
}

qint32 cImages::fileSize()
{
	return(m_fileSize);
}

void cImages::setUniqueHash(const QString& uniqueHash)
{
	m_uniqueHash	= uniqueHash;
}

QString cImages::uniqueHash()
{
	return(m_uniqueHash);
}

void cImages::setManualOrder(qint32& manualOrder)
{
	m_manualOrder	= manualOrder;
}

qint32 cImages::manualOrder()
{
	return(m_manualOrder);
}

void cImages::setItem(QStandardItem* item)
{
	m_item	= item;
}

QStandardItem* cImages::item()
{
	return(m_item);
}

void cImages::loadThumbnail()
{
	if(!m_dbThumbnail)
		return;

	QSqlQuery	query(*m_dbThumbnail);
	QString		sql	= QString("SELECT		t.id id, "
							  "				t.type type, "
							  "				t.modificationDate modificationDate, "
							  "				t.orientationHint orientationHint, "
							  "				t.data data "
							  "FROM			Thumbnails t "
							  "JOIN			UniqueHashes h ON (t.id = h.thumbId) "
							  "WHERE		h.uniqueHash = '%1';").arg(m_uniqueHash);

	if(!query.exec(sql))
	{
		qDebug() << "cImages: Thumbnail Loading failed.";
		return;
	}

	if(m_thumbnail)
		delete m_thumbnail;

	cImage	thumb;

	if(query.first())
	{
		if(!query.value("data").isNull())
			thumb.loadPGF(query.value("data").toByteArray());

		if(thumb.isNull())
		{
			m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, QImage::Format_RGB16);

			QPainter	painter(m_thumbnail);
			painter.setPen(QPen(Qt::black));
			painter.setBrush(QBrush(Qt::white));
			painter.drawRect(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
			painter.drawLine(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
			painter.drawLine(0, THUMBNAIL_HEIGHT-1, THUMBNAIL_WIDTH-1, 0);
			painter.end();
		}
		else if(thumb.width() != THUMBNAIL_WIDTH || thumb.height() != THUMBNAIL_HEIGHT)
		{
			m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, thumb.format());
			m_thumbnail->fill(Qt::black);

			qreal	sw	= THUMBNAIL_WIDTH/(qreal)thumb.width();
			qreal	sh	= THUMBNAIL_HEIGHT/(qreal)thumb.height();

			qreal	w;
			qreal	h;

			if(sw < sh)
			{
				w	= (qint32)((qreal)thumb.width()*sw);
				h	= (qint32)((qreal)thumb.height()*sw);
			}
			else
			{
				w	= (qint32)((qreal)thumb.width()*sh);
				h	= (qint32)((qreal)thumb.height()*sh);
			}

			QPainter	painter(m_thumbnail);
			painter.drawImage((THUMBNAIL_WIDTH-w)/2, (THUMBNAIL_HEIGHT-h)/2, thumb.scaled(w, h, Qt::KeepAspectRatio));
			painter.end();
		}
		else
		{
			m_thumbnail		= new cImage;
			*m_thumbnail	= thumb;
		}
	}
	else
	{
		m_thumbnail	= new cImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, QImage::Format_RGB16);

		QPainter	painter(m_thumbnail);
		painter.setPen(QPen(Qt::black));
		painter.setBrush(QBrush(Qt::white));
		painter.drawRect(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
		painter.drawLine(0, 0, THUMBNAIL_WIDTH-1, THUMBNAIL_HEIGHT-1);
		painter.drawLine(0, THUMBNAIL_HEIGHT-1, THUMBNAIL_WIDTH-1, 0);
		painter.end();
	}
}

QImage* cImages::thumbnail()
{
	return(m_thumbnail);
}

void cImages::setImageComment(const IMAGECOMMENT& ImageComment)
{
	m_ImageComment	= ImageComment;
}

IMAGECOMMENT cImages::imageComment()
{
	return(m_ImageComment);
}

void cImages::setImageCopyright(const IMAGECOPYRIGHT& ImageCopyright)
{
	m_ImageCopyright	= ImageCopyright;
}

IMAGECOPYRIGHT cImages::imageCopyright()
{
	return(m_ImageCopyright);
}

void cImages::setImageInformation(const IMAGEINFORMATION& ImageInformation)
{
	m_ImageInformation	= ImageInformation;
}

IMAGEINFORMATION cImages::imageInformation()
{
	return(m_ImageInformation);
}

void cImages::setImageMetadata(const IMAGEMETADATA& ImageMetadata)
{
	m_ImageMetadata	= ImageMetadata;
}

IMAGEMETADATA cImages::imageMetadata()
{
	return(m_ImageMetadata);
}

void cImages::setImagePositions(const IMAGEPOSITIONS& ImagePositions)
{
	m_ImagePositions	= ImagePositions;
}

IMAGEPOSITIONS cImages::imagePositions()
{
	return(m_ImagePositions);
}

void cImages::setImageProperties(const IMAGEPROPERTIES& ImageProperties)
{
	m_ImageProperties	= ImageProperties;
}

IMAGEPROPERTIES cImages::imageProperties()
{
	return(m_ImageProperties);
}

void cImages::setVideoMetadata(const VIDEOMETADATA& VideoMetadata)
{
	m_VideoMetadata	= VideoMetadata;
}

VIDEOMETADATA cImages::videoMetadata()
{
	return(m_VideoMetadata);
}

cImagesList::cImagesList(QSqlDatabase* dbDigikam, QSqlDatabase *dbThumbnail, cAlbumsList* albumsList, QObject* parent) :
	QObject(parent),
	m_dbDigikam(dbDigikam),
	m_dbThumbnail(dbThumbnail),
	m_albumsList(albumsList)
{
}

bool cImagesList::load(bool loadThumbnails, cAlbums* albums)
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);
	QString		sql	= QString("SELECT		i.id Image_id, "
							  "				i.album Image_album, "
							  "				i.name Image_name, "
							  "				i.status Image_status, "
							  "				i.category Image_category, "
							  "				i.modificationDate Image_modificationDate, "
							  "				i.fileSize Image_fileSize, "
							  "				i.uniqueHash Image_uniqueHash, "
							  "				i.manualOrder Image_manualOrder, "
							  "				icom.id ImageComment_id, "
							  "				icom.imageid ImageComment_imageid, "
							  "				icom.type ImageComment_type, "
							  "				icom.language ImageComment_language, "
							  "				icom.author ImageComment_author, "
							  "				icom.date ImageComment_date, "
							  "				icom.comment ImageComment_comment, "
							  "				icop.id ImageCopyright_id, "
							  "				icop.imageid ImageCopyright_imageid, "
							  "				icop.property ImageCopyright_property, "
							  "				icop.value ImageCopyright_value, "
							  "				icop.extraValue ImageCopyright_extraValue, "
							  "				iinfo.imageid ImageInformation_imageid, "
							  "				iinfo.rating ImageInformation_rating, "
							  "				iinfo.creationDate ImageInformation_creationDate, "
							  "				iinfo.digitizationDate ImageInformation_digitizationDate, "
							  "				iinfo.orientation ImageInformation_orientation, "
							  "				iinfo.width ImageInformation_width, "
							  "				iinfo.height ImageInformation_height, "
							  "				iinfo.format ImageInformation_format, "
							  "				iinfo.colorDepth ImageInformation_colorDepth, "
							  "				iinfo.colorModel ImageInformation_colorModel, "
							  "				imeta.imageid ImageMetadata_imageid, "
							  "				imeta.make ImageMetadata_make, "
							  "				imeta.model ImageMetadata_model, "
							  "				imeta.lens ImageMetadata_lens, "
							  "				imeta.aperture ImageMetadata_aparture, "
							  "				imeta.focalLength ImageMetadata_focalLength, "
							  "				imeta.focalLength35 ImageMetadata_focalLength35, "
							  "				imeta.exposureTime ImageMetadata_exposureTime, "
							  "				imeta.exposureProgram ImageMetadata_exposureProgram, "
							  "				imeta.exposureMode ImageMetadata_exposureMode, "
							  "				imeta.sensitivity ImageMetadata_sensitivity, "
							  "				imeta.flash ImageMetadata_flash, "
							  "				imeta.whiteBalance ImageMetadata_whiteBalance, "
							  "				imeta.whiteBalanceColorTemperature ImageMetadata_whiteBalanceColorTemperature, "
							  "				imeta.meteringMode ImageMetadata_meteringMode, "
							  "				imeta.subjectDistance ImageMetadata_subjectDistance, "
							  "				imeta.subjectDistanceCategory ImageMetadata_subjectDistanceCategory, "
							  "				ipos.imageid ImagePositions_imageid, "
							  "				ipos.latitude ImagePositions_latitude, "
							  "				ipos.latitudeNumber ImagePositions_latitideNumber, "
							  "				ipos.longitude ImagePositions_longitude, "
							  "				ipos.longitudeNumber ImagePositions_longitudeNumber, "
							  "				ipos.altitude ImagePositions_altitude, "
							  "				ipos.orientation ImagePositions_orientation, "
							  "				ipos.tilt ImagePositions_tilt, "
							  "				ipos.roll ImagePositions_roll, "
							  "				ipos.accuracy ImagePositions_accuracy, "
							  "				ipos.description ImagePositions_description, "
							  "				iprop.imageid ImageProperties_imageid, "
							  "				iprop.property ImageProperties_property, "
							  "				iprop.value ImageProperties_value, "
							  "				vmeta.imageid VideoMetadata_imageid, "
							  "				vmeta.aspectRatio VideoMetadata_aspectRation, "
							  "				vmeta.audioBitRate VideoMetadata_audioBitRate, "
							  "				vmeta.audioChannelType VideoMetadata_audioChannelType, "
							  "				vmeta.audioCompressor VideoMetadata_audioCompressor, "
							  "				vmeta.duration VideoMetadata_duration, "
							  "				vmeta.frameRate VideoMetadata_frameRate, "
							  "				vmeta.exposureProgram VideoMetadata_exposureProgram, "
							  "				vmeta.videoCodec VideoMetadata_videoCodec "
							  "FROM			Images i "
							  "LEFT JOIN	ImageComments icom ON (i.id = icom.imageid) "
							  "LEFT JOIN	ImageCopyright icop ON (i.id = icop.imageid) "
							  "LEFT JOIN	ImageInformation iinfo ON (i.id = iinfo.imageid) "
							  "LEFT JOIN	ImageMetadata imeta ON (i.id = imeta.imageid) "
							  "LEFT JOIN	ImagePositions ipos ON (i.id = ipos.imageid) "
							  "LEFT JOIN	ImageProperties iprop ON (i.id = iprop.imageid) "
							  "LEFT JOIN	VideoMetadata vmeta ON (i.id = vmeta.imageid) "
							  "WHERE		album = %1 "
							  "ORDER BY	name;").arg(albums->id());
	if(!query.exec(sql))
	{
		qDebug() << "Images: loading failed.";
		return(false);
	}

	while(query.next())
	{
		cImages*			newImages	= add(query.value("Image_id").toInt(), albums, query.value("Image_name").toString(), query.value("Image_status").toInt(), query.value("Image_category").toInt(), query.value("Image_modificationDate").toDateTime(), query.value("Image_fileSize").toInt(), query.value("Image_uniqueHash").toString(), query.value("Image_manualOrder").toInt(), loadThumbnails, this);
		IMAGECOMMENT		ImageComment;
		IMAGECOPYRIGHT		ImageCopyright;
		IMAGEINFORMATION	ImageInformation;
		IMAGEMETADATA		ImageMetadata;
		IMAGEPOSITIONS		ImagePositions;
		IMAGEPROPERTIES		ImageProperties;
		VIDEOMETADATA		VideoMetadata;

		ImageComment.id								= query.value("ImageComment_id").toInt();
		ImageComment.imageid						= query.value("ImageComment_imageid").toInt();
		ImageComment.type							= query.value("ImageComment_type").toInt();
		ImageComment.language						= query.value("ImageComment_language").toString();
		ImageComment.author							= query.value("ImageComment_author").toString();
		ImageComment.date							= query.value("ImageComment_date").toDateTime();
		ImageComment.comment						= query.value("ImageComment_comment").toString();
		ImageCopyright.id							= query.value("ImageCopyright_id").toInt();
		ImageCopyright.imageid						= query.value("ImageCopyright_imageid").toInt();
		ImageCopyright.property						= query.value("ImageCopyright_property").toString();
		ImageCopyright.value						= query.value("ImageCopyright_value").toString();
		ImageCopyright.extraValue					= query.value("ImageCopyright_extraValue").toString();
		ImageInformation.imageid					= query.value("ImageInformation_imageid").toInt();
		ImageInformation.rating						= query.value("ImageInformation_rating").toInt();
		ImageInformation.creationDate				= query.value("ImageInformation_creationDate").toDateTime();
		ImageInformation.digitizationDate			= query.value("ImageInformation_digitizationDate").toDateTime();
		ImageInformation.orientation				= query.value("ImageInformation_orientation").toInt();
		ImageInformation.width						= query.value("ImageInformation_width").toInt();
		ImageInformation.height						= query.value("ImageInformation_height").toInt();
		ImageInformation.format						= query.value("ImageInformation_format").toString();
		ImageInformation.colorDepth					= query.value("ImageInformation_colorDepth").toInt();
		ImageInformation.colorModel					= query.value("ImageInformation_colorModel").toInt();
		ImageMetadata.imageid						= query.value("ImageMetadata_imageid").toInt();
		ImageMetadata.make							= query.value("ImageMetadata_make").toString();
		ImageMetadata.model							= query.value("ImageMetadata_model").toString();
		ImageMetadata.lens							= query.value("ImageMetadata_lens").toString();
		ImageMetadata.aparture						= query.value("ImageMetadata_aparture").toReal();
		ImageMetadata.focalLength					= query.value("ImageMetadata_focalLength").toReal();
		ImageMetadata.focalLength35					= query.value("ImageMetadata_focalLength35").toReal();
		ImageMetadata.exposureTime					= query.value("ImageMetadata_exposureTime").toReal();
		ImageMetadata.exposureProgram				= query.value("ImageMetadata_exposureProgram").toInt();
		ImageMetadata.exposureMode					= query.value("ImageMetadata_exposureMode").toInt();
		ImageMetadata.sensitivity					= query.value("ImageMetadata_sensitivity").toInt();
		ImageMetadata.flash							= query.value("ImageMetadata_flash").toInt();
		ImageMetadata.whiteBalance					= query.value("ImageMetadata_whiteBalance").toInt();
		ImageMetadata.whiteBalanceColorTemperature	= query.value("ImageMetadata_whiteBalanceColorTemperature").toInt();
		ImageMetadata.meteringMode					= query.value("ImageMetadata_meteringMode").toInt();
		ImageMetadata.subjectDistance				= query.value("ImageMetadata_subjectDistance").toReal();
		ImageMetadata.subjectDistanceCategory		= query.value("ImageMetadata_subjectDistanceCategory").toInt();
		ImagePositions.imageid						= query.value("ImagePositions_imageid").toInt();
		ImagePositions.latitude						= query.value("ImagePositions_latitude").toString();
		ImagePositions.latitideNumber				= query.value("ImagePositions_latitideNumber").toReal();
		ImagePositions.longitude					= query.value("ImagePositions_longitude").toString();
		ImagePositions.longitudeNumber				= query.value("ImagePositions_longitudeNumber").toReal();
		ImagePositions.altitude						= query.value("ImagePositions_altitude").toReal();
		ImagePositions.orientation					= query.value("ImagePositions_orientation").toReal();
		ImagePositions.tilt							= query.value("ImagePositions_tilt").toReal();
		ImagePositions.roll							= query.value("ImagePositions_roll").toReal();
		ImagePositions.accuracy						= query.value("ImagePositions_accuracy").toReal();
		ImagePositions.description					= query.value("ImagePositions_description").toString();
		ImageProperties.imageid						= query.value("ImageProperties_imageid").toInt();
		ImageProperties.property					= query.value("ImageProperties_property").toString();
		ImageProperties.value						= query.value("ImageProperties_value").toString();
		VideoMetadata.imageid						= query.value("VideoMetadata_imageid").toInt();
		VideoMetadata.aspectRation					= query.value("VideoMetadata_aspectRation").toString();
		VideoMetadata.audioBitRate					= query.value("VideoMetadata_audioBitRate").toString();
		VideoMetadata.audioChannelType				= query.value("VideoMetadata_audioChannelType").toString();
		VideoMetadata.audioCompressor				= query.value("VideoMetadata_audioCompressor").toString();
		VideoMetadata.duration						= query.value("VideoMetadata_duration").toString();
		VideoMetadata.frameRate						= query.value("VideoMetadata_frameRate").toString();
		VideoMetadata.exposureProgram				= query.value("VideoMetadata_exposureProgram").toInt();
		VideoMetadata.videoCodec					= query.value("VideoMetadata_videoCodec").toString();

		newImages->setImageComment(ImageComment);
		newImages->setImageCopyright(ImageCopyright);
		newImages->setImageInformation(ImageInformation);
		newImages->setImageMetadata(ImageMetadata);
		newImages->setImagePositions(ImagePositions);
		newImages->setImageProperties(ImageProperties);
		newImages->setVideoMetadata(VideoMetadata);
	}

	return(true);
}

bool cImagesList::load(bool loadThumbnails, const qint32 &albumsID)
{
	if(!m_dbDigikam)
		return(false);

	QSqlQuery	query(*m_dbDigikam);
	QString		sql	= QString("SELECT	id, "
							  "			album, "
							  "			name, "
							  "			status, "
							  "			category, "
							  "			modificationDate, "
							  "			fileSize, "
							  "			uniqueHash, "
							  "			manualOrder "
							  "FROM		Images "
							  "WHERE	album = %1 "
							  "ORDER BY	name;").arg(albumsID);
	if(!query.exec(sql))
	{
		qDebug() << "Images: loading failed.";
		return(false);
	}

	while(query.next())
		add(query.value("id").toInt(), query.value("album").toInt(), query.value("name").toString(), query.value("status").toInt(), query.value("category").toInt(), query.value("modificationDate").toDateTime(), query.value("fileSize").toInt(), query.value("uniqueHash").toString(), query.value("manualOrder").toInt(), loadThumbnails, this);

	return(true);
}

bool cImagesList::loadThumbnails()
{
	for(int i = 0;i < count();i++)
	{
		cImages*	images	= at(i);
		if(!images->thumbnail())
		{
			images->loadThumbnail();
			images->item()->setIcon(QIcon(QPixmap::fromImage(*images->thumbnail())));
		}
	}
	return(true);
}

cImages* cImagesList::add(const qint32& id, QObject* parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cImages(id, parent);

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::add(const qint32& id, cAlbums* albums, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, bool loadThumbnails, QObject *parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	lpNew	= new cImages(id, albums, name, status, category, modificationDate, fileSize, uniqueHash, manualOrder, m_dbThumbnail, parent);
	QStandardItem*	item	= new QStandardItem(name);
	item->setTextAlignment(Qt::AlignCenter);
	item->setData(QVariant::fromValue(lpNew));
	item->setCheckable(true);
	lpNew->setItem(item);

	if(loadThumbnails)
	{
		lpNew->loadThumbnail();
		item->setIcon(QIcon(QPixmap::fromImage(*lpNew->thumbnail())));
	}

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::add(const qint32& id, const qint32& albumsID, const QString& name, const qint8& status, const qint8& category, const QDateTime& modificationDate, const qint32& fileSize, const QString& uniqueHash, const qint32& manualOrder, bool loadThumbnails, QObject *parent)
{
	cImages*	lpNew	= find(id);
	if(lpNew)
		return(lpNew);

	cAlbums*	albums	= m_albumsList->find(albumsID);
	lpNew	= new cImages(id, albums, name, status, category, modificationDate, fileSize, uniqueHash, manualOrder, m_dbThumbnail, parent);
	QStandardItem*	item	= new QStandardItem(name);
	item->setTextAlignment(Qt::AlignCenter);
	item->setData(QVariant::fromValue(lpNew));
	item->setCheckable(true);
	lpNew->setItem(item);

	if(loadThumbnails)
	{
		lpNew->loadThumbnail();
		item->setIcon(QIcon(QPixmap::fromImage(*lpNew->thumbnail())));
	}

	append(lpNew);
	return(lpNew);
}

cImages* cImagesList::find(const qint32& id)
{
	for(int i = 0;i < count();i++)
	{
		if(at(i)->id() == id)
			return(at(i));
	}
	return(nullptr);
}
