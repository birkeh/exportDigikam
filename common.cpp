/*!
 \file common.cpp

*/

#include "common.h"

#include <QDebug>
#include <QDir>


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

QList<SolidVolumeInfo> actuallyListVolumes()
{
	QList<SolidVolumeInfo> volumes;

	QList<Solid::Device> devices = Solid::Device::listFromType(Solid::DeviceInterface::StorageAccess);

	foreach (const Solid::Device& accessDevice, devices)
	{
		if (!accessDevice.is<Solid::StorageAccess>())
			continue;

		const Solid::StorageAccess* access = accessDevice.as<Solid::StorageAccess>();

		if (!access->isAccessible())
			continue;

		Solid::Device driveDevice;

		for (Solid::Device currentDevice = accessDevice; currentDevice.isValid() ; currentDevice = currentDevice.parent())
		{
			if(currentDevice.is<Solid::StorageDrive>())
			{
				driveDevice = currentDevice;
				break;
			}
		}


		Solid::StorageDrive* drive = driveDevice.as<Solid::StorageDrive>();

		// check for StorageVolume

		Solid::Device volumeDevice;

		for (Solid::Device currentDevice = accessDevice; currentDevice.isValid() ; currentDevice = currentDevice.parent())
		{
			if (currentDevice.is<Solid::StorageVolume>())
			{
				volumeDevice = currentDevice;
				break;
			}
		}

		if (!volumeDevice.isValid())
			continue;

		Solid::StorageVolume* const volume = volumeDevice.as<Solid::StorageVolume>();

		SolidVolumeInfo info;
		info.udi       = accessDevice.udi();
		info.path      = QDir::fromNativeSeparators(access->filePath());
		info.isMounted = access->isAccessible();

		if (!info.path.isEmpty() && !info.path.endsWith(QLatin1Char('/')))
		{
			info.path += QLatin1Char('/');
		}

		info.uuid  = volume->uuid();
		info.label = volume->label();

		if (drive)
			info.isRemovable = (drive->isHotpluggable() || drive->isRemovable());
		else
			info.isRemovable = false;

		volumes << info;
	}

	return volumes;
}
