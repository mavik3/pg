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

    if (ui->Polygon->isChecked())
    {

        ui->Move->setChecked(false);
        if (e->button() == Qt::LeftButton)
        {
            if(w->getPolygonFinished()){
                w->clear();
                w->setPolygonFinished(false);
            }
            w->getPolygonPoints().push_back(e->pos());

            w->setPixel(e->pos().x(), e->pos().y(), globalColor);

            int n = w->getPolygonPoints().size();
            if (n >= 2) {
                QPoint a = w->getPolygonPoints()[n - 2];
                QPoint b = w->getPolygonPoints()[n - 1];

                w->drawLine(a, b, globalColor, ui->comboBoxLineAlg->currentIndex());
                if(ui->comboBoxLineAlg->currentIndex() == 2){
                    w->setPolygonFinished(true);
                }
            }
            else {
                w->update();
            }
        }
        else if (e->button() == Qt::RightButton)
        {
            QVector<QPoint>& pts = w->getPolygonPoints();

            if (pts.size() > 2)
            {
                w->drawPolygon(
                    pts,
                    globalColor,
                    ui->comboBoxLineAlg->currentIndex(),
                    true
                    );
            }
            else if (pts.size() == 2)
            {
                w->drawLine(pts[0],pts[1],globalColor,ui->comboBoxLineAlg->currentIndex());
            }
            w->setPolygonFinished(true);
        }
    }
    // почати посування готового полігону
    else if (ui->Move->isChecked() && w->getPolygonFinished())
    {
        ui->Polygon->setChecked(false);
        if (vW->getPolygonPoints().size() <= 2)
        w->CyrBec();
        w->SutHod();
        if (e->button() == Qt::LeftButton) {
            w->setDraggingPolygon(true);
            w->setLastMousePos(e->pos());

           // if (ui->comboBoxLineAlg->currentIndex() == 2){

          //  }
        }
        if (e->button() == Qt::RightButton){
            w->setDraggingPolygon(false);
            w->setLastMousePos(e->pos());

          //  if(ui->comboBoxLineAlg->currentIndex() == 2){

          //  }
        }

        return;
    }
}


void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);

    if (!w->getDraggingPolygon()) return;

    QPoint currentPos = e->pos();
    QPoint lastPos = w->getLastMousePos();

    int dx = currentPos.x() - lastPos.x();
    int dy = currentPos.y() - lastPos.y();

    w->movePolygon(dx, dy);
    w->setLastMousePos(currentPos);

    w->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex());

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
        vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex());
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
    vW->SutHod();
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex());
}

void ImageViewer::on_Scale_clicked(){
    double x = ui->spinX->value();
    double y = ui->spinY->value();
    vW->Scale(x, y);
    vW->SutHod();
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex());
}
void ImageViewer::on_Shear_clicked(){
    double pS = ui->spinShear->value();
    vW->Shear(pS, ui->comboBoxShear->currentIndex());
    vW->SutHod();
    vW->redrawPolygon(globalColor,ui->comboBoxLineAlg->currentIndex());
}

void ImageViewer::on_OsSum_clicked(){
    vW->OsSum();
    vW->SutHod();
    vW->redrawPolygon(globalColor, ui->comboBoxLineAlg->currentIndex());

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
