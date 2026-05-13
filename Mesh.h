#pragma once
#include <QVector>
#include <QString>
#include <QPoint>
#include <QColor>


using namespace std;
struct Vertex3D {
    double x, y, z;

    static void normalize(Vertex3D &v) {
        double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length > 0) {
            v.x /= length;
            v.y /= length;
            v.z /= length;
        }
    }
    Vertex3D operator-(const Vertex3D& V) const{
        return { x - V.x, y - V.y, z - V.z };
    }
    Vertex3D operator*(const Vertex3D &b) const {
        return {
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        };
    }
    double operator|(const Vertex3D &b) const {
       return x * b.x + y * b.y + z * b.z;
    }
    Vertex3D operator*(const double &c) const {
        return {x * c, y * c, z * c};
    }
};
struct Triangle {
    int v1, v2, v3;
};
struct Material {
    double dif[3] = {0.8, 0.8, 0.8};
    double ref[3] = {0.7, 0.7, 0.7};
    double amb[3] = {0.5, 0.5, 0.1};
    double shininess = 32.0;
};
struct Scene {
    Vertex3D lightPos = {0, 0, 500};
    Vertex3D cameraPos = {0, 0, 1000};
    int lightColor[3] = {255, 125, 150};
    int Amb[3] = {255, 100, 100};
};


class Mesh {
private:
    QVector<Vertex3D> VectorNorm;
    QVector<Vertex3D> VectorLight;
    double Thetta;
    double Phi;
    QVector<Triangle> Obj;
    QVector<Triangle> Array;
    QVector<Vertex3D> Tpoints;
    QVector<Vertex3D> Spoints;
    void addObject(int v1, int v2, int v3,QVector<Triangle>& tri){
        Triangle t;
        t.v1 = v1;//
        t.v2 = v2;
        t.v3 = v3;
        tri.push_back(t);
    }

public:
    QVector<Triangle> getObj() const {return Obj;}
    QVector<Triangle> getArray() const {return Array;}
    QVector<Vertex3D> getTpoints() const {return Tpoints;}
    QVector<Vertex3D> getSpoints() const {return Spoints;}

    void setObj(const QVector<Triangle>& t){Obj = t;}
    void setArray(const QVector<Triangle>& t){Array = t;}
    void setTpoints(const QVector<Vertex3D>& Tp) {Tpoints = Tp;}
    void setSpoints(const QVector<Vertex3D>& Sp) {Spoints = Sp;}
    void createCube(double a);
    void createSphere(double r, int floor);
    void createArray(double a, int z, int kus);

    bool loadFromVTK(QString filename);
    bool saveToVTK(QString filename);
    
    void setThetta(int thetta){Thetta = thetta;}
    void setPhi(int phi){Phi = phi;}
    void setVectorNorm(int Thetta, int Phi);
    
    double getThetta() const {return Thetta;}
    double getPhi() const {return Phi;}
    QVector<Vertex3D>& getVectorNorm() {return VectorNorm;}
    QVector<Vertex3D> mutation(const QVector<Vertex3D>& VectorNorm, const QVector<Vertex3D>& point);

    QVector<Vertex3D> parallelProj(QVector<Vertex3D>& points);
    QVector<Vertex3D> perspectiveProj(QVector<Vertex3D>& points, int d);
    void Projection(int index);

};