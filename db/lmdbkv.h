#ifndef LMDBKV_H
#define LMDBKV_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <lmdb.h>

class LmdbKV : public QObject
{
    Q_OBJECT
public:
    explicit LmdbKV(const QString &dbPath, QObject *parent = nullptr);
    ~LmdbKV();

    // 写入或修改键值对
    bool put(const QString &key, const QString &value);
    bool put(const QByteArray &key, const QByteArray &value);

    // 读取键对应的值，如果键不存在返回空字符串
    QString get(const QString &key) const;
    QByteArray get(const QByteArray &key) const;

    // 删除键
    bool remove(const QString &key);
    bool remove(const QByteArray &key);

private:
    MDB_env *m_env;
    MDB_dbi m_dbi;
    QString m_dbPath;

    // 内部通用操作（使用字节数组）
    bool putInternal(const QByteArray &key, const QByteArray &value);
    QByteArray getInternal(const QByteArray &key) const;
    bool removeInternal(const QByteArray &key);
};

#endif // LMDBKV_H