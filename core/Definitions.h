//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_CONSTANTS_H
#define GBMOT_CONSTANTS_H

#include <vector>
#include <memory>

namespace core
{
    class ObjectData;
    class ObjectData3D;
    class ObjectDataMap;
    class ObjectDataAngular;
    class Tracklet;

    typedef std::vector<std::vector<std::vector<double>>> Vector3d;
    typedef std::vector<std::vector<double>> Vector2d;
    typedef std::shared_ptr<ObjectData> ObjectDataPtr;
    typedef std::shared_ptr<ObjectData3D> ObjectData3DPtr;
    typedef std::shared_ptr<ObjectDataMap> ObjectDataMapPtr;
    typedef std::shared_ptr<ObjectDataAngular> ObjectDataAngularPtr;
    typedef std::shared_ptr<Tracklet> TrackletPtr;
}

#endif //GBMOT_CONSTANTS_H
