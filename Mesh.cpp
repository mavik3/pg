#include "Mesh.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <QRandomGenerator>
void Mesh::createCube(double a){
    Tpoints.clear();
    Obj.clear();
    double half = a / 2.0;
    for (int z = 0; z <= 1; z++) {
        for (int y = 0; y <= 1; y++) {
            for (int x = 0; x <= 1; x++) {
                double px = (x == 0) ? -half : half;
                double py = (y == 0) ? -half : half;
                double pz = (z == 0) ? -half : half;
                Tpoints.push_back({px, py, pz});
            }
        }
    }/*
    Tpoints.push_back({-half,-half,-half});
    Tpoints.push_back({half,-half,-half});
    Tpoints.push_back({-half,half,-half});
    Tpoints.push_back({half,half,-half});
    Tpoints.push_back({-half,-half,half});
    Tpoints.push_back({half,-half,half});
    Tpoints.push_back({-half,half,-half});
    Tpoints.push_back({half,half,-half});
    Tpoints.push_back({0,0,-half});
    Tpoints.push_back({0,0,half});
    Tpoints.push_back({-half,0,0});
    Tpoints.push_back({half,0,-half});
    Tpoints.push_back({0,-half,0});
    Tpoints.push_back({0,half,0});
 */
    //передня
    addObject(0, 2, 3,Obj);
    addObject(0, 3, 1,Obj);

    // Задня грань (Z = +half)
    addObject(4, 5, 7,Obj);
    addObject(4, 7, 6,Obj);

    // Ліва грань (X = -half)
    addObject(0, 4, 6,Obj);
    addObject(0, 6, 2,Obj);

    // Права грань (X = +half)
    addObject(1, 3, 7,Obj);
    addObject(1, 7, 5,Obj);

    // Верхня грань (Y = +half)
    addObject(2, 6, 7,Obj);
    addObject(2, 7, 3,Obj);

    // Нижня грань (Y = -half)
    addObject(0, 1, 5,Obj);
    addObject(0, 5, 4,Obj);
}
void Mesh::createArray(double a, int y, int kus){
    Spoints.clear();
    Array.clear();
    for (int i = 0; i < kus; i++){
        for (int j = 0; j < kus; j++){
            double Sx = -a / 2 + a / (kus - 1) * j;
            double Sz = (double)y;
            double Sy = -a / 2 + a / (kus - 1) * i;
            Spoints.append({Sx,Sz,Sy});
        }
    }

    for (int i = 0; i < kus - 1; i++){
        for(int k = 0; k < kus - 1; k++){
            //double Sx = (k % 2 == 0) ? -a / 2 + a * i : -a * 2 + a / 4 * (j + k);
            //double Sz = (k / 2 == 0) ? a / 2 + a / 4 * i : -a / 4 + a / 4 * i;
            //double Sy = (double)y;
            addObject(kus * i + k,kus * i + (k + 1),(i + 1) * kus + k,Array);
            addObject((k + 1) + kus * i, (k + 1) + (i + 1) * kus, (i + 1) * kus + k, Array);

        }
    }
}

void Mesh::createSphere(double r, int stacks){

    int sectors = stacks;
    Tpoints.clear();
    Obj.clear();
    //rozdelenie na casti
    double deltaPhi = M_PI / stacks; //po verticale
    double deltaTheta = 2.0 * M_PI / sectors; //po horizontale

    for (int i = 0; i <= stacks; i++){
        double Phi = i * deltaPhi;
        for (int j = 0; j <= sectors; j++){
            double Theta = j * deltaTheta;
            Tpoints.push_back({(double)(r * sin(Phi) * cos(Theta)),
                               (double)(r * cos(Phi)),
                               (double)(r * sin(Phi) * sin(Theta))});
        }
    }
    for (int i = 0; i < stacks; i++) { // horizontal
        for (int j = 0; j < sectors; j++) { // meredian

            int k1 = i * (sectors + 1) + j;

            int k2 = k1 + (sectors + 1);

            // k1 --- k1+1
            // |       |
            // k2 --- k2+1
            //dva trojuholnika
            addObject(k1, k1 + 1, k2,Obj);
            addObject(k1 + 1, k2 + 1, k2,Obj);
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
                    addObject(v1, v2, v3,Obj);
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
    file << "POINTS " << Spoints.size() << " double" << "\n";

    for (const Vertex3D& p : Spoints){
        file << p.x << " " << p.y << " " << p.z << endl;
    }

    file << "POLYGONS " << Array.size() << " " << Array.size() * 4 << endl;
    for (const Triangle& T : Array){
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


    double radThetta = Thetta * M_PI / 180.0;
    double radPhi = Phi * M_PI / 180.0;

    Vertex3D n = {sin(radThetta) * sin(radPhi),
                  sin(radThetta) * cos(radPhi),
                  cos(radThetta)};
    Vertex3D u = {sin(radThetta + M_PI / 2) * sin(radPhi),
                  sin(radThetta + M_PI / 2) * cos(radPhi),
                  cos(radThetta + M_PI / 2)};
    Vertex3D v = {n.y * u.z - n.z * u.y,
                  n.z * u.x - n.x * u.z,
                  n.x * u.y - n.y * u.x};
    VectorNorm.push_back(n);
    VectorNorm.push_back(u);
    VectorNorm.push_back(v);
}

QVector<Vertex3D> Mesh::mutation(const QVector<Vertex3D>& VectorNorm, const QVector<Vertex3D>& points){
    QVector<Vertex3D> Mpoints;
    if (VectorNorm.size() < 3) return points;
    if (!points.isEmpty()){
        for(int i = 0; i < points.size(); i++){
            Vertex3D W = {points[i].x * VectorNorm[0].x +
                              points[i].y * VectorNorm[0].y +
                              points[i].z * VectorNorm[0].z,
                          points[i].x * VectorNorm[1].x +
                              points[i].y * VectorNorm[1].y +
                              points[i].z * VectorNorm[1].z,
                          points[i].x * VectorNorm[2].x +
                              points[i].y * VectorNorm[2].y +
                              points[i].z * VectorNorm[2].z};
            Mpoints.push_back(W);
        }
    }
    return Mpoints;
}
QVector<Vertex3D> Mesh::parallelProj(QVector<Vertex3D>& points){

    QVector<Vertex3D> Parallel;
    for (int i = 0; i < points.size(); i++){
        Parallel.push_back({points[i].x,points[i].y,points[i].z});}

    return Parallel;
}

QVector<Vertex3D> Mesh::perspectiveProj(QVector<Vertex3D>& points, int d) {
    QVector<Vertex3D> Perspective;
    for (int i = 0; i < points.size(); i++) {

        double z_coords = points[i].z;

        double divisor = (d - z_coords);

        if (qAbs(divisor) > 0.0001) {
            Perspective.push_back({points[i].x * d / divisor, points[i].y * d / divisor,points[i].z});
        }
    }
    return Perspective;
}


