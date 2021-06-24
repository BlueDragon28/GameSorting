#include "StarWidget.h"
#include "StarEditor.h"
#include "Common.h"
#include <QPainter>
#include <QMouseEvent>

StarWidget::StarWidget(int value, int max, QWidget* parent) :
    QWidget(parent),
    m_value(value),
    m_max(max)
{
    setFixedSize(int(StarEditor::paintFactor() * m_max), StarEditor::paintFactor());
}

StarWidget::StarWidget(QWidget* parent) :
    StarWidget(0, 5, parent)
{}

void StarWidget::setValue(int value)
{
    if (value >= 0 && value <= m_max)
    {
        m_value = value;
        emit valueChanged(m_value);
        update();
    }
}

void StarWidget::setMax(int max)
{
    if (max > 0)
    {
        m_max = max;
        if (m_value > m_max)
            m_value = m_max;
        emit valueChanged(m_value);
        setFixedSize(int(StarEditor::paintFactor() * m_max), StarEditor::paintFactor());
        update();
    }
}

int StarWidget::getValue() const
{
    return m_value;
}

int StarWidget::getMax() const
{
    return m_max;
}

void StarWidget::paintEvent(QPaintEvent* e)
{
    // Calling the static paintStars from the StarEditor class
    QPainter painter(this);
    StarEditor::paintStars(m_value, &painter, rect(), palette(), false);
}

void StarWidget::mousePressEvent(QMouseEvent* e)
{
    // Calculate the number of stars when the user press on the widget.
    float nb = inRange((float)e->position().x() / ((float)width() / (float)m_max), 0.f, (float)m_max);
    int stars = int(nb+0.5);

    m_value = stars;
    emit valueChanged(m_value);
    update();

    QWidget::mousePressEvent(e);
}