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

    for (int i = 0; i < pts.size() - 1; i++) {
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
        if (originalPoints.size() > 1) {
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
        QVector<QPoint> clipped = calculateClippedPolygon(originalPoints);

        if (clipped.isEmpty()) return;

        if (fillEnabled && originalPoints.size() == 3){
              /*  QVector<QPoint> backup = polygonPoints;
                polygonPoints = clipped;
                Scan_line(color);
                polygonPoints = backup;
            }
            else if (originalPoints.size() == 3 && clipped.size() == 3) {
                    // Викликаємо твій спеціальний метод для трикутника
                    // ТУТ ми передаємо currentFillType, який прийшов з UI
                    Vertex v0 = { clipped[0], base_t0.color };
                    Vertex v1 = { clipped[1], base_t1.color };
                    Vertex v2 = { clipped[2], base_t2.color };
                    fillTriangle(base_t0, base_t1, base_t2, currentFillType);
                }
            }*/
            base_t0.pos = originalPoints[0];
            base_t1.pos = originalPoints[1];
            base_t2.pos = originalPoints[2];
                    this->polygonPoints = clipped; // Даємо Scan-line обрізані межі
                    Scan_line(color);             // Запускаємо заповнення
                } else if (fillEnabled) {
            this->polygonPoints = clipped;
            Scan_line(color);

                }

    for (int i = 0; i < clipped.size(); i++) {
        drawLine(clipped[i], clipped[(i + 1) % clipped.size()], color, algType);

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


QVector<QPoint> ViewerWidget::calculateCyrusBeckLine(QPoint P1, QPoint P2)
{
    double tL = 0.0;
    double tU = 1.0;
    QVector2D d(P2 - P1);

    int xmax = img->width() - 1;
    int ymax = img->height() - 1;

    // Нормалі та точки на межах екрана (внутрішні нормалі)
    struct Boundary { QPoint Ei; QVector2D ni; };
    QVector<Boundary> boundaries = {
        {QPoint(0, 0),    QVector2D(1, 0)},  // Ліва
        {QPoint(xmax, 0), QVector2D(-1, 0)}, // Права
        {QPoint(0, 0),    QVector2D(0, 1)},  // Верхня
        {QPoint(0, ymax), QVector2D(0, -1)}  // Нижня
    };

    for (const auto& edge : boundaries) {
        QVector2D w(P1 - edge.Ei);

        double dn = QVector2D::dotProduct(d, edge.ni);
        double wn = QVector2D::dotProduct(w, edge.ni);

        if (dn != 0) {
            double t = -wn / dn;
            if (dn > 0) tL = std::max(t, tL);

            else tU = std::min(tU, t);
        } else if (wn < 0) return {}; // Паралельно і зовні
    }

    if (tL <= tU) {
        return { QPoint(qRound(P1.x() + tL * d.x()), qRound(P1.y() + tL * d.y())),
                QPoint(qRound(P1.x() + tU * d.x()), qRound(P1.y() + tU * d.y())) };
    }
    return {};
}
// Функція тепер повертає результат, а не змінює вхідні дані
QVector<QPoint> ViewerWidget::calculateClippedPolygon(const QVector<QPoint>& sourcePoints) {
    if (sourcePoints.size() < 3) return sourcePoints;

    QVector<QPoint> V = sourcePoints; // vytvorme pole vrcholov V
    int xmax = img->width() - 1;
    int ymax = img->height() - 1;

    // Робимо це для всіх 4-х меж (ліва, права, верхня, нижня)
    for (int border = 0; border < 4; border++) {
        if (V.isEmpty()) return {};

        QVector<QPoint> W;     // vytvor prázdne pole W
        QPoint S = V.last();   // do bodu S uložíme posledný vrchol Vn-1

        // opakuj pre 0 <= i < n:
        for (int i = 0; i < V.size(); i++) {
            QPoint P = V[i];   // Це наш Vi з конспекту

            // Визначаємо, чи знаходяться точки всередині поточної межі
            bool Sin, Pin;
            if (border == 0)      { Sin = (S.x() >= 0);    Pin = (P.x() >= 0); }    // Ліва
            else if (border == 1) { Sin = (S.x() <= xmax); Pin = (P.x() <= xmax); } // Права
            else if (border == 2) { Sin = (S.y() >= 0);    Pin = (P.y() >= 0); }    // Верхня
            else                  { Sin = (S.y() <= ymax); Pin = (P.y() <= ymax); } // Нижня

            // ТУТ ПОЧИНАЄТЬСЯ ЛОГІКА З ТВОГО КОНСПЕКТУ:

            if (Pin) { // ak Vi,x >= xmin (Поточна точка P ВСЕРЕДИНІ)

                if (Sin) {
                    // ak Sx >= xmin (Попередня точка S теж ВСЕРЕДИНІ)
                    // Випадок: ВНУТРІ -> ВНУТРІ
                    W.append(P); // tak pridaj Vi do W
                }
                else {
                    // inak (Попередня точка S ЗОВНІ)
                    // Випадок: ЗОВНІ -> ВНУТРІ

                    // vypočítaj priesečník Pi
                    double t = (border < 2) ? (double)((border==0?0:xmax) - S.x()) / (P.x() - S.x())
                                            : (double)((border==2?0:ymax) - S.y()) / (P.y() - S.y());
                    QPoint priesecnik(qRound(S.x() + t*(P.x()-S.x())), qRound(S.y() + t*(P.y()-S.y())));

                    W.append(priesecnik); // pridaj ho (Pi) do W
                    W.append(P);          // a následne pridaj do W aj bod Vi
                }
            }
            else { // inak (Поточна точка P ЗОВНІ)

                if (Sin) {
                    // ak Sx >= xmin (Попередня точка S ВСЕРЕДИНІ)
                    // Випадок: ВНУТРІ -> ЗОВНІ

                    // vypočítaj priesečník Pi
                    double t = (border < 2) ? (double)((border==0?0:xmax) - S.x()) / (P.x() - S.x())
                                            : (double)((border==2?0:ymax) - S.y()) / (P.y() - S.y());
                    QPoint priesecnik(qRound(S.x() + t*(P.x()-S.x())), qRound(S.y() + t*(P.y()-S.y())));

                    W.append(priesecnik); // pridaj ho (Pi) do W
                }
                // inak: (Обидві точки зовні. Випадок: ЗОВНІ -> ЗОВНІ).
                // Не робимо нічого (žiadna akcia).
            }

            S = P; // aktualizuj bod S = Vi
        }
        V = W; // Результат обрізки однією межею стає вхідними даними для наступної
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
                if (originalPoints.size() == 3) {
                    // Ми використовуємо твою функцію getColor, яка вміє
                    // рахувати барицентричні координати (градієнт)
                    QColor c = getColor(x, y, currentFillType);
                    setPixel(x, y, c);
                } else {
                    setPixel(x, y, color); // Якщо це звичайний багатокутник
                }
            }
        }
    }
}
void ViewerWidget::fillTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType) {
    base_t0 = t0;
    base_t1 = t1;
    base_t2 = t2;

    std::vector<Vertex> points = { t0, t1, t2 };

    std::sort(points.begin(), points.end(), [](const Vertex& a, const Vertex& b) {
        if (a.pos.y() != b.pos.y()) { //ak y sa nerovnaju - teda mozme ich porovnavat <>
            return a.pos.y() < b.pos.y(); //tak primarne usporiadame podla y
        }
        else { //ak nemozme  rozhodnut, ako usporiadat podla y, pretoze sa rovnaju
            return a.pos.x() < b.pos.x(); //sekindarne podla x
        }
    });

    t0 = points[0];
    t1 = points[1];
    t2 = points[2];

    if (t0.pos.y() == t1.pos.y()) {
        //pripad: vodorovna horna hrana
        fillBottomTriangle(t0, t1, t2, fillType);
    }
    else if (t1.pos.y() == t2.pos.y()) {
        //pripad: vodorovna spodna hrana
        fillTopTriangle(t0, t1, t2, fillType);
    }
    else {
        QPoint pos_p;
        pos_p = QPoint((t0.pos.x()+(t1.pos.y()- t0.pos.y())*((double)(t2.pos.x()- t0.pos.x())/(t2.pos.y() - t0.pos.y()))), t1.pos.y());
        Vertex p = {pos_p, t1.color};

        if (t1.pos.x() < p.pos.x()) {
            fillTopTriangle(t0, t1, p, fillType);
            fillBottomTriangle(t1, p, t2, fillType);
        }
        else {
            fillTopTriangle(t0, p, t1, fillType);
            fillBottomTriangle(p, t1, t2, fillType);
        }
    }
}

void ViewerWidget::fillTrianglePart(int y1, int y2, double x1, double x2, double w1, double w2, int fillType)
{
    for (int y = y1; y <= y2; y++) {

        int startX = (int)std::ceil(std::min(x1, x2));
        int endX = (int)std::floor(std::max(x1, x2));

        for (int x = startX; x <= endX; x++) {
            setPixel(x, y, getColor(x, y, fillType));
        }

        x1 += w1;
        x2 += w2;
    }
}


void ViewerWidget::fillBottomTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType)
{
    double w1 = (double)(t2.pos.x() - t0.pos.x()) / (t2.pos.y() - t0.pos.y());
    double w2 = (double)(t2.pos.x() - t1.pos.x()) / (t2.pos.y() - t1.pos.y());

    double x1 = t0.pos.x();
    double x2 = t1.pos.x();

    int y1 = t0.pos.y();
    int y2 = t2.pos.y();

    fillTrianglePart(y1, y2, x1, x2, w1, w2, fillType);
}

void ViewerWidget::fillTopTriangle(Vertex t0, Vertex t1, Vertex t2, int fillType)
{
    //hrany idu zhora nadol: e1 spaja t0-t1, e2 spaja t0-t2
    double w1 = (double)(t1.pos.x() - t0.pos.x()) / (t1.pos.y() - t0.pos.y());
    double w2 = (double)(t2.pos.x() - t0.pos.x()) / (t2.pos.y() - t0.pos.y());

    //zaciname na vrchole t0
    double x1 = t0.pos.x();
    double x2 = t0.pos.x();

    int y1 = t0.pos.y();
    int y2 = t1.pos.y();

    fillTrianglePart(y1, y2, x1, x2, w1, w2, fillType);
}

QColor ViewerWidget::getNearestColor(int x, int y, Vertex t0, Vertex t1, Vertex t2)
{
    // vzdialenosti od pixelu k jednotlivym vrcholom
    int d0 = (x - t0.pos.x()) * (x - t0.pos.x()) +
             (y - t0.pos.y()) * (y - t0.pos.y());

    int d1 = (x - t1.pos.x()) * (x - t1.pos.x()) +
             (y - t1.pos.y()) * (y - t1.pos.y());

    int d2 = (x - t2.pos.x()) * (x - t2.pos.x()) +
             (y - t2.pos.y()) * (y - t2.pos.y());

    // vyberieme farbu najblizsieho vrcholu
    if (d0 <= d1 && d0 <= d2) {
        return t0.color;
    }
    else if (d1 <= d0 && d1 <= d2) {
        return t1.color;
    }
    else {
        return t2.color;
    }
}

QColor ViewerWidget::getBarycentricColor(int x, int y, Vertex t0, Vertex t1, Vertex t2)
{
    // celkova plocha trojuholnika T0,T1,T2
    double A = abs((t1.pos.x() - t0.pos.x()) * (t2.pos.y() - t0.pos.y()) -
                   (t1.pos.y() - t0.pos.y()) * (t2.pos.x() - t0.pos.x())) / 2;

    // plochy podtrojuholnikov s bodom P(x,y)
    double A0 = abs((t1.pos.x() - x) * (t2.pos.y() - y) -
                    (t1.pos.y() - y) * (t2.pos.x() - x)) / 2;

    double A1 = abs((t0.pos.x() - x) * (t2.pos.y() - y) -
                    (t0.pos.y() - y) * (t2.pos.x() - x)) / 2;

    double A2 = A - A0 - A1; // tretia plocha (aby sme nemuseli pocitat znova)

    // vahy (barycentricke suradnice)
    double l0 = A0 / A;
    double l1 = A1 / A;
    double l2 = A2 / A;

    // interpolacia farby
    int r = (int)(l0 * t0.color.red() + l1 * t1.color.red() + l2 * t2.color.red());
    int g = (int)(l0 * t0.color.green() + l1 * t1.color.green() + l2 * t2.color.green());
    int b = (int)(l0 * t0.color.blue() + l1 * t1.color.blue() + l2 * t2.color.blue());

    // orezanie na rozsah 0255
    r = qBound(0, r, 255);
    g = qBound(0, g, 255);
    b = qBound(0, b, 255);

    return QColor(r, g, b);
}

QColor ViewerWidget::getColor(int x, int y, int fillType) {
    QColor color;
    if (fillType == 0) {
        color = getNearestColor(x, y, base_t0, base_t1, base_t2);
    }
    else if (fillType == 1) {
        color = getBarycentricColor(x, y, base_t0, base_t1, base_t2);
    }
    else {
        color = base_t0.color;
    }
    int xy;
    return color;

}

void ViewerWidget::updateTriangleLogic(){
    if (originalPoints.size() == 3) {
        // Оновлюємо тільки позиції, зберігаючи кольори
        base_t0.pos = originalPoints[0];
        base_t1.pos = originalPoints[1];
        base_t2.pos = originalPoints[2];
    }
}






//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)//головна функція яку викликає qt
{
    QPainter painter(this);//створюється qpainter який буде малювати на цьому віджеті
    if (!img || img->isNull()) return;//не створений або порожній

    QRect area = event->rect();// прямокутник, оптимізація  "пошкодженої частини", не завжди треба перемальовувати весь
	painter.drawImage(area, *img, area);//vykresli novy obrazok
}
