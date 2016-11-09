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

#include "sambaconnectionserialhelper.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>

#define MAX_BUF_SIZE (1*1024)

#define ATMEL_USB_VID 0x03eb
#define SAMBA_USB_PID 0x6124

Q_LOGGING_CATEGORY(sambaLogConnSerial, "samba.connserial")

static bool serial_is_at91(const QSerialPortInfo& info)
{
	return info.hasVendorIdentifier()
			&& info.vendorIdentifier() == ATMEL_USB_VID
			&& info.hasProductIdentifier()
			&& info.productIdentifier() == SAMBA_USB_PID;
}

SambaConnectionSerialHelper::SambaConnectionSerialHelper(QQuickItem* parent)
	: QQuickItem(parent),
	  m_port(""),
	  m_baudRate(0)
{
}

SambaConnectionSerialHelper::~SambaConnectionSerialHelper()
{
	close();
}

QString SambaConnectionSerialHelper::port() const
{
	return m_port;
}

void SambaConnectionSerialHelper::setPort(const QString& port)
{
	if (m_port != port)
	{
		m_port = port;
		emit portChanged();
	}
}

quint32 SambaConnectionSerialHelper::baudRate() const
{
	return m_baudRate;
}

void SambaConnectionSerialHelper::setBaudRate(quint32 baudRate)
{
	if (m_baudRate != baudRate)
	{
		m_baudRate = baudRate;
		emit baudRateChanged();
	}
}

QStringList SambaConnectionSerialHelper::availablePorts()
{
	QStringList list_at91, list_other;
	QSerialPortInfo info;
	foreach (info, QSerialPortInfo::availablePorts()) {
		if (serial_is_at91(info))
			list_at91.append(info.portName());
		else
			list_other.append(info.portName());
	}
	return list_at91 + list_other;
}

void SambaConnectionSerialHelper::open()
{
	if (port().isEmpty())
	{
		QStringList ports = availablePorts();
		if (ports.isEmpty())
		{
			emit connectionFailed("No serial ports found");
			return;
		}

		setPort(ports.at(0));
	}

	QSerialPortInfo info(port());
	if (!info.isValid())
	{
		emit connectionFailed(
				QString().sprintf("Cannot open invalid port '%s'",
					port().toLocal8Bit().constData()));
		return;
	}

	m_at91 = serial_is_at91(info);

	m_serial.setPort(info);
	if (m_baudRate <= 0)
		m_baudRate = m_at91 ? 921600 : 115200;
	m_serial.setBaudRate(m_baudRate);
	m_serial.setDataBits(QSerialPort::Data8);
	m_serial.setParity(QSerialPort::NoParity);
	m_serial.setStopBits(QSerialPort::OneStop);
	m_serial.setFlowControl(QSerialPort::NoFlowControl);

	qCInfo(sambaLogConnSerial, "Opening serial port '%s'",
			port().toLocal8Bit().constData());

	if (m_serial.open(QIODevice::ReadWrite))
	{
		// resync in case some data was already sent to monitor:
		// send a single '#' and ignore response
		writeSerial(QString("#"));
		readAllSerial();

		// switch to binary mode
		writeSerial(QString("N#"));
		QString resp(QString::fromLatin1(readAllSerial(2)));
		if (resp.isEmpty() || resp == "\n\r")
		{
			emit connectionOpened(m_at91);
		}
		else
		{
			if (resp == "CACK,ffffffff,00000000#")
			{
				emit connectionFailed(
						QString().sprintf(
							"Cannot communicate with monitor on port '%s' because chip is in secure mode",
							port().toLocal8Bit().constData()));
			}
			else {
				emit connectionFailed(
						QString().sprintf(
							"Could not switch monitor on port '%s' to binary mode",
							port().toLocal8Bit().constData()));
			}
		}
	}
	else
	{
		emit connectionFailed(
				QString().sprintf("Could not open serial port '%s': %s",
					port().toLocal8Bit().constData(),
					m_serial.errorString().toLocal8Bit().constData()));
	}
}

void SambaConnectionSerialHelper::writeSerial(const QString &str)
{
	qCDebug(sambaLogConnSerial).noquote().nospace() << "SERIAL<<" << str;

	QByteArray data = str.toLocal8Bit();
	m_serial.write(data.constData(), data.length());
	m_serial.waitForBytesWritten(10);
}

void SambaConnectionSerialHelper::writeSerial(const QByteArray &data)
{
	qCDebug(sambaLogConnSerial).noquote().nospace() << "SERIAL<<" << data.toHex();

	m_serial.write(data.constData(), data.length());
	m_serial.waitForBytesWritten(10);
}

QByteArray SambaConnectionSerialHelper::readAllSerial(int minBytes, int timeout)
{
	QByteArray resp;
	QElapsedTimer timer;

	/* timeout=0 -> default timeout, timeout<0 -> no timeout */
	if (timeout == 0)
		timeout = 10;

	timer.start();
	do {
		if (timeout > 0 && (timer.elapsed() >= timeout))
			break;
		m_serial.waitForReadyRead(10);
		if (m_serial.bytesAvailable())
			resp.append(m_serial.readAll());
	} while (resp.length() < minBytes);

	qCDebug(sambaLogConnSerial).noquote().nospace() << "SERIAL>>" << resp.toHex();

	return resp;
}

void SambaConnectionSerialHelper::close()
{
	if (m_serial.isOpen())
	{
		m_serial.close();
		emit connectionClosed();
	}
}

QVariant SambaConnectionSerialHelper::readu8(quint32 address, int timeout)
{
	if (!m_serial.isOpen())
		return QVariant();

	writeSerial(QString().sprintf("o%x,#", address));

	QByteArray resp = readAllSerial(1, timeout);
	quint8 value = (quint8)resp[0];
	return QVariant(value);
}

QVariant SambaConnectionSerialHelper::readu16(quint32 address, int timeout)
{
	if (!m_serial.isOpen())
		return QVariant();

	writeSerial(QString().sprintf("h%x,#", address));

	QByteArray resp = readAllSerial(2, timeout);
	quint16 value = (((quint8)resp[1]) << 8) + ((quint8)resp[0]);
	return QVariant(value);
}

QVariant SambaConnectionSerialHelper::readu32(quint32 address, int timeout)
{
	if (!m_serial.isOpen())
		return false;

	writeSerial(QString().sprintf("w%x,#", address));

	QByteArray resp = readAllSerial(4, timeout);
	quint32 value = (((quint8)resp[3]) << 24) + (((quint8)resp[2]) << 16) +
			(((quint8)resp[1]) << 8) + ((quint8)resp[0]);
	return QVariant(value);
}

SambaByteArray *SambaConnectionSerialHelper::read(quint32 address, unsigned length, int timeout)
{
	if (!m_serial.isOpen() || length == 0)
		return new SambaByteArray();

	QByteArray data;
	QElapsedTimer timer;

	/* timeout=0 -> default timeout, timeout<0 -> no timeout */
	if (timeout == 0)
		timeout = 1000;

	int offset = 0;
	timer.start();
	while (length > 0)
	{
		if (timeout > 0 && (timer.elapsed() >= timeout)) {
			qCCritical(sambaLogConnSerial).noquote().nospace() <<
				QString().sprintf("Timeout while reading from '%s'",
					port().toLocal8Bit().constData());
			break;
		}
		int chunkSize = length > MAX_BUF_SIZE ? MAX_BUF_SIZE : length;
		if ((chunkSize & 63) == 0)
			chunkSize--;
		writeSerial(QString().sprintf("R%x,%x#", address + offset, chunkSize));
		data.append(readAllSerial(chunkSize, timeout - timer.elapsed()));
		offset += chunkSize;
		length -= chunkSize;
	}

	return new SambaByteArray(data);
}

bool SambaConnectionSerialHelper::writeu8(quint32 address, quint8 data)
{
	if (!m_serial.isOpen())
		return false;

	writeSerial(QString().sprintf("O%x,%02x#", address, data));
	return true;
}

bool SambaConnectionSerialHelper::writeu16(quint32 address, quint16 data)
{
	if (!m_serial.isOpen())
		return false;

	writeSerial(QString().sprintf("H%x,%04x#", address, data));
	return true;
}

bool SambaConnectionSerialHelper::writeu32(quint32 address, quint32 data)
{
	if (!m_serial.isOpen())
		return false;

	writeSerial(QString().sprintf("W%x,%08x#", address, data));
	return true;
}

bool SambaConnectionSerialHelper::write(quint32 address, SambaByteArray *data)
{
	if (!m_serial.isOpen())
		return false;

	int length = data->constData().length();
	int offset = 0;
	while (length > 0)
	{
		int chunkSize = length > MAX_BUF_SIZE ? MAX_BUF_SIZE : length;
		writeSerial(QString().sprintf("S%x,%x#", address + offset, chunkSize));
		writeSerial(data->constData().mid(offset, chunkSize));
		offset += chunkSize;
		length -= chunkSize;
	}

	return true;
}

bool SambaConnectionSerialHelper::go(quint32 address)
{
	if (!m_serial.isOpen())
		return false;

	writeSerial(QString().sprintf("G%x#", address));

	return true;
}
