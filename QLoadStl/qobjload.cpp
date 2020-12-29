#include "qobjload.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <algorithm>

bool QObjLoad::load(QString fileName, QVector<float>& vPoints)
{
    if (fileName.mid(fileName.lastIndexOf('.')) != ".obj" && fileName.mid(fileName.lastIndexOf('.')) != ".OBJ")
    {
        qDebug() << "file is not a obj file.";
        return false;
    }

    QFile objFile(fileName);
    if (!objFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "open" << fileName << "failed";
        return false;
    }
    else
    {
        qDebug() << "open" << fileName << "success!";
    }

    QVector<float> vertextPoints, texturePoints, normalPoints;
    QVector<int> facesIndexs;
    while (!objFile.atEnd())
    {
        QByteArray lineData = objFile.readLine();

        QList<QByteArray> strValues = lineData.trimmed().split(' ');
        QString dataType = strValues.takeFirst();
        if (dataType == "v")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(vertextPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vt")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(texturePoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vn")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(normalPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "f")
        {
            facesIndexs << strValues.at(0).toInt() << strValues.at(1).toInt() << strValues.at(2).toInt();
        }
    }
    objFile.close();

    for (auto& verFaceInfo : facesIndexs)
    {
        int vIndex = verFaceInfo - 1;

        vPoints << vertextPoints.at(vIndex * 3);
        vPoints << vertextPoints.at(vIndex * 3 + 1);
        vPoints << vertextPoints.at(vIndex * 3 + 2);
    }

    vertextPoints.clear();
    texturePoints.clear();
    normalPoints.clear();
    facesIndexs.clear();

    return true;
}
