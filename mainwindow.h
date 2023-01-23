#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <csrv.h>

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
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_pushButton_pull_clicked();
  void on_lineEdit_accessToken_textChanged(const QString &arg1);
  void on_pushButton_fetch_clicked();
  void on_treeView_doubleClicked(const QModelIndex &index);
  void on_treeView_expanded(const QModelIndex &index);
  void on_treeView_collapsed(const QModelIndex &index);

private:
  QString token;
  Ui::MainWindow *ui;
  canvas::Server server;
  QString start_dir =
      QDir::homePath(); // directory that the file explorer will open with.
};
#endif // MAINWINDOW_H
