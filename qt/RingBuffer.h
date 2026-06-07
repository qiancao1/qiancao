// RingBuffer.h
#pragma once
#include <QVector>
#include <QList>
#include <QAtomicInt>
#include <algorithm>
#include "chatpage.h"

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(){}

    void setCapacity(int newCapacity) {
        if (newCapacity <= 0) {
            m_data.clear();
            m_capacity = 0;
            m_head=0;
        } else {
            m_data.resize(newCapacity);
            m_capacity = newCapacity;
            m_head=0;
        }
    }
    int allocate() {
        if (m_capacity==0) return -1;
        return m_head.fetchAndAddOrdered(1) % m_capacity;
    }

    T* allocate2() {
        if (m_capacity==0) return nullptr;
        int idx = m_head.fetchAndAddOrdered(1) % m_capacity;
        return &m_data[idx];
    }

    T& at(int index) {
        static T dummy;
        if (m_capacity==0) return dummy;
        Q_ASSERT(index >= 0 && index < m_capacity);
        return m_data[index];
    }

    // 原有的通用读取
    QVector<T> readLatest(int n) const {
        static QList<T> dummy;
        if (m_capacity==0) return dummy;
        QVector<T> result;
        int head = m_head.loadAcquire();
        if (head == 0) return result;
        int total = qMin(n, head);
        result.reserve(total);
        int firstLogical = (head > m_capacity) ? (head - m_capacity) : 0;
        int startLogical = qMax(firstLogical, head - total);
        for (int logical = startLogical; logical < head; ++logical) {
            int physical = logical % m_capacity;
            result.append(m_data[physical]);
        }
        return result;
    }

    // 新增：直接返回 QList<Message>，最多 maxMessages 条，自动过滤 groupId 和 appid
    // 参数说明：
    //   maxMessages     - 最多返回多少条 Message（例如 100）
    //   contactId       - 需要匹配的 groupId
    //   appid2          - 需要匹配的 appid（当 checkAppid 为 true 时使用）
    //   checkAppid      - 是否检查 appid（对应 m_currentBotIndex != -1）
    //   lastMsgId       - 输出参数，返回最后一个 content 非空的 msgid（可选）
    QList<Message> readLatestMessages(int maxMessages,
                                      const QString& contactId,
                                      int appid2,
                                      bool checkAppid,
                                      QString* lastMsgId = nullptr) const
    {
        QList<Message> result;
        if (m_capacity==0) return result;
        int head = m_head.loadAcquire();
        if (head == 0) return result;

        int firstLogical = (head > m_capacity) ? (head - m_capacity) : 0;
        // 临时收集（逆序）
        QVector<Message> temp;
        temp.reserve(maxMessages);

        // 从最新向最旧扫描
        for (int logical = head - 1; logical >= firstLogical && temp.size() < maxMessages; --logical) {
            const T& e = m_data[logical % m_capacity];
            // 过滤条件
            if (checkAppid && e.appid != appid2) continue;
            if (e.groupId != contactId) continue;

            // 先处理 direction（可以产生一条 Message）
            if (!e.direction.isEmpty() && temp.size() < maxMessages) {
                if(!e.direction.startsWith("[黑名单]"))
                {
                    Message me;
                    me.msg = e.direction;
                    me.user = e.user;
                    me.timestamp = e.time;
                    me.isSelf = true;
                    me.hf=e.hf;
                    me.ch = e.deleteid;
                    temp.append(me);
                }
            }
            // 再处理 content
            if (!e.msg.isEmpty() && temp.size() < maxMessages) {
                Message me;
                me.msg = e.msg;
                me.user = e.user;
                me.timestamp = e.time;
                me.name= e.user_name;
                me.hf=e.hf;
                temp.append(me);
                // 记录最后一个 content 的 msgid（因为是从新到旧，第一次遇到的就是最新的）
                if (lastMsgId && lastMsgId->isEmpty()) {
                    *lastMsgId = e.msgid;
                }
            }
        }
        // 反转 temp 为正序（旧->新）
        std::reverse(temp.begin(), temp.end());
        // 转换为 QList<Message>
        result = temp.toList();
        return result;
    }

    int totalWritten() const {
        if(m_capacity==0) return 0;
        return m_head.loadAcquire(); }
    int capacity() const { return m_capacity; }

private:
    QVector<T> m_data;
    int m_capacity=0;
    mutable QAtomicInt m_head;
};