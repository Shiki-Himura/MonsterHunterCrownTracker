#include "crownoverlaywidget.h"
#include <QPainter>
#include <QMouseEvent>

CrownOverlayWidget::CrownOverlayWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(128, 128);
}

void CrownOverlayWidget::setMainIcon(const QPixmap &icon)
{
    m_mainIcon = icon;
    update();
}

void CrownOverlayWidget::setCrownIcons(const QPixmap &leftCrown, const QPixmap &rightCrown)
{
    m_leftCrown = leftCrown;
    m_rightCrown = rightCrown;
    update();
}

QPair<bool, bool> CrownOverlayWidget::crownStates() const
{
    return {m_leftActive, m_rightActive};
}

void CrownOverlayWidget::setIndex(int index)
{
    m_index = index;
}

void CrownOverlayWidget::setCrownState(bool left, bool right)
{
    m_leftActive = left;
    m_rightActive = right;
    update();
}

void CrownOverlayWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_leftCrownRect.contains(event->pos()))
    {
        m_leftActive = !m_leftActive;
        emit crownStateChanged(m_index, m_leftActive, m_rightActive);
        update();
    }
    else if (m_rightCrownRect.contains(event->pos()))
    {
        m_rightActive = !m_rightActive;
        emit crownStateChanged(m_index, m_leftActive, m_rightActive);
        update();
    }
}

void CrownOverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Determine the visual state
    int activeCount = int(m_leftActive) + int(m_rightActive);

    if (activeCount == 0)
    {
        // Both inactive → grayscale + 50%
        painter.drawPixmap(rect(), createGrayed(m_mainIcon, 0.5));
    }
    else if (activeCount == 1)
    {
        // One active → grayscale + 100%
        painter.drawPixmap(rect(), createGrayed(m_mainIcon, 1.0));
    }
    else
    {
        // Both active → full color
        painter.drawPixmap(rect(), m_mainIcon);
    }

    // Draw crowns
    int crownSize = 32;
    m_leftCrownRect = QRect(4, 4, crownSize, crownSize);
    m_rightCrownRect = QRect(width() - crownSize - 4, 4 - 3, crownSize, crownSize);

    if (!m_leftCrown.isNull())
    {
        painter.setOpacity(m_leftActive ? 1.0 : 0.4);
        painter.drawPixmap(m_leftCrownRect, m_leftCrown);
    }

    if (!m_rightCrown.isNull())
    {
        painter.setOpacity(m_rightActive ? 1.0 : 0.4);
        painter.drawPixmap(m_rightCrownRect, m_rightCrown);
    }

    painter.setOpacity(1.0);
}

QPixmap CrownOverlayWidget::createGrayed(const QPixmap &pixmap, qreal opacity)
{
    if (pixmap.isNull()) return QPixmap();

    QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    QPainter p(&img);

    // Desaturate
    for (int y = 0; y < img.height(); ++y)
    {
        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x)
        {
            QColor color = QColor::fromRgba(line[x]);
            int gray = qGray(color.rgb());
            color.setRgb(gray, gray, gray);
            line[x] = color.rgba();
        }
    }

    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(img.rect(), QColor(0, 0, 0, int(opacity * 255)));
    p.end();

    return QPixmap::fromImage(img);
}
