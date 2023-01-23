#include "mainwindow.h"
#include "filetree.h"
#include "tree_model.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFuture>
#include <QMessageBox>
#include <QPromise>
#include <QtConcurrent>
#include <csrv.h>

void insert(TreeItem *item, FileTree *tree)
{
  int child_count = tree->folders.size();
  if (child_count == 0)
    return;
  for (int i = 0; i < child_count; i++) {
    auto f = tree->folders[i];
    QString id = QString::fromStdString(to_string(f.id));
    QString name = QString::fromStdString(f.name);
    item->appendChild(new TreeItem(QStringList() << name << "" << id));
    insert(item->child(i), &f);
  }
}

TreeModel *newTreeModel()
{
  auto headers = QStringList() << "canvas folder"
                               << "local folder";
  TreeModel *model = new TreeModel(headers);
  return model;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      server("", "https://canvas.nus.edu.sg")
{
  ui->setupUi(this);
  TreeModel *model = newTreeModel();
  ui->treeView->setModel(model);
  ClickableTreeView *ctv = ui->treeView;
  ctv->expandAll();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_pull_clicked()
{
  this->server.load_tree();
  auto tree = this->server.get_tree();
  TreeModel *model = newTreeModel();
  insert(model->item(0), tree);
  ui->treeView->setModel(model);
}

void MainWindow::on_lineEdit_accessToken_textChanged(const QString &input)
{
  this->token = input;
  if (token.size() > 20) {
    string token = input.toStdString();
    this->server.set_token(&token);
    ui->label_authenticationStatus->setText("authenticating...");
    ui->label_authenticationStatus->repaint();
    qApp->processEvents();
  } else {
    ui->label_authenticationStatus->setText("unauthenticated");
    return;
  }
  QFuture<bool> fut =
      QtConcurrent::run([this, input] { return this->server.valid_token(); });
  if (fut.result()) {
    ui->label_authenticationStatus->setText("authenticated!");
    ui->lineEdit_accessToken->setReadOnly(true);
    ui->lineEdit_accessToken->setDisabled(true);
    this->server.load_tree();
    auto tree = this->server.get_tree();
    TreeModel *model = newTreeModel();
    insert(model->item(0), tree);
    ui->treeView->setModel(model);
    ui->treeView->resizeColumnToContents(0);
  } else {
    ui->label_authenticationStatus->setText("unauthenticated");
  }
}

void to_dir_dialog(QFileDialog *dialog)
{
  dialog->setFileMode(QFileDialog::Directory);
  // because for some reason it crashes on macOS
  dialog->setOption(QFileDialog::DontUseNativeDialog);
}

void MainWindow::on_pushButton_fetch_clicked()
{
  QFileDialog dialog(this);
  to_dir_dialog(&dialog);
  dialog.setWindowTitle("DANK MEMES");
  dialog.setDirectory(this->start_dir);
  if (dialog.exec() != 0) {
    QMessageBox::warning(this, "Oops", "Unable to open a file manager.");
  } else {
    auto fileNames = dialog.selectedFiles();
    for (auto f : fileNames) {
      qDebug() << "selected" << f;
    }
  }
}

TreeItem *retrieve(TreeItem *item, vector<int> *path)
{
  while (!path->empty()) {
    qDebug() << "at -> " << item->data()[0] << item->data()[1];
    int idx = path->back();
    qDebug() << "idx " << idx;
    path->pop_back();
    item = item->child(idx);
  }
  return item;
}
TreeItem *retrieve(TreeModel *model, vector<int> *path)
{
  if (path->size() == 1) {
    qDebug() << "Size 1";
    return model->item(path->back());
  } else {
    int idx = path->back();
    path->pop_back();
    qDebug() << "idx " << idx;
    qDebug() << "start going in";
    return retrieve(model->item(idx), path);
  }
}
TreeItem *retrieve(TreeModel *model, const QModelIndex &index)
{
  vector<int> path;
  path.push_back(index.row());
  QModelIndex a = index;
  while (a.parent().isValid()) {
    path.push_back(a.parent().row());
    a = a.parent();
  }
  return retrieve(model, &path);
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
  // go home for non-localdir bois
  if (index.column() != 1)
    return;

  QFileDialog dialog(this);
  to_dir_dialog(&dialog);
  auto home = QDir::homePath();
  dialog.setDirectory(this->start_dir != home ? this->start_dir : home);
  dialog.exec();

  auto item = retrieve(ui->treeView->model(), index);

  auto local_dir = dialog.selectedFiles()[0];
  item->setData(1, local_dir);

  // updated last selected dir
  QDir selected_dir = QDir::fromNativeSeparators(local_dir);
  selected_dir.cdUp();
  this->start_dir = selected_dir.path();
}

void MainWindow::on_treeView_expanded(const QModelIndex &index)
{
  ui->treeView->resizeColumnToContents(0);
}

void MainWindow::on_treeView_collapsed(const QModelIndex &index)
{
  ui->treeView->resizeColumnToContents(0);
}
