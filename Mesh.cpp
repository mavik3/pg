#include "Mesh.h"
#include <string>
#include <fstream>
#include <sstream>
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
    for (const Triangle& T : getObj()){
        file << "3 "
             << T.v1 << " "
             << T.v2 << " "
             << T.v3 << endl;
    }
    file.close();
    return true;
}