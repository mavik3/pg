#include "Mesh.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
void Mesh::createCube(double a){
    Tpoints.clear();
    Obj.clear();
    for (int z = 0; z <= 1; z++) {
        for (int y = 0; y <= 1; y++) {
            for (int x = 0; x <= 1; x++) {
                Tpoints.push_back({(double)(x * a), (double)(y * a), (double)(z * a)});
            }
        }
    }
    //musim potom este raz preist tuto cast a
    //optimalizovat ju (myslim ze to mozne spravit)

    addObject(0,1,3);
    addObject(0,2,3);

    addObject(0,1,5);
    addObject(0,4,5);

    addObject(0,2,6);
    addObject(0,4,6);

    addObject(7,3,1);
    addObject(7,5,1);

    addObject(7,3,2);
    addObject(7,6,2);

    addObject(7,5,4);
    addObject(7,6,4);

}

void Mesh::createSphere(double r, int stacks){

    int sectors = 30;
    Tpoints.clear();
    Obj.clear();

    double deltaPhi = M_PI / stacks;        // крок по вертикалі
    double deltaTheta = 2.0 * M_PI / sectors;

    for (int i = 0; i <= stacks; i++){
        double Phi = i * deltaPhi;
        for (int j = 0; j <= sectors; j++){
            double Theta = j * deltaTheta;
            Tpoints.push_back({(double)(r * sin(Phi) * cos(Theta)),
                               (double)(r * cos(Phi)),
                               (double)(r * sin(Phi) * sin(Theta))});
        }
    }
    for (int i = 0; i < stacks; i++) { // цикл по поверхах
        for (int j = 0; j < sectors; j++) { // цикл по меридіанах

            // k1 - це ліва верхня точка нашого "квадратика"
            int k1 = i * (sectors + 1) + j;

            // k2 - це точка прямо під нею (на наступному поверсі)
            int k2 = k1 + (sectors + 1);

            // Тепер у нас є 4 точки секції:
            // k1 --- k1+1
            // |       |
            // k2 --- k2+1

            // Додаємо два трикутники (розбиваємо квадрат діагоналлю)
            addObject(k1, k1 + 1, k2);     // Перший трикутник
            addObject(k1 + 1, k2 + 1, k2); // Другий трикутник
        }
    }
}


bool Mesh::loadFromVTK(QString filename){
    string path = filename.toStdString();
    ifstream file(path);
    if (!file.is_open())
        return false;
    string line;
    while (getline(file,line)){
        stringstream ss(line);
        string point;
        ss >> point;
        if(point == "POINTS"){
            int count;
            string dataType;
            ss >> count >> dataType;
            Tpoints.resize(count);
            for (int i = 0; i < count; i++){
                file >> Tpoints[i].x >> Tpoints[i].y >> Tpoints[i].z;
            }
        }
        else if (point == "POLYGONS"){
            Obj.clear();
            int objectNum, totalNum;
            ss >> objectNum >> totalNum;
            for (int i = 0; i < objectNum; i++){
                int n;
                file >> n;
                if (n == 3){
                    int v1, v2, v3;
                    file >> v1 >> v2 >> v3;
                    addObject(v1, v2, v3);
                }
                else {
                    int dummy;
                    for(int j=0; j<n; j++) file >> dummy;//безпечне викидання непотрібних чисел
                }
            }
        }
    }
    file.close();
    return !Obj.empty();
}


bool Mesh::saveToVTK(QString filename){
    string path = filename.toStdString();
    ofstream file(path);
    if (!file.is_open())
        return false;
    file << "# vtk DataFile Version 3.0" << "\n";
    file << "meow kocka" << "\n";
    file << "ASCII" << "\n";
    file << "DATASET POLYDATA" << "\n";
    file << "POINTS " << Tpoints.size() << " double" << "\n";

    for (const Vertex3D& p : Tpoints){
        file << p.x << " " << p.y << " " << p.z << endl;
    }

    file << "POLYGONS " << Obj.size() << " " << Obj.size() * 4 << endl;
    for (const Triangle& T : Obj){
        file << "3 "
             << T.v1 << " "
             << T.v2 << " "
             << T.v3 << endl;
    }
    file.close();
    return true;
}

void Mesh::setVectorNorm(int Thetta, int Phi){
    VectorNorm.clear();

    Vertex3D n = {sin(Thetta) * cos(Phi), sin(Thetta) * sin(Phi), cos(Thetta)};
    Vertex3D u = {sin(Thetta + M_PI / 2) * cos(Phi), sin(Thetta + M_PI / 2) * sin(Phi), cos(Thetta + M_PI / 2)};
    Vertex3D v = {n.y * u.z - n.z * u.y,
                  n.z * u.x - n.x * u.z,
                  n.x * u.y - n.y * u.x};
    VectorNorm.push_back(n);
    VectorNorm.push_back(u);
    VectorNorm.push_back(v);
}

QVector<Vertex3D> Mesh::mutation(const QVector<Vertex3D>& VectorNorm){
    QVector<Vertex3D> Mpoints;
    if (VectorNorm.size() < 3) return Tpoints;
    if (!Tpoints.isEmpty()){
        for(int i = 0; i < Tpoints.size(); i++){
            Vertex3D W = {Tpoints[i].x * VectorNorm[0].x +
                              Tpoints[i].y * VectorNorm[0].y +
                              Tpoints[i].z * VectorNorm[0].z,
                          Tpoints[i].x * VectorNorm[1].x +
                              Tpoints[i].y * VectorNorm[1].y +
                              Tpoints[i].z * VectorNorm[1].z,
                          Tpoints[i].x * VectorNorm[2].x +
                              Tpoints[i].y * VectorNorm[2].y +
                              Tpoints[i].z * VectorNorm[2].z};
            Mpoints.push_back(W);
        }
    }
    return Mpoints;
}
void Mesh::parallelProj(QVector<Vertex3D>& points, int d){
    double a = VectorNorm[0].x;
    double b = VectorNorm[0].y;
    double c = VectorNorm[0].z;
    double low = a * a + b * b + c * c;
    for (int i = 0; i < points.size(); i++){
        double high = a * points[i].x + b * points[i].y + c * points[i].z + d;
        points[i].x -= a * high / low;
        points[i].y -= b * high / low;
        points[i].z -= c * high / low;
    }
}


