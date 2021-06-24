#ifndef GAMESORTING_STARWIDGET_H_
#define GAMESORTING_STARWIDGET_H_

#include <QWidget>

class StarWidget : public QWidget
{
    Q_OBJECT
public:
    StarWidget(QWidget* parent = nullptr);
    StarWidget(int value = 0, int max = 5, QWidget* parent = nullptr);

    void setValue(int value);
    void setMax(int max);

    int getValue() const;
    int getMax() const;

signals:
    void valueChanged(int value);

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    int m_value;
    int m_max;
};

#endif