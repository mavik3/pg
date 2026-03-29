 #include "ImageViewer.h"

//QObject
//QEvent nase situaci
//pos()- position mouse
//QPoint - int 
//QPointf - double
//

//pocas vypoctu moze byt ne celeciselna hodnota preto potom zaokruhlim do int
ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
    vW = new ViewerWidget(QSize(500, 500), ui->scrollArea);//velkost platna dinamicka
	ui->scrollArea->setWidget(vW);//to nase platno QScrollArea este to bude posuvaci
    ui->spinRotation->setRange(-360.0,360.0);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);// aby bola sd farba
	ui->scrollArea->setWidgetResizable(false);//nemenie rozmer
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);//ked zmensi
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->spinShear->setRange(-1,1);
	vW->setObjectName("ViewerWidget"); //aby nazvat objekt
	vW->installEventFilter(this);

	globalColor = Qt::blue; //farba
	QString style_sheet = QString("background-color: %1;").arg(globalColor.name(QColor::HexRgb));
    ui->pushButtonSetColor->setStyleSheet(style_sheet);
    colorT0 = Qt::red;
    colorT1 = Qt::green;
    colorT2 = Qt::blue;
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return QMainWindow::eventFilter(obj, event);
}
//QObject->QWidget->ViewerWidget
//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {//stalacit button
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {//odysel od platna
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {//zasel do platna
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);

    /*if (vW->getOriginalPoints().size() == 3) {

        ui->FillType->setEnabled(true);
        ui->T0Color->setEnabled(true);
        ui->T1Color->setEnabled(true);
        ui->T2Color->setEnabled(true);
    } else {
        ui->FillType->setCurrentIndex(2); // Скидаємо на суцільний колір
        ui->FillType->setEnabled(false);        // Вимикаємо можливість вибору
        ui->T0Color->setEnabled(false);
        ui->T1Color->setEnabled(false);
        ui->T2Color->setEnabled(false);
        // Дозволяємо вибір для трикутника
    }*/
    if (ui->Polygon->isChecked()) {
        if (e->button() == Qt::LeftButton) {
            if (w->getPolygonFinished()) {
                w->getOriginalPoints().clear(); // Очистить і originalPoints, і екран
                w->setPolygonFinished(false);
            }
            // Додаємо в ОРИГІНАЛ
            w->getOriginalPoints().push_back(e->pos());
        }
        else if (e->button() == Qt::RightButton) {
            w->setPolygonFinished(true);
        }
        w->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());

    }
    if (ui->Move->isChecked() && e->button() == Qt::LeftButton) {
        w->setDraggingPolygon(true); // Активація режиму перетягування
        w->setLastMousePos(e->pos());
    }
}



void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event) {
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    if (e->button() == Qt::LeftButton) {
        w->setDraggingPolygon(false); // Зупиняємо перетягування
    }
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event) {
    QMouseEvent* e = static_cast<QMouseEvent*>(event);

    if (w->getDraggingPolygon()) {
        QPoint currentPos = e->pos();
        QPoint lastPos = w->getLastMousePos();

        int dx = currentPos.x() - lastPos.x();
        int dy = currentPos.y() - lastPos.y();

        w->movePolygon(dx, dy); // Змінює originalPoints
        w->setLastMousePos(currentPos);

        // Перемальовуємо з автоматичним відсіканням
        w->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());
    }
}

void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
    {
        QWheelEvent* e = static_cast<QWheelEvent*>(event);

        double factor = (e->angleDelta().y() > 0) ? 1.25 : 0.75;

        QVector<QPoint> p = w->getPolygonPoints();
        vW->Scale(factor,factor);
        vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());
    }
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}
//regit
//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());//ak vybralo obrazok tak nastavi cestu do tych registrov

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	vW->clear();
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_Rotation_clicked(){
    double k = ui->spinRotation->value();
    vW->rotation(k);
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());
}

void ImageViewer::on_Scale_clicked(){
    double x = ui->spinX->value();
    double y = ui->spinY->value();
    vW->Scale(x, y);
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());
    int xy;
}
void ImageViewer::on_Shear_clicked(){
    double pS = ui->spinShear->value();
    vW->Shear(pS, ui->comboBoxShear->currentIndex());
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());
}

void ImageViewer::on_OsSum_clicked(){
    vW->OsSum();
    vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());

}
void ImageViewer::on_Fill_clicked() {
    QVector<QPoint> pts = vW->getOriginalPoints();

    bool isChecked = ui->Fill->isChecked();
    int fillType = ui->FillType->currentIndex();

    if (pts.size() == 3) {

        Vertex t0 = { pts[0], colorT0 };
        Vertex t1 = { pts[1], colorT1 };
        Vertex t2 = { pts[2], colorT2 };
        vW->setTriangleVertixes(t0, t1, t2);
    }
    vW->setFillEnabled(ui->Fill->isChecked());
    vW->setFillType(ui->FillType->currentIndex());

    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex(),ui->Fill->isChecked());
}
void ImageViewer::on_T0Color_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT0, this);

    if (newColor.isValid()) {
        colorT0 = newColor;

        QString style = QString("background-color: %1;")
                            .arg(colorT0.name(QColor::HexRgb));
        ui->T0Color->setStyleSheet(style);
        QVector<QPoint> pts = vW->getOriginalPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({pts[0], colorT0}, {pts[1], colorT1}, {pts[2], colorT2});
        }
        // Перемальовуємо
        vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());
    }
}

void ImageViewer::on_T1Color_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT1, this);

    if (newColor.isValid()) {
        colorT1 = newColor;

        QString style = QString("background-color: %1;")
                            .arg(colorT1.name(QColor::HexRgb));
        ui->T1Color->setStyleSheet(style);
        QVector<QPoint> pts = vW->getOriginalPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({pts[0], colorT0}, {pts[1], colorT1}, {pts[2], colorT2});
        }
        // Перемальовуємо
        vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());
    }

}

void ImageViewer::on_T2Color_clicked()
{
    QColor newColor = QColorDialog::getColor(colorT2, this);

    if (newColor.isValid()) {
        colorT2 = newColor;

        QString style = QString("background-color: %1;")
                            .arg(colorT2.name(QColor::HexRgb));
        ui->T2Color->setStyleSheet(style);
        QVector<QPoint> pts = vW->getOriginalPoints();
        if (pts.size() == 3) {
            vW->setTriangleVertixes({pts[0], colorT0}, {pts[1], colorT1}, {pts[2], colorT2});
        }
        // Перемальовуємо
        vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());
    }
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: %1;").arg(newColor.name(QColor::HexRgb));
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;//zmeni global farba
	}
}

void ImageViewer::on_FillType_currentIndexChanged(int index)
{
    vW->setFillType(index); // Оновлюємо індекс у віджеті
    // Перемальовуємо
    vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex(), ui->Fill->isChecked());
}

