 #include   "ViewerWidget.h"
#include <cmath>
#include <QtMath>
#include <algorithm>
#include <QVector2D>

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);//відстеження мищі при не натисканні
    if (imgSize != QSize(0, 0)) {//якщо поле не 0 0 то робимо нове
        img = new QImage(imgSize, QImage::Format_ARGB32);// A oposity R G B  color
        img->fill(Qt::white); //заповнює поле білим
        resizeWidget(img->size());//робить розмір віджета рівним розміру картинки
        setDataPtr(); //записує кількість байтів
	}
}
ViewerWidget::~ViewerWidget()
{
    delete img;//звільняє память
    img = nullptr;//видаляє вказівник
    data = nullptr;//те саме
}
void ViewerWidget::resizeWidget(QSize size)
{
    this->resize(size);//херачить новий розмір вікна
    this->setMinimumSize(size);
	this->setMaximumSize(size);
    //фіксований розмір
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img) {
        ViewerWidget::~ViewerWidget();//якщо старе поле існує воно його обнуляє
	}
    img = new QImage(inputImg.convertToFormat(QImage::Format_ARGB32));//бере вхідну картинку та конвертує її в ARGB32і створює копію динамічну
	if (!img || img->isNull()) {
        return false;//якщо не вдалось або ж пустий то повертає фолс
	}
    resizeWidget(img->size());//уєбали під наш імедж новий розмір
    setDataPtr();//змінює поточні данні щодо кількості батйів
    update();//премальовує наш образок на нвоий скачаний чи ще щось

	return true;
}
bool ViewerWidget::isEmpty()
{
    if (img == nullptr) {
        return true;//ked prazdny obrazok
	}

	if (img->size() == QSize(0, 0)) {
        return true;//alebo size ho je 0 tak vrati true
	}
    return false;//ak nie tak false
}

bool ViewerWidget::changeSize(int width, int height)
{
    QSize newSize(width, height);//величина картини

    if (newSize != QSize(0, 0)) {// якщо розмір не 0 0
        if (img != nullptr) { // якщо стара картина ше є
            delete img;// видаляємо стару
		}

        img = new QImage(newSize, QImage::Format_ARGB32);//створюємо нову
        if (!img || img->isNull()) {//якщо порожня
            return false;//повертаємо фолс
		}
        img->fill(Qt::white);//запонює білим
        resizeWidget(img->size());// бере правильні розміри
        setDataPtr();//вказуємо нові розміри
        update();//просить перемалювати картину
	}

	return true;
}

void ViewerWidget::setPixel(int x, int y, int r, int g, int b, int a)
{
    if (!img || !data) return;//якщо картинки немає то повертає
    if (!isInside(x, y)) return;//якщо кординати поза межами повертає
    //кольори
    r = r > 255 ? 255 : (r < 0 ? 0 : r);//якщо r більше 255 то r = 255 інакше якщо r менше 0 то r = 0 інакше r = r
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

    size_t startbyte = y * img->bytesPerLine() + x * 4;//кожен рядок займає img->bytesPerLines()//чому ж х*4 бо ARGB 4 байти
    //виходить (зсув до рядка) + (зсув у рядку)
    data[startbyte] = static_cast<uchar>(b);//все навпаки йде як BGRA
	data[startbyte + 1] = static_cast<uchar>(g);
	data[startbyte + 2] = static_cast<uchar>(r);
	data[startbyte + 3] = static_cast<uchar>(a);
}
//перевантаження setPixel 0<(A,R,G,B)<1;; R = 1 макс чевоний;; 0 мін
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{

    valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);//якщо більще 1 то = 1 інакще якщо менше 0 то = 0 інакше ValR = ValR
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);
    //переводимо з 0 1 до 0 255 заокруглюємо 0,5 кожне до нашого setPixel
	setPixel(x, y, static_cast<int>(255 * valR + 0.5), static_cast<int>(255 * valG + 0.5), static_cast<int>(255 * valB + 0.5), static_cast<int>(255 * valA + 0.5));
}
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (color.isValid()) {
		setPixel(x, y, color.red(), color.green(), color.blue(), color.alpha());
	}
}//якщо норм колір то ставим цей

bool ViewerWidget::isInside(int x, int y)
{
	return img && x >= 0 && y >= 0 && x < img->width() && y < img->height();
}//перевірка попорядку якщо хоч одна не спилнена то false

//Draw functions
void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int algType)
{
	if (!img || !data) return;

    if (algType == 0) {
		drawLineDDA(start, end, color);
	}
    else if (algType == 1){
		drawLineBresenham(start, end, color);
	}
    else {
        drawLineCircle(start, end, color);
    }
    update();

	//Po implementovani drawLineDDA a drawLineBresenham treba vymazat
    /*QPainter painter(img);
	painter.setPen(QPen(color));
    painter.drawLine(start, end);
    update();//малює лінію і обновляє*/
}

void ViewerWidget::clear() {
    if (!img) return;
    img->fill(Qt::white);
    originalPoints.clear(); // Очищаємо оригінал
    polygonPoints.clear();  // Очищаємо робочий вектор
    polygonFinished = false;
    update();
}

void ViewerWidget::drawLineDDA(QPoint start, QPoint end, QColor color)
{
    if (!img || !data) return;

    int x0 = start.x(), y0 = start.y();
    int x1 = end.x(),   y1 = end.y();

    int dxI = x1 - x0;
    int dyI = y1 - y0;

    if (dxI == 0 && dyI == 0) {
        setPixel(x0, y0, color);
        return;
    }

    if (dxI == 0) {
        int yStep = (y1 >= y0) ? 1 : -1;
        for (int y = y0; y != y1 + yStep; y += yStep)
            setPixel(x0, y, color);
        return;
    }

    if (dyI == 0) {
        int xStep = (x1 >= x0) ? 1 : -1;
        for (int x = x0; x != x1 + xStep; x += xStep)
            setPixel(x, y0, color);
        return;
    }

    double dx = (double)dxI;
    double dy = (double)dyI;
    double m  = dy / dx;

    if (qAbs(m) <= 1.0) {

        int xStep = (x1 >= x0) ? 1 : -1;
        double x = x0;
        double y = y0;

        while (true) {
            int xi = (int)(x + 0.5);
            int yi = (int)(y + 0.5);
            setPixel(xi, yi, color);

            if (xi == x1) break;

            x += xStep;
            y += m * xStep; // напрямок враховано
        }
    } else {

        int yStep = (y1 >= y0) ? 1 : -1;
        double x = x0;
        double y = y0;

        double invM = dx / dy;

        while (true) {
            int xi = (int)(x + 0.5);
            int yi = (int)(y + 0.5);
            setPixel(xi, yi, color);

            if (yi == y1) break;

            y += yStep;
            x += invM * yStep; // напрямок враховано
        }
    }
}
void ViewerWidget::drawLineBresenham(QPoint start, QPoint end, QColor color){

//pracujeme z jednym oktantom a preto mozem pisat tak (0 <m < 1) || (m > 1)
    int x1 = start.x();
    int y1 = start.y();
    int x2 = end.x();
    int y2 = end.y();

    // 1. Vypocet rozdielov
    int dx = x2 - x1;
    int dy = y2 - y1;

    // 2. Urcenie smeru (namiesto kontroly m > 0 alebo m < 0)
    int smer_x = (x2 >= x1) ? 1 : -1; //dx>0
    int smer_y = (y2 >= y1) ? 1 : -1; //dy>0

    // 3. Absolutne hodnoty (pre vypocet parametra p)
    int adx = std::abs(dx);
    int ady = std::abs(dy);

    // 4. Rozdelenie len na dva pripady podla sklonu (ciara je plocha alebo strma)
    if (adx >= ady) {
        // RIADIACA OS JE X (-1 < m < 1)
        int p = 2 * ady - adx;
        int k1 = 2 * ady;
        int k2 = 2 * (ady - adx);

        for (int i = 0; i <= adx; i++) {
            setPixel(x1, y1, color);
            x1 += smer_x; // Posun v smere x
            if (p > 0) {
                y1 += smer_y; // Posun v smere y len ak treba
                p += k2;
            }
            else {
                p += k1;
            }
        }
    }
    else {
        // RIADIACA OS JE Y ( < -1  1 <)
        int p = 2 * adx - ady;
        int k1 = 2 * adx;
        int k2 = 2 * (adx - ady);

        for (int i = 0; i <= ady; i++) {
            setPixel(x1, y1, color);
            y1 += smer_y; // Posun v smere sy
            if (p > 0) {
                x1 += smer_x; // Posun v smere sx
                p += k2;
            }
            else {
                p += k1;
            }
        }
    }
}
void ViewerWidget::drawLineCircle(QPoint center, QPoint radius, QColor color){
    int xc = center.x(), yc = center.y();
    int xr = radius.x(), yr = radius.y();

    int r = sqrt(pow(xr - xc,2) + pow(yr - yc,2));
    int twoX = 3;
    int twoY = 2 * r - 2;
    int x = 0;
    int y = r;

    int p0 = 1 - r;
    while(x <= y){
        drawCirclePoints(xc, yc, x, y, color);
        if(p0 > 0){ p0 -= twoY; y--; twoY -= 2; }
        //else{ p0 += 2 * x + 3; }
        p0 +=twoX;
        twoX += 2;
        x += 1;
    }
}
void ViewerWidget::drawCirclePoints(int xc, int yc, int x, int y, QColor color)
{
    setPixel(xc + x, yc + y, color);
    setPixel(xc - x, yc + y, color);
    setPixel(xc + x, yc - y, color);
    setPixel(xc - x, yc - y, color);
    setPixel(xc + y, yc + x, color);
    setPixel(xc - y, yc + x, color);
    setPixel(xc + y, yc - x, color);
    setPixel(xc - y, yc - x, color);
}
void ViewerWidget::drawPolygon(const QVector<QPoint>& pts, QColor color, int algType, bool closed) {
    if (pts.size() < 2) return;

    for (int i = 0; i < pts.size() - 1; ++i) {
        drawLine(pts[i], pts[i+1], color, algType);
    }

    if (closed && pts.size() >= 3) {
        drawLine(pts.last(), pts.first(), color, algType); // Замикаємо полігон правильно
    }
}

void ViewerWidget::redrawPolygon(const QColor& color, int algType, bool scan)
{
    if (!img) return;
    img->fill(Qt::white);

    // ВИПАДОК 1: Малюємо окремі лінії (поки полігон не завершено ПКМ)
    if (!polygonFinished) {
        if (originalPoints.size() >= 2) {
            // Використовуємо Сайрус-Бек для кожного відрізка в ланцюжку
            for (int i = 0; i < originalPoints.size() - 1; i++) {
                QVector<QPoint> clipped = calculateCyrusBeckLine(originalPoints[i], originalPoints[i+1]);
                if (clipped.size() == 2) {
                    drawLine(clipped[0], clipped[1], color, algType);
                }
            }
        }
    }
    // ВИПАДОК 2: Малюємо готовий полігон (після натискання ПКМ)
    else {
        // Використовуємо Сазерленд-Ходжман (він повертає замкнений вектор)
        QVector<QPoint> clippedPoly = calculateClippedPolygon(originalPoints);

        if (!clippedPoly.isEmpty()) {
            drawPolygon(clippedPoly, color, algType, true); // true = замикати

            // Якщо увімкнено заповнення
            if (scan && clippedPoly.size() >= 3) {
                QVector<QPoint> backup = polygonPoints;
                polygonPoints = clippedPoly;
                Scan_line(color);
                polygonPoints = backup;
            }
        }
    }
    update();
}


void ViewerWidget::rotation(double k){
    if(originalPoints.size() < 2 || !img) return;

    double rad = k * M_PI / 180.0;
    QPoint center = originalPoints[0];
    for (QPoint& p : originalPoints){
        double x = p.x() - center.x();
        double y = p.y() - center.y();


        double xr = x * cos(rad) - y * sin(rad) + center.x();
        double yr = x * sin(rad) + y * cos(rad) + center.y();
        p.setX(xr);
        p.setY(yr);
    }

}


void ViewerWidget::Scale(double sx, double sy)
{
    if (originalPoints.isEmpty() || !img) return;

    double cx = 0.0;
    double cy = 0.0;

    for (const QPoint& p : originalPoints) {
        cx += p.x();
        cy += p.y();
    }

    cx /= originalPoints.size();
    cy /= originalPoints.size();
    for (QPoint& p : originalPoints) {
    if (sx != 0) { double nx = cx + (p.x() - cx) * sx; p.setX(nx); }
    if (sy != 0) { double ny = cy + (p.y() - cy) * sy; p.setY(ny); }
}
    /*if (polygonPoints.size() == 1) {
        QPoint c = polygonPoints[0];

        double s = std::max(std::abs(sx), std::abs(sy));
        if (s <= 0.0) s = 1.0;

        int r = qMax(1, qRound(20.0 * s)); // 20 - базовий радіус

        polygonPoints.push_back(QPoint(c.x() + r, c.y()));
        return;
    }*/
}
void ViewerWidget::Shear(double pS,int algType){
    if (originalPoints.size() < 2 || !img) return;

    for (QPoint& p : originalPoints) {
        if (algType == 0) { // Shear X
            p.setX(p.x() + qRound(p.y() * pS));
        } else { // Shear Y
            p.setY(p.y() + qRound(p.x() * pS));
        }
    }
}

void ViewerWidget::movePolygon(int dx, int dy) {
    for (QPoint& p : originalPoints) { // Рухаємо тільки оригінал!
        p.setX(p.x() + dx);
        p.setY(p.y() + dy);
    }
}


void ViewerWidget::OsSum(){
    if (originalPoints.size() < 2 || !img) return;
   // if (polygonPoints.size() == 2){}
  //  else if (polygonPoints.size() > 2){}
    int x1 = originalPoints[0].x(), x2 = originalPoints[1].x();
        int y1 = originalPoints[0].y(), y2 = originalPoints[1].y();
        int Vx = x2 - x1, Vy = y2 - y1;
        double a = Vy, b = -Vx, c = - a * x1 - b * y1;

        if (originalPoints.size() > 2){
            for (int i = 1; i < originalPoints.size(); i++){
            double xN = originalPoints[i].x() - 2 * a * ((a * originalPoints[i].x() + b * originalPoints[i].y() + c) / (a * a + b * b));
            double yN = originalPoints[i].y() - 2 * b * ((a * originalPoints[i].x() + b * originalPoints[i].y() + c) / (a * a + b * b));
            originalPoints[i].setX(xN), originalPoints[i].setY(yN);
        }
        }
        else if (originalPoints.size() == 2){
            originalPoints[1].setX(x2);
            originalPoints[1].setY(2 * y1 - y2);
        }

}


QVector<QPoint> ViewerWidget::calculateCyrusBeckLine(QPoint S, QPoint E)
{
    QVector<QPoint> viewerWidgetEdges;
    viewerWidgetEdges.push_back(QPoint(0, 0));
    viewerWidgetEdges.push_back(QPoint(0, img->height()));
    viewerWidgetEdges.push_back(QPoint(img->width(), img->height()));
    viewerWidgetEdges.push_back(QPoint(img->width(), 0));

    double tL = 0.0;
    double tU = 1.0;
    QPoint d(polygonPoints[1].x() -polygonPoints[0].x(), polygonPoints[1].y() - polygonPoints[0].y());

    qsizetype i = 0;
    while (i < viewerWidgetEdges.size())
    {
        QPoint normal;
        if(i == viewerWidgetEdges.size() - 1) // upper edge
        {
            normal.setX(viewerWidgetEdges.back().y() - viewerWidgetEdges.front().y());
            normal.setY(-(viewerWidgetEdges.back().x() - viewerWidgetEdges.front().x()));
        }
        else // other edges
        {
            normal.setX(viewerWidgetEdges[i + 1].y() - viewerWidgetEdges[i].y());
            normal.setY(-(viewerWidgetEdges[i + 1].x() - viewerWidgetEdges[i].x()));
        }
        QPoint w(polygonPoints[0].x() - viewerWidgetEdges[i].x(), polygonPoints[0].y() - viewerWidgetEdges[i].y());

        int dn = d.x() * normal.x() + d.y() * normal.y();
        int wn = w.x() * normal.x() + w.y() * normal.y();

        double t = -wn / (double)dn;

        if (dn != 0)
        {
            if (dn > 0 && t <= 1)
                tL = qMax(t, tL);
            else if (dn < 0 && t >= 0)
                tU = qMax(t, tU);
        }
        i++;
    }

    if (tL == 0.0 && tU == 1.0)
    {
        if (!(polygonPoints[0].x() <= img->width() && polygonPoints[0].x() >= 0 && polygonPoints[0].y() <= img->height() && polygonPoints[0].y() >= 0))
            return polygonPoints;
    }
    else if (tL > 0 && tL < 1 && tU > 0 && tU < 1)
    {
        QVector<QPoint> clippedPoints;
        clippedPoints.push_back(QPoint(polygonPoints[0].x() + ( polygonPoints[1].x() - polygonPoints[0].x() ) * tL + 0.5, polygonPoints[0].y() + (polygonPoints[1].y() - polygonPoints[0].y()) * tL + 0.5));
        clippedPoints.push_back(QPoint(polygonPoints[0].x() + (polygonPoints[1].x() - polygonPoints[0].x()) * tU + 0.5, polygonPoints[0].y() + (polygonPoints[1].y() - polygonPoints[0].y()) * tU + 0.5));
        return clippedPoints;
    }

    return polygonPoints;
}
// Функція тепер повертає результат, а не змінює вхідні дані
QVector<QPoint> ViewerWidget::calculateClippedPolygon(const QVector<QPoint>& sourcePoints) {
    if (sourcePoints.size() < 3) return sourcePoints;

    QVector<QPoint> V = sourcePoints;
    QVector<QPoint> W;
    int xmax = img->width() - 1;
    int ymax = img->height() - 1;

    for (int border = 0; border < 4; border++) {
        if (V.isEmpty()) return {};
        W.clear();
        QPoint S = V.last();
        for (int i = 0; i < V.size(); i++) {
            QPoint P = V[i];
            bool Sin, Pin;
            if (border == 0) { Sin = (S.x() >= 0); Pin = (P.x() >= 0); }
            else if (border == 1) { Sin = (S.x() <= xmax); Pin = (P.x() <= xmax); }
            else if (border == 2) { Sin = (S.y() >= 0); Pin = (P.y() >= 0); }
            else { Sin = (S.y() <= ymax); Pin = (P.y() <= ymax); }

            if (Pin) {
                if (!Sin) { // Outside -> Inside
                    double t;
                    if (border < 2) t = (double)((border==0?0:xmax) - S.x()) / (P.x() - S.x());
                    else t = (double)((border==2?0:ymax) - S.y()) / (P.y() - S.y());
                    W.append(QPoint(qRound(S.x() + t*(P.x()-S.x())), qRound(S.y() + t*(P.y()-S.y()))));
                }
                W.append(P);
            } else if (Sin) { // Inside -> Outside
                double t;
                if (border < 2) t = (double)((border==0?0:xmax) - S.x()) / (P.x() - S.x());
                else t = (double)((border==2?0:ymax) - S.y()) / (P.y() - S.y());
                W.append(QPoint(qRound(S.x() + t*(P.x()-S.x())), qRound(S.y() + t*(P.y()-S.y()))));
            }
            S = P;
        }
        V = W;
    }
    return V;
}
void ViewerWidget::Scan_line(const QColor& color)
{
    if (!img || polygonPoints.size() < 3)
        return;

    int ymin = polygonPoints[0].y();
    int ymax = polygonPoints[0].y();

    // знайти вертикальні межі полігона
    for (const QPoint& p : polygonPoints) {
        if (p.y() < ymin) ymin = p.y();
        if (p.y() > ymax) ymax = p.y();
    }

    // обрізка по межах зображення
    ymin = std::max(0, ymin);
    ymax = std::min(img->height() - 1, ymax);

    // проходимо по кожному scan-line
    for (int y = ymin; y <= ymax; y++) {
        QVector<int> xYes;

        // шукаємо всі перетини рядка y з ребрами полігона
        for (int i = 0; i < polygonPoints.size(); i++) {
            QPoint p1 = polygonPoints[i];
            QPoint p2 = polygonPoints[(i + 1) % polygonPoints.size()];

            // горизонтальні ребра пропускаємо
            if (p1.y() == p2.y())
                continue;

            // впорядковуємо по y
            if (p1.y() > p2.y())
                std::swap(p1, p2);

            // правило: включаємо нижню вершину, не включаємо верхню
            if (y >= p1.y() && y < p2.y()) {
                double x = p1.x() + (double)(y - p1.y()) * (p2.x() - p1.x()) / (double)(p2.y() - p1.y());

                xYes.push_back(qRound(x));
            }
        }

        // сортуємо всі x-перетини
        std::sort(xYes.begin(), xYes.end());

        // зафарбовуємо попарно
        for (int i = 0; i + 1 < xYes.size(); i += 2) {
            int xStart = xYes[i];
            int xEnd   = xYes[i + 1];

            if (xStart > xEnd)
                std::swap(xStart, xEnd);

            xStart = std::max(0, xStart);
            xEnd   = std::min(img->width() - 1, xEnd);

            for (int x = xStart; x <= xEnd; x++) {
                setPixel(x, y, color);
            }
        }
    }
}
//void ViewerWidget::Triangle(const QVector<QPoint>& T){







//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)//головна функція яку викликає qt
{
    QPainter painter(this);//створюється qpainter який буде малювати на цьому віджеті
    if (!img || img->isNull()) return;//не створений або порожній

    QRect area = event->rect();// прямокутник, оптимізація  "пошкодженої частини", не завжди треба перемальовувати весь
	painter.drawImage(area, *img, area);//vykresli novy obrazok
}
