/*!
 \file cimage.h

*/

#ifndef CIMAGE_H
#define CIMAGE_H


#include "libraw/libraw.h"
#include <opencv2/opencv.hpp>

#include <QImage>


/*!
 \brief

 \class cImage cimage.h "cimage.h"
*/
class cImage : public QImage
{
public:
	cImage();
	/*!
	 \brief

	 \fn cImage
	 \param size
	 \param format
	*/
	cImage(const QSize &size, QImage::Format format);
	/*!
	 \brief

	 \fn cImage
	 \param width
	 \param height
	 \param format
	*/
	cImage(int width, int height, QImage::Format format);
	/*!
	 \brief

	 \fn cImage
	 \param data
	 \param width
	 \param height
	 \param format
	 \param cleanupFunction
	 \param cleanupInfo
	*/
	cImage(uchar *data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);
	/*!
	 \brief

	 \fn cImage
	 \param data
	 \param width
	 \param height
	 \param format
	 \param cleanupFunction
	 \param cleanupInfo
	*/
	cImage(const uchar *data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);
	/*!
	 \brief

	 \fn cImage
	 \param data
	 \param width
	 \param height
	 \param bytesPerLine
	 \param format
	 \param cleanupFunction
	 \param cleanupInfo
	*/
	cImage(uchar *data, int width, int height, int bytesPerLine, QImage::Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);
	/*!
	 \brief

	 \fn cImage
	 \param data
	 \param width
	 \param height
	 \param bytesPerLine
	 \param format
	 \param cleanupFunction
	 \param cleanupInfo
	*/
	cImage(const uchar *data, int width, int height, int bytesPerLine, QImage::Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);
	/*!
	 \brief

	 \fn cImage
	 \param fileName
	 \param format
	*/
	cImage(const QString &fileName, const char *format = nullptr);
	/*!
	 \brief

	 \fn cImage
	 \param image
	*/
	cImage(const QImage &image);
	/*!
	 \brief

	 \fn cImage
	 \param other
	*/
	cImage(QImage &&other);

	/*!
	 \brief

	 \fn load
	 \param fileName
	 \param format
	 \return bool
	*/
	bool	load(const QString &fileName, const char *format = nullptr);

	/*!
	 \brief

	 \fn loadPGF
	 \param data
	 \return bool
	*/
	bool	loadPGF(const QByteArray& data);

	/*!
	 \brief

	 \fn isRaw
	 \return bool
	*/
	bool	isRaw();

protected:
	/*!
	 \brief

	 \enum Cam
	*/
	enum Cam
	{
		camera_unknown	= 0,
		camera_iiq,
		camera_canon,
		camera_end
	};

	template <typename num>
	/*!
	 \brief

	 \fn clip
	 \param val
	 \return num
	*/
	num clip(float val) const
	{
		int vr = qRound(val);

		// trust me I'm an engineer @ -2
		// with -2 we do not get pink in oversaturated areas
		if (vr > std::numeric_limits<num>::max())
			vr = std::numeric_limits<num>::max()-2;
		if (vr < 0)
			vr = 0;

		return static_cast<num>(vr);
	}

	template <typename num>
	/*!
	 \brief

	 \fn clip
	 \param val
	 \return num
	*/
	num clip(double val) const
	{
		int vr = qRound(val);

		// trust me I'm an engineer @ -2
		// with -2 we do not get pink in oversaturated areas
		if (vr > std::numeric_limits<num>::max())
			vr = std::numeric_limits<num>::max()-2;
		if (vr < 0)
			vr = 0;

		return static_cast<num>(vr);
	}

	template <typename num>
	/*!
	 \brief

	 \fn clip
	 \param val
	 \return num
	*/
	num clip(int val) const
	{
		int vr = qRound(static_cast<double>(val));

		// trust me I'm an engineer @ -2
		// with -2 we do not get pink in oversaturated areas
		if (vr > std::numeric_limits<num>::max())
			vr = std::numeric_limits<num>::max()-2;
		if (vr < 0)
			vr = 0;

		return static_cast<num>(vr);
	}

private:
	bool	m_isChromatic;		/*!< TODO: describe */
	Cam		m_camType;			/*!< TODO: describe */
	bool	m_raw;				/*!< TODO: describe */

	/*!
	 \brief

	 \fn loadRAW
	 \param fileName
	 \return bool
	*/
	bool	loadRAW(const QString &fileName);

	/*!
	 \brief

	 \fn loadPGF
	 \param fileName
	 \return bool
	*/
	bool	loadPGF(const QString &fileName);

	/*!
	 \brief

	 \fn openBuffer
	 \param fileName
	 \param ba
	 \param iProcessor
	 \return bool
	*/
	bool	openBuffer(const QString &fileName, const QScopedPointer<QByteArray>& ba, LibRaw& iProcessor);
	/*!
	 \brief

	 \fn detectSpecialCamera
	 \param iProcessor
	*/
	void	detectSpecialCamera(const LibRaw & iProcessor);

	/*!
	 \brief

	 \fn demosaic
	 \param iProcessor
	 \return cv::Mat
	*/
	cv::Mat	demosaic(LibRaw & iProcessor);
	/*!
	 \brief

	 \fn prepareImg
	 \param iProcessor
	 \return cv::Mat
	*/
	cv::Mat	prepareImg(const LibRaw & iProcessor);
	/*!
	 \brief

	 \fn whiteBalance
	 \param iProcessor
	 \param img
	*/
	void	whiteBalance(const LibRaw & iProcessor, cv::Mat & img);
	/*!
	 \brief

	 \fn whiteMultipliers
	 \param iProcessor
	 \return cv::Mat
	*/
	cv::Mat	whiteMultipliers(const LibRaw & iProcessor);
	/*!
	 \brief

	 \fn gammaCorrection
	 \param iProcessor
	 \param img
	*/
	void	gammaCorrection(const LibRaw & iProcessor, cv::Mat& img);
	/*!
	 \brief

	 \fn gammaTable
	 \param iProcessor
	 \return cv::Mat
	*/
	cv::Mat	gammaTable(const LibRaw & iProcessor);

	/*!
	 \brief

	 \fn raw2Img
	 \param iProcessor
	 \param img
	 \return QImage
	*/
	QImage	raw2Img(const LibRaw & iProcessor, cv::Mat & img);
	/*!
	 \brief

	 \fn mat2QImage
	 \param img
	 \return QImage
	*/
	QImage	mat2QImage(cv::Mat img);
};

#endif // CIMAGE_H
