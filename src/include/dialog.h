#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <vector>

using namespace std;
namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  void init(vector<double> *best, vector<double> *avg);
  void paintEvent(QPaintEvent *event) override;
  ~Dialog() override;

private:
  Ui::Dialog *ui;
  vector<double> *best{};
  vector<double> *avg{};
  void paint_axis(QPainter *painter, double y_scale, double y_start,
                  double x_scale);
};

#endif // DIALOG_H
