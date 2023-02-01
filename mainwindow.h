#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <filesystem>
#include <map>
#include <mutex>

#include "convert.h"
#include "filetree.h"
#include "tree.h"
#include "tree_model.h"
#include "types.h"
#include "ui_mainwindow.h"
#include "updates.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSaveFile>
#include <QSettings>
#include <QStandardPaths>

class Network : public QNetworkAccessManager
{
  Q_OBJECT

private:
  std::string base_url;

private slots:
  void replyFinished(QNetworkReply *r)
  {
    qDebug() << r->readAll();
  }

public:
  Network(std::string base_url) : base_url(base_url){};
  void get(std::string &url)
  {
  }
  QNetworkReply *get(QNetworkRequest q)
  {
    return QNetworkAccessManager::get(q);
  }
  void ping()
  {
    QUrl qrl("https://nguyenvukhang.com/api/nus");
    connect(this, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(replyFinished(QNetworkReply *)));
    QNetworkRequest r(qrl);
    r.setRawHeader("Authorization", "Bearer token");
    this->get(r);
  }
};

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

private:
  QNetworkRequest req(std::string url)
  {
    QString q = this->base_url;
    q.append(QString::fromStdString(url));
    QUrl qrl(q);
    QNetworkRequest r(qrl);
    QString bearer = "Bearer ";
    bearer += this->token;
    r.setRawHeader("Authorization", bearer.toUtf8());
    return r;
  }

  QNetworkRequest download_req(std::string full_url)
  {
    QNetworkRequest r(*new QUrl(full_url.c_str()));
    QString bearer = "Bearer ";
    bearer += this->token;
    r.setRawHeader("Authorization", bearer.toUtf8());
    return r;
  }

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void pull_clicked();
  void fetch_clicked();
  void changeToken_clicked();
  void accessToken_textChanged(const QString &);
  // network stuff
  void check_auth_fetched();
  void courses_fetched();
  void course_folders_fetched(const Course &);
  void folder_files_fetched(Update u, size_t c, bool download);
  void file_downloaded(File f, size_t c);
  // tree stuff
  void treeView_clicked(const QModelIndex &);
  void treeView_doubleClicked(const QModelIndex &);
  void treeView_cleared(const QModelIndex &);
  void treeView_expanded(const QModelIndex &);
  void treeView_collapsed(const QModelIndex &);

public:
  std::string folder_name(const int folder_id);
  std::string course_name(const int course_id);
  void enable_pull();
  void disable_pull();
  void enable_fetch();
  void disable_fetch();
  void refresh_tree();
  void set_auth_state(bool);
  void show_updates(const std::vector<Update> &);
  void check_auth(const QString &token);
  void fetch_courses();
  void fetch_course_folders(const Course &);
  void fetch_folder_files(Update u, size_t c, bool download);
  void download_file(File f, size_t c);
  std::vector<Update> gather_tracked();

  // data
  bool authenticated = false, updates_done;
  std::mutex tree_mtx, update_mtx, dl_e_mtx, dl_r_mtx;
  QSettings settings;
  std::vector<Update> updates;
  int expected_downloads, received_downloads;
  std::vector<FileTree> course_trees;
  std::vector<Course> user_courses;
  std::map<int, std::string> folder_names;
  QString token, base_url = "https://canvas.nus.edu.sg";
  Ui::MainWindow *ui;
  Network nw;
  QString start_dir = QDir::homePath();
};
#endif // MAINWINDOW_H
