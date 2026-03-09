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

	ui->scrollArea->setBackgroundRole(QPalette::Dark);// aby bola sd farba
	ui->scrollArea->setWidgetResizable(false);//nemenie rozmer
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);//ked zmensi
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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

    // ===== LINE MODE =====
    if (ui->toolButtonDrawLine->isChecked())
    {
        if (e->button() == Qt::LeftButton)
        {
            if (w->getDrawLineActivated()) {
                w->drawLine(w->getDrawLineBegin(),
                            e->pos(),
                            globalColor,
                            ui->comboBoxLineAlg->currentIndex());

                w->setDrawLineActivated(false);
            }
            else {
                w->setDrawLineBegin(e->pos());
                w->setDrawLineActivated(true);

                w->setPixel(e->pos().x(), e->pos().y(), globalColor);
                w->update();
            }
        }
    }
    else if (ui->Polygon->isChecked())
    {
        if (e->button() == Qt::LeftButton)
        {
            if (w->isPolygonFinished()) return;
            w->getPolygonPoints().push_back(e->pos());

            w->setPixel(e->pos().x(), e->pos().y(), globalColor);

            int n = w->getPolygonPoints().size();
            if (n >= 2) {
                QPoint a = w->getPolygonPoints()[n - 2];
                QPoint b = w->getPolygonPoints()[n - 1];
                w->drawLine(a, b, globalColor, ui->comboBoxLineAlg->currentIndex());
            }
            else {
                w->update();
            }
        }
        else if (e->button() == Qt::RightButton)
        {
            QVector<QPoint>& pts = w->getPolygonPoints();

            if (pts.size() >= 3)
            {
                w->drawPolygon(
                    pts,
                    globalColor,
                    ui->comboBoxLineAlg->currentIndex(),
                    true
                    );
                w->setPolygonFinished(true);
            }
        }
    }
}


void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
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

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: %1;").arg(newColor.name(QColor::HexRgb));
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;//zmeni global farba
	}
}
