#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QSettings>
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
  void pull_clicked();
  void fetch_clicked();
  void changeToken_clicked();
  void accessToken_textChanged(const QString &arg1);
  void treeView_doubleClicked(const QModelIndex &index);
  void treeView_expanded(const QModelIndex &index);
  void treeView_collapsed(const QModelIndex &index);
  void treeView_clicked(const QModelIndex &index);
  void treeView_cleared(const QModelIndex &index);

private:
  QSettings settings;
  QString token;
  Ui::MainWindow *ui;
  canvas::Server server;
  QString start_dir =
      QDir::homePath(); // directory that the file explorer will open with.
  void try_auth(const QString &token);
  void set_auth_state(bool);
};
#endif // MAINWINDOW_H
