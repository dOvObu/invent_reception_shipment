#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>

std::shared_ptr<QStandardItemModel> modelReception (new QStandardItemModel ());
std::shared_ptr<QStandardItemModel> modelShipment (new QStandardItemModel ());
QString path;

MainWindow::MainWindow (QWidget *parent) :
	QMainWindow (parent),
	ui (new Ui::MainWindow)
{
	ui->setupUi (this);
	QStringList headers;
	headers.append ("Наименование");
	headers.append ("Количество");
	modelReception->setHorizontalHeaderLabels (headers);
	modelShipment->setHorizontalHeaderLabels (headers);
	ui->tableView->setModel (modelReception.get ());
	ui->tableView_2->setModel (modelShipment.get ());
    std::ifstream input ("./_config.txt");
	std::string str;
	getline (input, str, '\n');
	path = str.c_str ();
}

MainWindow::~MainWindow ()
{
	delete ui;
}

void deleteRowWithKeyFromModel (QString& key, QStandardItemModel* model)
{
	QList<QStandardItem*> list = model->findItems (key);
	for (auto it = list.begin (); it != list.end (); ++it)
	{
		model->removeRow ((*it)->row ());
	}
}

void updateTableModels (
		QStandardItemModel* inclModel,
		QStandardItemModel* exclModel,
		std::vector<QString>& keys,
		std::vector<int>& found,
		std::vector<unsigned>& wasNotFound)
{
	for (unsigned i = 0; i < keys.size (); ++i)
	{
		if (found[i])
		{
			QList<QStandardItem*> pair;
			pair.append (new QStandardItem(keys[i]));
			pair.append (new QStandardItem(std::to_string (found[i]).c_str ()));
			inclModel->appendRow (pair);
			deleteRowWithKeyFromModel (keys[i], exclModel);
		}
		else
		{
			wasNotFound.push_back(i);
		}
	}
}

void xmlToModels (
		QXmlStreamReader* reader,
		QStandardItemModel* inclModel,
		QStandardItemModel* exclModel,
		std::vector<QString>& keys,
		std::vector<unsigned>& wasNotFound)
{
	std::vector<int> found (keys.size ());
	while (reader->readNextStartElement ())
		if (reader->name () == "id")
		{
			auto str = reader->readElementText ();
			for (unsigned i = 0; i < keys.size (); ++i)
			{
				if (str == keys[i])
				{
					if (!found[i]) { deleteRowWithKeyFromModel (keys[i], inclModel); }
					++(found[i]);
				}
			}
		}
	updateTableModels (inclModel, exclModel, keys, found, wasNotFound);
}

void splitByTarget (
		const Operation& type,
		QXmlStreamReader* reader,
		std::vector<QString>& keys,
		std::vector<unsigned>& wasNotFound)
{
	if (type == Operation::Reception)
	{
		xmlToModels (
					reader,
					modelReception.get (), modelShipment.get (),
					keys, wasNotFound);
	}
	else
	{
		xmlToModels (
					reader,
					modelShipment.get (), modelReception.get (),
					keys, wasNotFound);
	}
}

void liveIfNotFound (
		QLineEdit* lineEdit,
		std::vector<unsigned>& wasNotFound,
		std::vector<QString>& names)
{
	QString str = "";
	for (unsigned& it : wasNotFound)
	{
		str += names[it] + " ";
	}
	lineEdit->setText (str);
}

void showReceptionOrShipment (
		QLineEdit* lineEdit,
		const Operation& type)
{
	std::string line = lineEdit->text ().toUtf8 ().constData ();
	std::stringstream sst (line.c_str ());
	QFile file (path);
	if (file.open (QIODevice::ReadOnly | QIODevice::Text))
	{
		lineEdit->clear ();
		QXmlStreamReader reader (&file);
		std::vector<QString> names;
		while (getline (sst, line, ' ')) { names.push_back (line.c_str ()); }
		std::vector<unsigned> wasNotFound;
		if (names.size ()) { splitByTarget (type, &reader, names, wasNotFound); }
		if (wasNotFound.size ()) { liveIfNotFound (lineEdit, wasNotFound, names); }
	}
	file.close ();
}

void MainWindow::on_lineEdit_returnPressed ()
{
	showReceptionOrShipment (ui->lineEdit, Operation::Reception);
	ui->tableView->resizeColumnsToContents ();
	ui->tableView_2->resizeColumnsToContents ();
}

void MainWindow::on_lineEdit_2_returnPressed ()
{
	showReceptionOrShipment (ui->lineEdit_2, Operation::Shipment);
	ui->tableView->resizeColumnsToContents ();
	ui->tableView_2->resizeColumnsToContents ();
}

void MainWindow::on_pushButton_clicked ()
{
	while (modelReception->rowCount()) modelReception->removeRow(0);
	while (modelShipment->rowCount()) modelShipment->removeRow(0);
}

void MainWindow::on_lineEdit_textChanged (const QString &arg1)
{
	ui->lineEdit->setText( arg1.toUpper () );
}
