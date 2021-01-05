#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

 private slots:
	void on_playHC_clicked();

	void on_playCH_clicked();

	void on_playCC_clicked();

	void on_playHH_clicked();

 private:
	Ui::MainWindow* ui;

};

#endif // MAINWINDOW_H
