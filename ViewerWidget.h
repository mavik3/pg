#pragma once
#include <QtWidgets>
class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);//velkost
	QImage* img = nullptr;// QPainter to iste ale ne budem pouzivat * 
	uchar* data = nullptr;// zapis baytov

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

    QVector<QPoint> polygonPoints;

    // чи вже є готовий об'єкт
    bool polygonFinished = false;   // чи завершений полігон

    bool draggingPolygon = false;
    QPoint lastMousePos = QPoint(0, 0);

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
    void redrawPolygon(const QColor& color, int algType);



    void rotation(double k);

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};
