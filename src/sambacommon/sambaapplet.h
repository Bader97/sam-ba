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

#ifndef SAMBA_APPLET_H
#define SAMBA_APPLET_H

#include <sambacommon.h>
#include <sambaappletcommand.h>
#include <QObject>
#include <QtQml>
#include <QtQuick/QQuickItem>

class SAMBACOMMONSHARED_EXPORT SambaApplet : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QString codeUrl READ codeUrl WRITE setCodeUrl NOTIFY codeUrlChanged)
	Q_PROPERTY(quint32 codeAddr READ codeAddr WRITE setCodeAddr NOTIFY codeAddrChanged)
	Q_PROPERTY(quint32 mailboxAddr READ mailboxAddr WRITE setMailboxAddr NOTIFY mailboxAddrChanged)
	Q_PROPERTY(quint32 entryAddr READ entryAddr WRITE setEntryAddr NOTIFY entryAddrChanged)
	Q_PROPERTY(quint32 traceLevel READ traceLevel WRITE setTraceLevel NOTIFY traceLevelChanged)
	Q_PROPERTY(QQmlListProperty<SambaAppletCommand> commands READ commands)
	Q_PROPERTY(quint32 bufferAddr READ bufferAddr WRITE setBufferAddr NOTIFY bufferAddrChanged)
	Q_PROPERTY(quint32 bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)
	Q_PROPERTY(quint32 bufferPages READ bufferPages WRITE setBufferPages NOTIFY bufferPagesChanged)
	Q_PROPERTY(quint32 pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)
	Q_PROPERTY(quint32 memorySize READ memorySize WRITE setMemorySize NOTIFY memorySizeChanged)
	Q_PROPERTY(quint32 memoryPages READ memoryPages WRITE setMemoryPages NOTIFY memoryPagesChanged)
	Q_PROPERTY(quint32 eraseSupport READ eraseSupport WRITE setEraseSupport NOTIFY eraseSupportChanged)
	Q_PROPERTY(quint32 paddingByte READ paddingByte WRITE setPaddingByte NOTIFY paddingByteChanged)
	Q_PROPERTY(bool trimPadding READ trimPadding WRITE setTrimPadding NOTIFY trimPaddingChanged)
	Q_PROPERTY(quint32 nandHeader READ nandHeader WRITE setNandHeader NOTIFY nandHeaderChanged)

public:
	explicit SambaApplet(QQuickItem *parent = 0);

	QString name() const;
	void setName(const QString& name);

	QString description() const;
	void setDescription(const QString& description);

	QString codeUrl() const;
	void setCodeUrl(const QString& codeUrl);

	quint32 codeAddr() const;
	void setCodeAddr(quint32 codeAddr);

	quint32 mailboxAddr() const;
	void setMailboxAddr(quint32 mailboxAddr);

	quint32 entryAddr() const;
	void setEntryAddr(quint32 entryAddr);

	quint32 traceLevel() const;
	void setTraceLevel(quint32 traceLevel);

	QQmlListProperty<SambaAppletCommand> commands();
	Q_INVOKABLE bool hasCommand(const QString& name) const;
	Q_INVOKABLE SambaAppletCommand* command(const QString& name) const;

	quint32 bufferAddr() const;
	void setBufferAddr(quint32 bufferAddr);

	quint32 bufferSize() const;
	void setBufferSize(quint32 bufferSize);

	quint32 bufferPages() const;
	void setBufferPages(quint32 bufferPages);

	quint32 pageSize() const;
	void setPageSize(quint32 pageSize);

	quint32 memorySize() const;
	void setMemorySize(quint32 memorySize);

	quint32 memoryPages() const;
	void setMemoryPages(quint32 memoryPages);

	quint32 eraseSupport() const;
	void setEraseSupport(quint32 eraseSupport);

	quint8 paddingByte() const;
	void setPaddingByte(quint8 paddingByte);

	bool trimPadding() const;
	void setTrimPadding(bool trimPadding);

	quint32 nandHeader() const;
	void setNandHeader(quint32 nandHeader);

signals:
	void nameChanged();
	void descriptionChanged();
	void codeUrlChanged();
	void codeAddrChanged();
	void mailboxAddrChanged();
	void entryAddrChanged();
	void traceLevelChanged();
	void retriesChanged();
	void commandsChanged();
	void bufferAddrChanged();
	void bufferSizeChanged();
	void bufferPagesChanged();
	void pageSizeChanged();
	void memorySizeChanged();
	void memoryPagesChanged();
	void eraseSupportChanged();
	void paddingByteChanged();
	void trimPaddingChanged();
	void nandHeaderChanged();

private:
	QString m_name;
	QString m_description;
	QString m_codeUrl;
	quint32 m_codeAddr;
	quint32 m_mailboxAddr;
	quint32 m_entryAddr;
	quint32 m_traceLevel;
	quint32 m_retries;
	QList<SambaAppletCommand*> m_commands;
	quint32 m_bufferAddr;
	quint32 m_bufferSize;
	quint32 m_bufferPages;
	quint32 m_pageSize;
	quint32 m_memorySize;
	quint32 m_memoryPages;
	quint32 m_eraseSupport;
	quint8 m_paddingByte;
	bool m_trimPadding;
	quint32 m_nandHeader;
};

#endif // SAMBA_APPLET_H
