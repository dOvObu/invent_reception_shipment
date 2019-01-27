#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QLineEdit"
#include "QStandardItemModel"
#include "QStandardItem"
#include <QDebug>
#include <QtXml>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_lineEdit_returnPressed();

	void on_lineEdit_2_returnPressed();

	void on_pushButton_clicked();

	void on_lineEdit_textChanged(const QString &arg1);

private:
	Ui::MainWindow *ui;
};

enum class Operation {Reception, Shipment};

void deleteRowWithKeyFromModel (QString& key, QStandardItemModel* model);

void updateTableModels (
		QStandardItemModel* inclModel,
		QStandardItemModel* exclModel,
		std::vector<QString>& keys,
		std::vector<int>& found,
		std::vector<unsigned>& wasNotFound);

void xmlToModels (
		QXmlStreamReader* reader,
		QStandardItemModel* inclModel,
		QStandardItemModel* exclModel,
		std::vector<QString>& keys,
		std::vector<unsigned>& wasNotFound);

void splitByTarget (
		const Operation& type,
		QXmlStreamReader* reader,
		std::vector<QString>& keys,
		std::vector<unsigned>& wasNotFound);

void liveIfNotFound (
		QLineEdit* lineEdit,
		std::vector<unsigned>& wasNotFound,
		std::vector<QString>& names);

void showReceptionOrShipment (
		QLineEdit* lineEdit,
		const Operation& type,
		const QString& path);

#endif // MAINWINDOW_H
