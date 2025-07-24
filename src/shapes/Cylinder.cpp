#include "Cylinder.h"
#include "Constants.h"
#include "shapes/OpenGLShape.h"

bool Cylinder::intersectsMathModel(const Ray& ray, float& distance) const {
    glm::vec3 o = ray.getOrigin(), d = ray.getDirection();
    float a2b2 = wx*wx*wy*wy, d2 = d[0]*d[0] + d[1]*d[1];
    if (glm::abs(d2) < TRACE_PRECISION) { // ray parallel to Z axis
        distance = 1; // TODO proper distance calculation
        return o[0]*o[0] + o[1]*o[1] <= a2b2;
    }
    float diff = o[0]*d[1] - o[1]*d[0];
    float D = d2 * a2b2 - diff*diff;
    if (D < 0) return false;
    float t1 = (-o[0]*d[0] - o[1]*d[1] + sqrt(D)) / d2,
          t2 = (-o[0]*d[0] - o[1]*d[1] - sqrt(D)) / d2;
    return abs(o[2] + t1 * d[2]) <= wz ||
           abs(o[2] + t2 * d[2]) <= wz;
}

Cylinder::Cylinder(float sizex, float sizey, float sizez, unsigned int facets, bool altering) :
    OpenGLShape(6), wx(sizex), wy(sizey), wz(sizez), facets(facets), isAltering(altering) {

    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    float x = wx / 2, y = wy / 2, z = wz / 2;

    std::vector<glm::vec3> ptsBottom, ptsTop;
    float angleShift = altering ? (PI / facets) : 0;
    for (int i = 0; i < facets; ++i) {
        float ab = 2*PI*i / facets, // angle bottom
              at = ab + angleShift; // angle top
        ptsBottom.emplace_back(x * cos(ab), y * sin(ab), -z);
        ptsTop.emplace_back(x * cos(at), y * sin(at), z);
    }

    addFacePoly(ptsBottom);
    addFacePoly(ptsTop);

    for (int i = 0; i < facets; ++i) {
        int j = (i + 1) % facets;
        if (altering) {
            addFace(ptsBottom[i], ptsBottom[j], ptsTop[i]);
            addFace(ptsBottom[j], ptsTop[j], ptsTop[i]);
        } else {
            addFacePoly({ptsBottom[i], ptsBottom[j], ptsTop[j], ptsTop[i]});
        }
    }

    setupBuffer();
}