#pragma once
#include <QtWidgets>

struct Vertex{
    QPoint pos;
    QColor color;
};

class ViewerWidget :public QWidget {
	Q_OBJECT
private:


    Vertex base_t0, base_t1, base_t2;

	QSize areaSize = QSize(0, 0);//velkost
	QImage* img = nullptr;// QPainter to iste ale ne budem pouzivat * 
	uchar* data = nullptr;// zapis baytov

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

    QVector<QPoint> polygonPoints;
    QVector<QPoint> originalPoints;

    // чи вже є готовий об'єкт
    bool polygonFinished = false;   // чи завершений полігон

    bool draggingPolygon = false;
    QPoint lastMousePos = QPoint(0, 0);

    bool fillEnabled = false;
    int currentFillType = 0;


public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, int r, int g, int b, int a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color);
	bool isInside(int x, int y);//do vnutra

	//Draw functions
	void drawLine(QPoint start, QPoint end, QColor color, int algType = 0);

	//Get/Set functions
	uchar* getData() { return data; }
    void setDataPtr() { data = img ? img->bits() : nullptr; }// якщо не задано скільки тоді 0

	int getImgWidth() { return img ? img->width() : 0; };
	int getImgHeight() { return img ? img->height() : 0; };

    void clear();

	//Algorithms
	void drawLineDDA(QPoint start, QPoint end, QColor color);
	void drawLineBresenham(QPoint start, QPoint end, QColor color);
    void drawLineCircle(QPoint center, QPoint radius, QColor color);
    void drawCirclePoints(int xc, int yc, int x, int y, QColor color);
    void drawPolygon(const QVector<QPoint>& pts, QColor color, int algLine, bool closed = true);


    QVector<QPoint>& getPolygonPoints() { return polygonPoints; }
    void clearPolygon() { polygonPoints.clear(); polygonFinished = false; }
    bool getPolygonFinished() { return polygonFinished; }
    void setPolygonFinished(bool s) { polygonFinished = s; }


    bool getDraggingPolygon() const { return draggingPolygon; }
    void setDraggingPolygon(bool state) { draggingPolygon = state; }

    QPoint getLastMousePos() const { return lastMousePos; }
    void setLastMousePos(QPoint p) { lastMousePos = p; }

    void movePolygon(int dx, int dy);
    void redrawPolygon(const QColor& color, int algType, bool scan);
    void fillTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);
    void Scale(double x, double y);
    void Shear(double pS, int algType);

    void rotation(double k);
    void OsSum();

    QVector<QPoint> calculateCyrusBeckLine(QPoint S, QPoint E);

    QVector<QPoint>& getOriginalPoints() { return originalPoints; }
    // Змінюємо функцію: тепер вона приймає вхідні точки і повертає відсічені
    QVector<QPoint> calculateClippedPolygon(const QVector<QPoint>& sourcePoints);

    void Scan_line(const QColor& color);

    void setScan(bool state){fillEnabled = state;}
    bool getScan(){return fillEnabled;}

    void setTriangleVertixes(Vertex t0, Vertex t1, Vertex t2) {
        base_t0 = t0;
        base_t1 = t1;
        base_t2 = t2;
    }

    void setFillEnabled(bool enabled) { fillEnabled = enabled; }
    void setFillType(int type) { currentFillType = type; }


    void fillButtomTriangle(Vertex t0, Vertex t1, Vertex t2);
    void fillTrianglePart(int y1, int y2, double x1, double x2, double w1, double w2, int fillType);
    void fillBottomTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);
    void fillTopTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType);

    QColor getNearestColor(int x, int y, Vertex t0, Vertex t1, Vertex t2);
    QColor getBarycentricColor(int x, int y, Vertex t0, Vertex t1, Vertex t2);
    QColor getColor(int x, int y, int fillType);

    void updateTriangleLogic();


public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};
