#ifndef CROWNOVERLAYWIDGET_H
#define CROWNOVERLAYWIDGET_H

#include <QWidget>
#include <QPixmap>

class CrownOverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CrownOverlayWidget(QWidget *parent = nullptr);

    void setMainIcon(const QPixmap &icon);
    void setCrownIcons(const QPixmap &leftCrown, const QPixmap &rightCrown);
    void setIndex(int index);
    void setCrownState(bool left, bool right);

    QPair<bool, bool> crownStates() const;

signals:
    void crownStateChanged(int index, bool m_leftActive, bool m_rightActive);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:

    QPixmap m_mainIcon;
    QPixmap m_leftCrown;
    QPixmap m_rightCrown;

    int m_index = -1;
    bool m_leftActive = false;
    bool m_rightActive = false;

    QRect m_leftCrownRect;
    QRect m_rightCrownRect;

    QPixmap createGrayed(const QPixmap &pixmap, qreal opacity);
};

#endif // CROWNOVERLAYWIDGET_H
