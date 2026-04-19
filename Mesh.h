#pragma once
#include <QVector>
#include <QString>


using namespace std;
struct Vertex3D {
    double x, y, z;
};
struct Triangle {
    int v1, v2, v3;
};

class Mesh {
private:
    QVector<Vertex3D> VectorNorm;
    double Thetta;
    double Phi;
    QVector<Triangle> Obj;
    QVector<Vertex3D> Tpoints;
    /*QVector<Vertex3D> p;
    QVector<Triangle> t;*/
    void addObject(int v1, int v2, int v3){
        Triangle t;
        t.v1 = v1;
        t.v2 = v2;
        t.v3 = v3;
        Obj.push_back(t);
    }
public:
    QVector<Triangle> getObj() const {return Obj;}
    QVector<Vertex3D> getTPoints() const {return Tpoints;}

    void setObj(const QVector<Triangle>& t){Obj = t;}
    void setTpoints(const QVector<Vertex3D>& Tp) {Tpoints = Tp;}

    void createCube(double a);
    void createSphere(double r, int floor);

    bool loadFromVTK(QString filename);
    bool saveToVTK(QString filename);
    
    void setThetta(int thetta){Thetta = thetta * M_PI / 180;}
    void setPhi(int phi){Phi = phi * M_PI / 180;;}
    void setVectorNorm(int Thetta, int Phi);
    
    double getThetta() const {return Thetta;}
    double getPhi() const {return Phi;}
    QVector<Vertex3D>& getVectorNorm() {return VectorNorm;}
    QVector<Vertex3D> mutation(const QVector<Vertex3D>& VectorNorm);
    void parallelProj(QVector<Vertex3D>& points, int d);



};