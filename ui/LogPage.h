#ifndef LOGPAGE_H
#define LOGPAGE_H

#include <QWidget>
#include <QTableView>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include "RingBuffer.h"      // 你的环形缓冲区头文件
#include <QPushButton>




struct LogEntry {

    QString time;
    QString groupId;
    QString user;
    QString user_name;
    QString msg;
    QString msgid; //如果是插件发的
    QString deleteid;
    QString direction; //插件输出
    QString botName;
    QString hf;
    qint64 timestamp_us=0;
    int appid=0;
    int color=0;
    int n=0;
    bool fullType=false;
   QAtomicInt dirState = 0;

};
// 列字段类型枚举
enum LogField {
    Field_Time,
    Field_BotName,
    Field_TargetId,      // 群号/目标ID
    Field_SenderId,      // 发送人ID
    Field_Content,       // 消息内容/接收内容
    Field_Direction      // 回应/插件处理
};

// 列描述：字段类型、标题、默认宽度
struct LogColumn {
    LogField field;
    QString title;
    int defaultWidth;
};



class LogListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LogListModel(const QList<LogColumn> &columns, RingBuffer<LogEntry> *buffer, QObject *parent = nullptr);
    ~LogListModel();

    // 设置过滤条件（机器人ID），调用后会全量重建（因为过滤条件变了）
    void setFilter(int botId, bool enable);

    // 开始自动同步（启动定时器，定期检查环形缓冲区变化）
    void startAutoSync(int intervalMs = 500);
    void stopAutoSync();
    void refreshNow();
    // 模型接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    LogEntry entryAt(int row) const;
    int count() const { return m_logicalIndices.size(); }

private slots:
    void onSyncTimer();   // 定时检查环形缓冲区并增量更新

private:
    // 增量更新：根据当前环形缓冲区的状态，同步索引列表
    void syncWithBuffer();

    // 内部辅助
    bool matchesFilter(const LogEntry &entry) const;
    void rebuildAll();    // 全量重建（用于过滤条件改变或初始加载）

    QList<LogColumn> m_columns;
    RingBuffer<LogEntry> *m_buffer = nullptr;
    QVector<int> m_logicalIndices;    // 当前显示的逻辑索引（按时间升序）

    int m_lastHead = 0;               // 上一次同步时的 head 值
    int m_lastFirstLogical = 0;       // 上一次同步时的第一个有效逻辑索引

    int m_filterBotId=0;
    bool m_filterEnabled = false;

    QTimer *m_syncTimer = nullptr;
    bool m_needFullRebuild = false;   // 需要全量重建的标志（如过滤条件改变）
};

// 主界面
class LogPage : public QWidget
{
    Q_OBJECT
public:
    explicit LogPage(QWidget *parent = nullptr);
    ~LogPage();




    // 设置当前选中的机器人
    void setCurrentBot(int botId, const QString &botName);
    void setActive(bool active);

    // 环形缓冲区（5个tab的数据源）

    bool m_active = true;
    LogListModel *eventModel = nullptr;
    int currentTabIndex = 0;

private slots:

    void switchTab(int index);  // 切换标签页


private:
    void setupUi();
    void applyStyleSheet();     // 你的样式表函数（假设存在）
    void updateCountDisplay();  // 更新底部标签
    bool entryMatchesCurrentBot(const LogEntry &entry) const;
    QString currentBotLabel() const;

    LogListModel* currentModel();
    QTableView* currentListView();

private:
    // UI组件
    QStackedWidget *tabStack = nullptr;
    QPushButton *btnEventTab = nullptr;
    QPushButton *btnGroupTab = nullptr;
    QPushButton *btnPrivateTab = nullptr;
    QPushButton *btnChannelTab = nullptr;
    QPushButton *btnChannelPrivateTab = nullptr;
    QLabel *logCountLabel = nullptr;

    // 五个Tab对应的View和Model
    QTableView *eventListView = nullptr;

    QTableView *groupListView = nullptr;
    LogListModel *groupModel = nullptr;
    QTableView *channelListView = nullptr;
    LogListModel *channelModel = nullptr;
    QTableView *privateListView = nullptr;
    LogListModel *privateModel = nullptr;
    QTableView *channelPrivateListView = nullptr;
    LogListModel *channelPrivateModel = nullptr;

    int m_currentBotId=0;
    QString m_currentBotName;


    // 常量：最大显示条数（可调）
    static constexpr int MAX_DISPLAY = 10000;
};

#endif // LOGPAGE_H