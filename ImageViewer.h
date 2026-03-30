#pragma once
//QcolorDialog umoznuje vyber farby
//event - co teraz deje s misou a ine 
//
#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);
	~ImageViewer() { delete ui; }
private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QColor globalColor;
	QSettings settings;
	QMessageBox msgBox;
    QColor colorT0;
    QColor colorT1;
    QColor colorT2;

    bool circleInProgress = false;


	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionExit_triggered();

    void on_Rotation_clicked();
    void on_Scale_clicked();
    void on_Shear_clicked();
    void on_OsSum_clicked();
    void on_Fill_clicked();

    void on_T0Color_clicked();
    void on_T1Color_clicked();
    void on_T2Color_clicked();
    //Tools

    void on_pushButtonSetColor_clicked();
    void on_FillType_currentIndexChanged(int index);
};
