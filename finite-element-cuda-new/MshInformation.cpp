#include "MshInformation.h"
#include<QCoreApplication>
#include "publicElement.h"
#include <unordered_set>
#include <gmsh.h>
#include "Eigen/Dense"
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
    this->coordinates.clear();
    this->filePath = "";
    this->pointIndexs.clear();
    this->lineIndexs.clear();
    this->circlePointIndexs.clear();
    this->circleArcIndexs.clear();
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



