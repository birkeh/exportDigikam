/*!
 \file cimage.cpp

*/

#include "cimage.h"

#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc.hpp"

#include <QTransform>
#include <QFileInfo>

#include <QDebug>
#include <QElapsedTimer>


cImage::cImage() :
	QImage(),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(const QSize &size, QImage::Format format) :
	QImage(size, format),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(int width, int height, QImage::Format format) :
	QImage(width, height, format),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(uchar *data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo) :
	QImage(data, width, height, format, cleanupFunction, cleanupInfo),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(const uchar *data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo) :
	QImage(data, width, height, format, cleanupFunction, cleanupInfo),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(uchar *data, int width, int height, int bytesPerLine, QImage::Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo) :
	QImage(data, width, height, bytesPerLine, format, cleanupFunction, cleanupInfo),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(const uchar *data, int width, int height, int bytesPerLine, QImage::Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo) :
	QImage(data, width, height, bytesPerLine, format, cleanupFunction, cleanupInfo),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(const QString &fileName, const char *format) :
	QImage(),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
	load(fileName, format);
}

cImage::cImage(const QImage &image) :
	QImage(image),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

cImage::cImage(QImage &&other) :
	QImage(other),
	m_isChromatic(true),
	m_camType(camera_unknown),
	m_raw(false)
{
}

bool cImage::load(const QString &fileName, const char *format)
{
	QFileInfo	fileInfo(fileName);

	if(!fileInfo.suffix().compare("NEF", Qt::CaseInsensitive))
		return(loadRAW(fileName));

	if(fileName.contains(".pgf", Qt::CaseInsensitive))
		return(loadPGF(fileName));
	if(QImage::load(fileName, format))
		return(true);

	return(loadRAW(fileName));
}

bool cImage::openBuffer(const QString &fileName, const QScopedPointer<QByteArray>& ba, LibRaw& iProcessor)
{
	int			error	= LIBRAW_DATA_ERROR;
	QFileInfo	fi(fileName);

	if(fi.suffix().contains("iiq", Qt::CaseInsensitive) || !ba || ba->isEmpty())
		error	= iProcessor.open_file(fileName.toStdString().c_str());
	else
	{
		if(ba->isEmpty() || ba->size() < 100)
			return(false);

		error	= iProcessor.open_buffer((void*)ba->constData(), static_cast<size_t>(ba->size()));
	}

	return (error == LIBRAW_SUCCESS);
}

void cImage::detectSpecialCamera(const LibRaw & iProcessor)
{
	if(QString(iProcessor.imgdata.idata.model) == "IQ260 Achromatic")
		m_isChromatic = false;

	if (QString(iProcessor.imgdata.idata.model).contains("IQ260"))
		m_camType = camera_iiq;
	else if (!QString(iProcessor.imgdata.idata.make).compare("Canon", Qt::CaseInsensitive))
		m_camType = camera_canon;
}

cv::Mat cImage::demosaic(LibRaw & iProcessor)
{
	cv::Mat		rawMat			= cv::Mat(iProcessor.imgdata.sizes.height, iProcessor.imgdata.sizes.width, CV_16UC1);
	double		dynamicRange	= static_cast<double>(iProcessor.imgdata.color.maximum - iProcessor.imgdata.color.black);

	// normalize all image values
	for(int rIdx = 0; rIdx < rawMat.rows; rIdx++)
	{
		unsigned short *ptrRaw = rawMat.ptr<unsigned short>(rIdx);

		for (int cIdx = 0; cIdx < rawMat.cols; cIdx++)
		{
			int		colIdx	= iProcessor.COLOR(rIdx, cIdx);
			double	val		= static_cast<double>(iProcessor.imgdata.image[(rawMat.cols*rIdx) + cIdx][colIdx]);

			// normalize the value w.r.t the black point defined
			val				= (val - iProcessor.imgdata.color.black) / dynamicRange;
			ptrRaw[cIdx]	= clip<unsigned short>(val * USHRT_MAX);  // for conversion to 16U
		}
	}

	// no demosaicing
	if(m_isChromatic)
	{
		unsigned long	type	= static_cast<unsigned long>(iProcessor.imgdata.idata.filters);
		type	= type & 255;

		cv::Mat			rgbImg;

		//define bayer pattern
		if(type == 180)
			cvtColor(rawMat, rgbImg, CV_BayerBG2RGB);		//bitmask  10 11 01 00  -> 3(G) 2(B) 1(G) 0(R) ->	RG RG RG
		else if(type == 30)
			cvtColor(rawMat, rgbImg, CV_BayerRG2RGB);		//bitmask  00 01 11 10	-> 0 1 3 2
		else if(type == 225)
			cvtColor(rawMat, rgbImg, CV_BayerGB2RGB);		//bitmask  11 10 00 01
		else if(type == 75)
			cvtColor(rawMat, rgbImg, CV_BayerGR2RGB);		//bitmask  01 00 10 11
		else
		{
			qWarning() << "Wrong Bayer Pattern (not BG, RG, GB, GR)\n";
			return cv::Mat();
		}

		rawMat = rgbImg;
	}

	// 16U (1 or 3 channeled) Mat
	return(rawMat);
}

cv::Mat cImage::prepareImg(const LibRaw & iProcessor)
{
	cv::Mat rawMat			= cv::Mat(iProcessor.imgdata.sizes.height, iProcessor.imgdata.sizes.width, CV_16UC3, cv::Scalar(0));
	double	dynamicRange	= static_cast<double>(iProcessor.imgdata.color.maximum - iProcessor.imgdata.color.black);

	// normalization function
	auto normalize = [&](double val)
	{
		val = (val - iProcessor.imgdata.color.black) / dynamicRange;
		return clip<unsigned short>(val * USHRT_MAX);
	};

	for (int rIdx = 0; rIdx < rawMat.rows; rIdx++)
	{
		unsigned short *ptrI = rawMat.ptr<unsigned short>(rIdx);

		for (int cIdx = 0; cIdx < rawMat.cols; cIdx++)
		{

			*ptrI = normalize(iProcessor.imgdata.image[rawMat.cols*rIdx + cIdx][0]);
			ptrI++;
			*ptrI = normalize(iProcessor.imgdata.image[rawMat.cols*rIdx + cIdx][1]);
			ptrI++;
			*ptrI = normalize(iProcessor.imgdata.image[rawMat.cols*rIdx + cIdx][2]);
			ptrI++;
		}
	}

	return rawMat;
}

void cImage::whiteBalance(const LibRaw & iProcessor, cv::Mat & img)
{
	// white balance must not be empty at this point
	cv::Mat			wb	= whiteMultipliers(iProcessor);
	const float*	wbp	= wb.ptr<float>();
	assert(wb.cols == 4);

	for(int rIdx = 0; rIdx < img.rows; rIdx++)
	{
		unsigned short *ptr = img.ptr<unsigned short>(rIdx);

		for (int cIdx = 0; cIdx < img.cols; cIdx++)
		{
			//apply white balance correction
			unsigned short r = clip<unsigned short>(*ptr		* wbp[0]);
			unsigned short g = clip<unsigned short>(*(ptr+1)	* wbp[1]);
			unsigned short b = clip<unsigned short>(*(ptr+2)	* wbp[2]);

			//apply color correction
			int cr = qRound(iProcessor.imgdata.color.rgb_cam[0][0] * r +
							iProcessor.imgdata.color.rgb_cam[0][1] * g +
							iProcessor.imgdata.color.rgb_cam[0][2] * b);
			int cg = qRound(iProcessor.imgdata.color.rgb_cam[1][0] * r +
							iProcessor.imgdata.color.rgb_cam[1][1] * g +
							iProcessor.imgdata.color.rgb_cam[1][2] * b);
			int cb = qRound(iProcessor.imgdata.color.rgb_cam[2][0] * r +
							iProcessor.imgdata.color.rgb_cam[2][1] * g +
							iProcessor.imgdata.color.rgb_cam[2][2] * b);

			// clip & save color corrected values
			*ptr = clip<unsigned short>(cr);
			ptr++;
			*ptr = clip<unsigned short>(cg);
			ptr++;
			*ptr = clip<unsigned short>(cb);
			ptr++;
		}
	}
}

cv::Mat cImage::whiteMultipliers(const LibRaw & iProcessor)
{
	// get camera white balance multipliers
	cv::Mat	wm(1, 4, CV_32FC1);

	float*	wmp	= wm.ptr<float>();

	for(int idx = 0; idx < wm.cols; idx++)
		wmp[idx] = iProcessor.imgdata.color.cam_mul[idx];

	if(wmp[3] == 0.0f)
		wmp[3] = wmp[1];	// take green (usually its RGBG)

	// normalize white balance multipliers
	float	w		= static_cast<float>(cv::sum(wm)[0] / 4.0f);
	float	maxW	= 1.0f;

	//clipping according the camera model
	//if w > 2.0 maxW is 256, otherwise 512
	//tested empirically
	//check if it can be defined by some metadata settings?
	if(w > 2.0f)
		maxW	= 255.0f;
	if(w > 2.0f && m_camType == camera_canon)
		maxW	= 511.0f;	// some cameras would even need ~800 - why?

	//normalize white point
	wm	/= maxW;

	// 1 x 4 32FC1 white balance vector
	return wm;
}

void cImage::gammaCorrection(const LibRaw & iProcessor, cv::Mat& img)
{
	// white balance must not be empty at this point
	cv::Mat	gt	= gammaTable(iProcessor);
	const unsigned short* gammaLookup = gt.ptr<unsigned short>();
	assert(gt.cols == USHRT_MAX);

	for(int rIdx = 0; rIdx < img.rows; rIdx++)
	{
		unsigned short *ptr = img.ptr<unsigned short>(rIdx);

		for(int cIdx = 0; cIdx < img.cols * img.channels(); cIdx++)
		{
			// values close to 0 are treated linear
			if (ptr[cIdx] <= 5)	// 0.018 * 255
				ptr[cIdx] = static_cast<unsigned short>(qRound(ptr[cIdx] * static_cast<double>(iProcessor.imgdata.params.gamm[1]) / 255.0));
			else
				ptr[cIdx] = gammaLookup[ptr[cIdx]];
		}
	}

}

cv::Mat cImage::gammaTable(const LibRaw & iProcessor)
{
	// OK this is an instance of reverse engineering:
	// we found out that the values of (at least) the PhaseOne's achromatic back have to be doubled
	// our images are no close to what their software (Capture One does) - only the gamma correction
	// seems to be slightly different... -> now we can load compressed IIQs that are not supported by PS : )
	double cameraHackMlp = (QString(iProcessor.imgdata.idata.model) == "IQ260 Achromatic") ? 2.0 : 1.0;

	//read gamma value and create gamma table
	double gamma = static_cast<double>(iProcessor.imgdata.params.gamm[0]);

	cv::Mat gmt(1, USHRT_MAX, CV_16UC1);
	unsigned short* gmtp = gmt.ptr<unsigned short>();

	for (int idx = 0; idx < gmt.cols; idx++) {
		gmtp[idx] = clip<unsigned short>(qRound((1.099*std::pow(static_cast<double>(idx) / USHRT_MAX, gamma) - 0.099) * 255 * cameraHackMlp));
	}

	// a 1 x 65535 U16 gamma table
	return gmt;
}

QImage cImage::raw2Img(const LibRaw & iProcessor, cv::Mat & img)
{
	//check the pixel aspect ratio of the raw image
	if(iProcessor.imgdata.sizes.pixel_aspect != 1.0f)
		cv::resize(img, img, cv::Size(), static_cast<double>(iProcessor.imgdata.sizes.pixel_aspect), 1.0f);

	// revert back to 8-bit image
	img.convertTo(img, CV_8U);

	// TODO: for now - fix this!
	if(img.channels() == 1)
		cv::cvtColor(img, img, CV_GRAY2RGB);

	return(mat2QImage(img));
}

QImage cImage::mat2QImage(cv::Mat img)
{
	QImage qImg;

	// since Mat header is copied, a new buffer should be allocated (check this!)
	if(img.depth() == CV_32F)
		img.convertTo(img, CV_8U, 255);

	if(img.type() == CV_8UC1)
		qImg = QImage(img.data, static_cast<int>(img.cols), static_cast<int>(img.rows), static_cast<int>(img.step), QImage::Format_Indexed8);	// opencv uses size_t for scaling in x64 applications
	if(img.type() == CV_8UC3)
		qImg = QImage(img.data, static_cast<int>(img.cols), static_cast<int>(img.rows), static_cast<int>(img.step), QImage::Format_RGB888);
	if(img.type() == CV_8UC4)
		qImg = QImage(img.data, static_cast<int>(img.cols), static_cast<int>(img.rows), static_cast<int>(img.step), QImage::Format_ARGB32);

	qImg = qImg.copy();

	return qImg;
}

bool cImage::loadRAW(const QString &fileName)
{
	QScopedPointer<QByteArray>	ba;
	LibRaw						iProcessor;

	if(!openBuffer(fileName, ba, iProcessor))
	{
		qDebug() << "could not open buffer";
		return(false);
	}

	detectSpecialCamera(iProcessor);

	int	error = iProcessor.unpack();

	if(strcmp(iProcessor.version(), "0.13.5") != 0)	// fixes a bug specific to libraw 13 - version call is UNTESTED
		iProcessor.raw2image();

	if(error != LIBRAW_SUCCESS)
		return(false);

	cv::Mat	rawMat;

	if(iProcessor.imgdata.idata.filters)
		rawMat	= demosaic(iProcessor);
	else
		rawMat	= prepareImg(iProcessor);

	// color correction + white balance
	if(m_isChromatic)
		whiteBalance(iProcessor, rawMat);

	gammaCorrection(iProcessor, rawMat);

	// reduce color noise
//	if (DkSettingsManager::param().resources().filterRawImages && mIsChromatic)
//		reduceColorNoise(iProcessor, rawMat);

	*this	= raw2Img(iProcessor, rawMat);

	iProcessor.recycle();
	rawMat.release();

	m_raw	= true;

	return(true);
}

bool cImage::loadPGF(const QString &fileName)
{
	QFile		file(fileName);

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Cannot open PGF file to read...";
		return(false);
	}

	QImage		img;
	QByteArray	data(file.size(), '\x00');
	QDataStream	stream(&file);
	stream.readRawData(data.data(), data.size());

	if (!PGFUtils::readPGFImageData(data, img))
	{
		qDebug() << "loadPGFScaled failed...";
		return(false);
	}

	return(true);
}

bool cImage::isRaw()
{
	return(m_raw);
}
