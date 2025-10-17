#pragma once
#include <QDialog>
#include <memory>
#include <vector>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  void init(const std::vector<double> &best, const std::vector<double> &avg);
  void paintEvent(QPaintEvent *event) override;
  ~Dialog() override;

private:
  Ui::Dialog *ui;
  std::shared_ptr<const std::vector<double>> best_;
  std::shared_ptr<const std::vector<double>> avg_;
  void paint_axis(QPainter *painter, double y_scale, double y_start,
                  double x_scale);
};
