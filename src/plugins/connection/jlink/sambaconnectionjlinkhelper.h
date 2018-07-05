/*
 * Copyright (c) 2015-2016, Atmel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef SAMBA_CONNECTION_JLINK_H
#define SAMBA_CONNECTION_JLINK_H

#include <QByteArray>
#include <QLoggingCategory>
#include <QtQml>
#include <QtQuick/QQuickItem>

Q_DECLARE_LOGGING_CATEGORY(sambaLogConnJlink)

class Q_DECL_EXPORT SambaConnectionJlinkHelper : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber NOTIFY serialNumberChanged)
	Q_PROPERTY(bool swd READ swd WRITE setSwd NOTIFY swdChanged)
	Q_PROPERTY(qint32 speed READ speed WRITE setSpeed NOTIFY speedChanged)

public:
	SambaConnectionJlinkHelper(QQuickItem *parent = 0);
	~SambaConnectionJlinkHelper();

	QString serialNumber() const;
	void setSerialNumber(const QString& serialNumber);

	bool swd() const;
	void setSwd(bool swd);

	qint32 speed() const;
	void setSpeed(qint32 speed);

	Q_INVOKABLE QStringList availableSerialNumbers();

	Q_INVOKABLE void open(const QString& deviceFamily);
	Q_INVOKABLE void close();

	Q_INVOKABLE QVariant readu8(quint32 address);
	Q_INVOKABLE QVariant readu16(quint32 address);
	Q_INVOKABLE QVariant readu32(quint32 address);
	Q_INVOKABLE QByteArray read(quint32 address, unsigned length);

	Q_INVOKABLE bool writeu8(quint32 address, quint8 data);
	Q_INVOKABLE bool writeu16(quint32 address, quint16 data);
	Q_INVOKABLE bool writeu32(quint32 address, quint32 data);
	Q_INVOKABLE bool write(quint32 address, const QByteArray& data);

	Q_INVOKABLE bool go(quint32 address);
	Q_INVOKABLE bool waitForMonitor(int timeout);

signals:
	void serialNumberChanged();
	void swdChanged();
	void speedChanged();
	void connectionOpened();
	void connectionFailed(const QString& message);
	void connectionClosed();

private:
	QString m_serialNumber;
	bool m_swd;
	qint32 m_speed;
	int m_core;
};

class Q_DECL_EXPORT SambaConnectionPluginJlink : public QQmlExtensionPlugin
{
	Q_OBJECT

	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
	void registerTypes(const char *uri)
	{
		Q_ASSERT(uri == QLatin1String("SAMBA.Connection.JLink"));
		qmlRegisterType<SambaConnectionJlinkHelper>(uri, 3, 2, "JLinkConnectionHelper");
	}
};

#endif // SAMBA_CONNECTION_JLINK_H
