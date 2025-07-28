#include "Cylinder.h"
#include "Constants.h"
#include "glm/common.hpp"
#include "shapes/OpenGLShape.h"

bool Cylinder::intersectsMathModel(const Ray& ray, float& distance) const {
    glm::vec3 o = ray.getOrigin(), d = ray.getDirection();
    float a = wy*d[0],
          b = wx*d[1],
          c = wy*o[0],
          _d = wx*o[1],
          aa = wx*wx,
          bb = wy*wy,
          e = aa*bb,
          z = o[2];
    if (c*c + _d*_d <= e && abs(z) <= wz) { // already inside the cylinder, direction is irrelevant
        distance = 0;
        return true;
    }
    if (abs(d[0]*d[0] + d[1]*d[1]) < TRACE_PRECISION) { // ray parallel to Z axis
        bool result = bb*o[0]*o[0] + aa*o[1]*o[1] <= e; // inside the infinite cylinder
        float z = o[2];
        result = result && ((z > -wz && d[2] < 0) || (z < wz && d[2] > 0)); // and pointed towards its wz-bound part
        if (result) distance = (d[2] > 0 ? -wz - z : z - wz);
        return result;
    }
    float A = a*a + b*b,
          B = a*c + b*_d,
          sq = a*_d - b*c,
          D = A*e - sq*sq; // C coefficient is redundant (built in here)
    if (D < 0) return false; // whole ray XY projection doesn't intersect the infinite cylinder
    float t1 = (-B + sqrt(D)) / A,
          t2 = (-B - sqrt(D)) / A; // intersections with infinite cylinder
    if (abs(d[2]) < TRACE_PRECISION) {
        bool result = abs(o[2]) <= wz; // ray belongs to the XY plane
        distance = std::min(abs(t1), abs(t2));
        return result;
    }
    float z1 = o[2] + t1 * d[2],
          z2 = o[2] + t2 * d[2];
    // both intersections with infinite cylinder are on one side of its wz-bound part
    bool result = (glm::sign(z1) != glm::sign(z2) || abs(z1) <= wz || abs(z2) <= wz);
    if (result) distance = std::min(abs(t1), abs(t2));
    return result;
}

Cylinder::Cylinder(float sizex, float sizey, float sizez, unsigned int facets, bool altering) :
    OpenGLShape(6), wx(sizex), wy(sizey), wz(sizez), facets(facets), isAltering(altering) {

    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    std::vector<glm::vec3> ptsBottom, ptsTop;
    float angleShift = altering ? (PI / facets) : 0;
    for (int i = 0; i < facets; ++i) {
        float ab = 2*PI*i / facets, // angle bottom
              at = ab + angleShift; // angle top
        ptsBottom.emplace_back(wx * cos(ab), wy * sin(ab), -wz);
        ptsTop.emplace_back(wx * cos(at), wy * sin(at), wz);
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