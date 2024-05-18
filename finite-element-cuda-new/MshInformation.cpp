#include "MshInformation.h"
#include<QCoreApplication>
#include "publicElement.h"
#include <unordered_set>
#include <gmsh.h>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <queue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "CalcTools.h"
#include <fstream>
#include <iostream>
extern Circle circle;

MshInformation::MshInformation() {}


vector<int> MshInformation::getPointTags(int i) {
    vector<int> tags;
    for (int j = 0; j < 3 ; j++) {
        tags.push_back(this->nodeTagsForTriangle[this->triangleIndex][i* 3 + j]);
    }
    return tags;
}

bool MshInformation::initTagMap() {
    if (!this->xList.empty()) {
        qDebug() << "have initialized.";
        return true;
    }
    int count = 0;
    std::unordered_set<int> tempSet;
    for (int i = 0; i < this->elementTags[this->triangleIndex].size(); i++) {
        tempSet.insert(this->nodeTagsForTriangle[this->triangleIndex][i * 3]);
        tempSet.insert(this->nodeTagsForTriangle[this->triangleIndex][i * 3 + 1]);
        tempSet.insert(this->nodeTagsForTriangle[this->triangleIndex][i * 3 + 2]);
    }
    // qDebug() << "the len of tempSet is " << tempSet.size();
    for (int i = 0; i < this->nodeTags.size(); i++) {
        if (tempSet.find(this->nodeTags[i]) == tempSet.end()) {
            // qDebug() << "the missing index is" << this->nodeTags[i];
            continue;
        }
        this->xList.push_back(this->coord[i * 3]);
        this->yList.push_back(this->coord[i * 3 + 1]);
        this->tagMap[this->nodeTags[i]] = count;
        count++;
    }
    qDebug() << " after function, the len of xList is " << this->xList.size();
    return true;
}

void MshInformation::initPointAndTriangleInfo(){//将网格中点的获取和三角形的获取放到该函数
    gmsh::model::mesh::getNodes(this->nodeTags, this->coord, this->parametricCoord);
    gmsh::model::mesh::getElements(this->elementTypes, this->elementTags, this->nodeTagsForTriangle);
    for (int i = 0; i < this->elementTypes.size(); i++) {
        if (this->elementTypes[i] == 2) {
            this->triangleIndex = i;
            break;
        }
    }
    if (this->triangleIndex == -1) {
        qFatal("no tiangle founded!");
    }
    this->initTagMap();
    this->fMatrix = Eigen::MatrixXd::Zero(this->xList.size() * 2, 1);
    int len = this->xList.size();
    this->kMatrix = Eigen::MatrixXd::Zero(len * 2, len * 2);
}

void MshInformation::clearAll()
{
    this->filePath = "";
    this->tagMap.clear();
    this->nodeTags.clear();
    this->coord.clear();
    this->parametricCoord.clear();
    this->elementTypes.clear();
    this->elementTags.clear();
    this->nodeTagsForTriangle.clear();
    this->xList.clear();
    this->yList.clear();
    this->forces.clear();
    this->edgeInfos.clear();
    this->mechanicBehaviors.clear();

}

void MshInformation::addRect(double x, double y, double width, double height)
{
    vector<int> pointIndexes;
    pointIndexes.push_back(gmsh::model::geo::addPoint(x, y, 0, lc));
    pointIndexes.push_back(gmsh::model::geo::addPoint(x + width, y, 0, lc));
    pointIndexes.push_back(gmsh::model::geo::addPoint(x + width, y + height, 0, lc));
    pointIndexes.push_back(gmsh::model::geo::addPoint(x, y + height, 0, lc));
    vector<int> lineIndexes;
    for (int i = 1; i < pointIndexes.size(); i++) {
        lineIndexes.push_back(gmsh::model::geo::addLine(pointIndexes[i - 1], pointIndexes[i]));
    }
    lineIndexes.push_back(gmsh::model::geo::addLine(pointIndexes[pointIndexes.size() - 1], pointIndexes[0]));
    this->curveLoopList.push_back(gmsh::model::geo::addCurveLoop(lineIndexes));//注意，这里的第一个curveLoop必须是最大的那个图形

}

void MshInformation::addCircle(double x, double y, double radius)
{
    vector<int> pointIndexes;
    pointIndexes.push_back(gmsh::model::geo::addPoint(x - radius, y, 0, lc));
    pointIndexes.push_back(gmsh::model::geo::addPoint(x, y, 0, lc));
    pointIndexes.push_back(gmsh::model::geo::addPoint(x + radius, y, 0, lc));

    int circleLeftIndex = gmsh::model::geo::addCircleArc(pointIndexes[0], pointIndexes[1], pointIndexes[2]);
    int circleRightIndex = gmsh::model::geo::addCircleArc(pointIndexes[2], pointIndexes[1], pointIndexes[0]);
    this->curveLoopList.push_back(gmsh::model::geo::addCurveLoop({ circleLeftIndex, circleRightIndex }));
}

void MshInformation::createMsh()
{
    gmsh::model::geo::addPlaneSurface(this->curveLoopList);

    gmsh::model::geo::synchronize();

    gmsh::model::mesh::generate(2);
}

void MshInformation::saveMsh() {
    if (!this->filePath.isEmpty()) {
        gmsh::write(this->filePath.toStdString());
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(nullptr,
        QObject::tr("保存文件"), "",
        QObject::tr("文本文件 (*.msh);;所有文件 (*)"));
    if (!fileName.endsWith(".msh", Qt::CaseInsensitive)) {
        fileName += ".msh";
    }
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 文件已成功创建
        qDebug() << "文件成功创建";
        file.close(); // 关闭文件
    }
    else {
        qDebug() << "文件创建失败";
    }
    this->filePath = fileName;
    gmsh::write(this->filePath.toStdString());
}

bool MshInformation::loadMsh()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, QObject::tr("打开文件"), "/home", QObject::tr("所有文件 (*)"));
    if (!fileName.isEmpty()) {
        gmsh::initialize();
        gmsh::open(fileName.toStdString());
        this->initPointAndTriangleInfo();
        return true;
    }
    else {
        QMessageBox::warning(nullptr, QObject::tr("警告"), QObject::tr("打开文件失败"));
        return false;
    }
}

void MshInformation::addUniformLoad(double startX, double startY, double endX, double endY, double xDirection, double yDirection)
{
    std::priority_queue<pair<double, double>, vector<pair<double, double>>, ComparePair> pq;
    pq.push(pair<double, double>(startX, startY));
    pq.push(pair<double, double>(endX, endY));
    for (int i = 0; i < this->xList.size(); i++) {
        double x = this->xList[i];
        double y = this->yList[i];
        double instance = CalcTools::pointToSegmentDistance(startX, startY, endX, endY, x, y);
        if (instance < 1e-5) {
            pq.push(pair<double, double>(x, y));
        }
    }
    while (pq.size() > 1) {
        pair<double, double> pre = pq.top();
        pq.pop();
        pair<double, double> next = pq.top();
        Force force;
        force.x = (pre.first + next.first) / 2;
        force.y = (pre.second + next.second) / 2;
        double len = sqrt(pow(pre.first - next.first, 2) + pow(pre.second - next.second, 2));
        force.xForce = len * xDirection;
        force.yForce = len * yDirection;
        this->forces.push_back(force);
    }
    QMessageBox::information(nullptr, "操作完成", "添加成功！");
}

void MshInformation::addPointForce(double x, double y, double xForce, double yForce)
{
     Force force;
     force.x = x;
     force.y = y;
     force.xForce = xForce;
     force.yForce = yForce;
     this->forces.push_back(force);
}

void MshInformation::saveConstraint()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
        QObject::tr("保存约束"), "",
        QObject::tr("文本文件 (*.json);;所有文件 (*)"));
    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".json";
    }
    QJsonArray forcesArray;
    for (const auto& force : this->forces) {
        QJsonObject forceObject;
        forceObject["x"] = force.x;
        forceObject["y"] = force.y;
        forceObject["xForce"] = force.xForce;
        forceObject["yForce"] = force.yForce;
        forcesArray.append(forceObject);
    }
    QJsonArray edgeInfosArray;
    for (const auto& edgeInfo : this->edgeInfos) {
        QJsonObject edgeInfoObject;
        edgeInfoObject["x"] = edgeInfo.x;
        edgeInfoObject["y"] = edgeInfo.y;
        edgeInfoObject["xFixed"] = edgeInfo.xFixed;
        edgeInfoObject["yFixed"] = edgeInfo.yFixed;
        edgeInfosArray.append(edgeInfoObject);
    }
    QJsonObject dataObject;
    dataObject["forces"] = forcesArray;
    dataObject["edgeInfos"] = edgeInfosArray;
    QJsonDocument doc(dataObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonString.toUtf8());
        file.close();
    }
}

void MshInformation::loadConstraint()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, QObject::tr("装载约束"), "/home/桌面", QObject::tr("所有文件(*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("无法打开文件");
            return;
        }
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (!doc.isObject()) {
            qWarning("JSON格式错误");
            return;
        }
        QJsonObject jsonObject = doc.object();
        QJsonArray forcesArray = jsonObject["forces"].toArray();
        for (const QJsonValue& value : forcesArray) {
            QJsonObject obj = value.toObject();
            Force force;
            force.x = obj["x"].toDouble();
            force.y = obj["y"].toDouble();
            force.xForce = obj["xForce"].toDouble();
            force.yForce = obj["yForce"].toDouble();
            this->forces.push_back(force);
        }
        QJsonArray edgeInfosArray = jsonObject["edgeInfos"].toArray();
        vector<EdgeInfo> edgeInfos;
        for (const QJsonValue& value : edgeInfosArray) {
            QJsonObject obj = value.toObject();
            EdgeInfo edgeInfo;
            edgeInfo.x = obj["x"].toDouble();
            edgeInfo.y = obj["y"].toDouble();
            edgeInfo.xFixed = obj["xFixed"].toBool();
            edgeInfo.yFixed = obj["yFixed"].toBool();
            this->edgeInfos.push_back(edgeInfo);
        }
    }
    else {
        QMessageBox::warning(nullptr, QObject::tr("警告"), QObject::tr("打开文件失败"));
    }
}

void MshInformation::saveMatrixes()
{

    std::ofstream file("Xmatrix.txt");
    if (file.is_open()) {
        for (int i = 0; i < this->X.rows(); ++i) {
            for (int j = 0; j < this->X.cols(); ++j) {
                file << std::fixed << std::scientific << std::setprecision(2) << this->X(i, j);
                if (j != this->X.cols() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "无法打开文件" << std::endl;
    }


    std::ofstream fileK("kmatrix.txt");
    if (fileK.is_open()) {
        for (int i = 0; i < this->kMatrix.rows(); ++i) {
            for (int j = 0; j < this->kMatrix.cols(); ++j) {
                fileK << std::fixed << std::scientific << std::setprecision(2) << this->kMatrix(i, j);
                if (j != this->kMatrix.cols() - 1) fileK << "\t";
            }
            fileK << "\n";
        }
        fileK.close();
    }
    else {
        std::cerr << "无法打开文件" << std::endl;
    }

    std::ofstream fileF("fmatrix.txt");
    if (fileF.is_open()) {
        for (int i = 0; i < this->fMatrix.rows(); ++i) {
            for (int j = 0; j < this->fMatrix.cols(); ++j) {
                fileF << std::fixed << std::scientific << std::setprecision(2) << this->fMatrix(i, j);
                if (j != this->fMatrix.cols() - 1) fileF << ",";
            }
            fileF << "\n";
        }
        fileF.close();
    }
    else {
        std::cerr << "无法打开文件" << std::endl;
    }

}



