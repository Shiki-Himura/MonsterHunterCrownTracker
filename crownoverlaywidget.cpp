#include "crownoverlaywidget.h"
#include "qgridlayout.h"
#include "qsettings.h"
#include <QPainter>
#include <QMouseEvent>
#include <QList>

QStringList CrownOverlayWidget::g_monsterNames;

CrownOverlayWidget::CrownOverlayWidget(QWidget *parent)
    : QWidget(parent), g_monsterLabel(nullptr)
{
    setFixedSize(125, 125);
    g_monsterLabel = new QLabel(this);
    g_monsterLabel->setAlignment(Qt::AlignCenter);
    g_monsterLabel->setGeometry(0, height() - 20, width(), 20);
}

QWidget *CrownOverlayWidget::CreateMonsterGrid(QWidget *parent)
{
    if(g_monsterNames.isEmpty())
    {
        CreateMonsterNameList();
    }

    const int monsterAmount = 26;
    QWidget *container = new QWidget(parent);
    QGridLayout *grid = new QGridLayout(container);
    container->setLayout(grid);

    QPixmap crownLeft(":/icons/MHWilds_Icons/small_crown.png");
    QPixmap crownRight(":/icons/MHWilds_Icons/big_crown.png");

    QSettings settings("Shiki", "CrownTracker");

    for (int i = 0; i <= monsterAmount; i++)
    {
        auto *widget = new CrownOverlayWidget(container);
        widget->setIndex(i);
        widget->setMainIcon(QPixmap(QString(":/icons/MHWilds_Icons/monster (%1).png").arg(i)));

        // add a label to the widget, index of the monster corresponding to the list of names
        if (i < g_monsterNames.size())
        {
            widget->g_monsterLabel->setText(g_monsterNames.at(i));
        }
        else
        {
            widget->g_monsterLabel->setText(QString("Monster %1").arg(i));
        }

        widget->setCrownIcons(crownLeft, crownRight);

        // Load saved state from settings
        bool leftActive = settings.value(QString("monster_%1_left").arg(i), false).toBool();
        bool rightActive = settings.value(QString("monster_%1_right").arg(i), false).toBool();
        widget->setCrownState(leftActive, rightActive);

        // Connect the signal to save the state
        connect(widget, &CrownOverlayWidget::crownStateChanged, container,
                [](int index, bool leftActive, bool rightActive)
                {
                    QSettings settings("Shiki", "CrownTracker");
                    settings.setValue(QString("monster_%1_left").arg(index), leftActive);
                    settings.setValue(QString("monster_%1_right").arg(index), rightActive);
                });

        grid->addWidget(widget, i / 6, i % 6);
    }
    return container;
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

void CrownOverlayWidget::CreateMonsterNameList()
{
    g_monsterNames = {
        "Ajarakan",
        "Alpha-Doshaguma",
        "Arkveld",
        "Balahara",
        "Blangonga",
        "Chatacabra",
        "Congalala",
        "Doshaguma",
        "Gore-Magala",
        "Gravios",
        "G. Ebony-Odogaron",
        "G. Fulgur-Anjanath",
        "G. Rathalos",
        "Gypceros",
        "Hirabami",
        "Lala-Barina",
        "Mizutsune",
        "Nerscylla",
        "Nu-Udra",
        "Quematrice",
        "Rathalos",
        "Rathian",
        "Rey-Dau",
        "Rompopolo",
        "Uth-Duna",
        "Xu-Wu",
        "Yian-Kut-Ku"
    };
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
        if (m_leftActive == true)
        {
            m_leftCrownClicked = QDateTime::currentDateTime(); // TODO: Add to history
        }
        emit crownStateChanged(m_index, m_leftActive, m_rightActive);
        update();
    }
    else if (m_rightCrownRect.contains(event->pos()))
    {
        m_rightActive = !m_rightActive;
        if (m_rightActive == true)
        {
            m_rightCrownClicked = QDateTime::currentDateTime(); // TODO: Add to history
        }
        emit crownStateChanged(m_index, m_leftActive, m_rightActive);
        update();
    }

    if (m_leftCrownClicked.isValid() || m_rightCrownClicked.isValid())
    {
        m_history.append({m_mainIcon, QDateTime::currentDateTime()});
    }
}

void CrownOverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Determine the visual state
    if (!m_mainIcon.isNull())
    {
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
