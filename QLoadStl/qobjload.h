#ifndef QOBJLOAD_H
#define QOBJLOAD_H

#include <QVector>
#include <QString>

class QObjLoad
{
public:
    QObjLoad() {}
    ~QObjLoad() {}

    bool load(QString fileName, QVector<float>& vPoints);
};

#endif // QOBJLOAD_H
