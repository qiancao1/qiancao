// EventTask.cpp
#include "EventTask.h"

EventTask::EventTask(const MessageEvent &event, EventCallback callback)
    : QRunnable(), m_event(event), m_callback(callback)
{
    setAutoDelete(true);
}

void EventTask::run()
{
    if (m_callback)
        m_callback(m_event);
}