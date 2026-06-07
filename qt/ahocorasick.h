#ifndef AHOCORASICK_H
#define AHOCORASICK_H

#include <QMap>
#include <QQueue>
#include <QChar>
#include <QSet>
#include <QString>
#include <vector>

struct ACNode {
    QMap<QChar, int> next;
    int fail = 0;
    QList<int> output;
    ACNode() = default;
};

class AhoCorasick {
public:
    void insert(const QString &pattern, int patternIndex);
    void build();
    QSet<int> scan(const QString &text) const;
private:
    std::vector<ACNode> nodes{1};
};

#endif // AHOCORASICK_H