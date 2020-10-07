// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2014-2020, The Monero Project.

#ifndef SUBADDRESS_H
#define SUBADDRESS_H

#include <functional>

#include <wallet/api/wallet2_api.h>
#include <QReadWriteLock>
#include <QObject>
#include <QList>
#include <QDateTime>

class Subaddress : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void getAll() const;
    Q_INVOKABLE bool getRow(int index, std::function<void (Monero::SubaddressRow &row)> callback) const;
    Q_INVOKABLE void addRow(quint32 accountIndex, const QString &label) const;
    Q_INVOKABLE void setLabel(quint32 accountIndex, quint32 addressIndex, const QString &label) const;
    Q_INVOKABLE void refresh(quint32 accountIndex) const;
    Q_INVOKABLE quint64 unusedLookahead() const;
    quint64 count() const;

signals:
    void refreshStarted() const;
    void refreshFinished() const;
    void labelChanged() const;

public slots:

private:
    explicit Subaddress(Monero::Subaddress * subaddressImpl, QObject *parent);
    friend class Wallet;
    mutable QReadWriteLock m_lock;
    Monero::Subaddress * m_subaddressImpl;
    mutable QList<Monero::SubaddressRow*> m_rows;
    mutable quint64 m_unusedLookahead;
};

#endif // SUBADDRESS_H
