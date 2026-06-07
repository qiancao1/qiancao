// EventTask.h
#ifndef EVENTTASK_H
#define EVENTTASK_H

#include <QRunnable>
#include <QObject>
#include "global.h"

// 任务处理器接口（实际处理事件的业务逻辑）
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    virtual void handle(const MessageEvent &event) = 0;
};

class EventTask : public QRunnable
{
public:
    // handler 可以是任意 QObject 子类，需要实现 handle 方法（通过 dynamic_cast 或函数回调）
    // 这里采用简单的函数回调方式，更灵活
    using EventCallback = std::function<void(const MessageEvent&)>;

    explicit EventTask(const MessageEvent &event, EventCallback callback);
    void run() override;

private:
    MessageEvent m_event;
    EventCallback m_callback;
};

#endif // EVENTTASK_H