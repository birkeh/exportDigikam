/*!
 \file common.cpp

*/

#include "common.h"


QString generateReadList(const QList<IMAGEFORMAT>& imageFormats)
{
	QString	all("all supported files (");
	QString	readList;

	for(int z = 0;z < imageFormats.count();z++)
	{
		IMAGEFORMAT	i	= imageFormats[z];

		if(i.read)
		{
			all.append(i.extension);
			all.append(" ");

			readList.append(";;");
			readList.append(i.description);
			readList.append(" (");
			readList.append(i.extension);
			readList.append(")");
		}
	}

	readList.prepend(all);
	return(readList);
}

QString generateWriteList(const QList<IMAGEFORMAT>& imageFormats)
{
	QString	all("all supported files (");
	QString	writeList;

	for(int z = 0;z < imageFormats.count();z++)
	{
		IMAGEFORMAT	i	= imageFormats[z];

		if(i.write)
		{
			all.append(i.extension);
			all.append(" ");

			writeList.append(";;");
			writeList.append(i.description);
			writeList.append(" (");
			writeList.append(i.extension);
			writeList.append(")");
		}
	}

	writeList.prepend(all);
	return(writeList);
}

