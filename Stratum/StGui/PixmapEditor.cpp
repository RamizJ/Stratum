#include "PixmapEditor.h"
#include "ui_PixmapEditor.h"

#include <QGraphicsPixmapItem>

PixmapEditor::PixmapEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PixmapEditor),
    m_pixmapGraphicsItem(NULL)
{
    ui->setupUi(this);

    m_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
}

PixmapEditor::~PixmapEditor()
{
    delete ui;
}

QPixmap PixmapEditor::pixmap() const
{
    if(m_pixmapGraphicsItem)
        return m_pixmapGraphicsItem->pixmap();

    return QPixmap();
}

void PixmapEditor::setPixmap(const QPixmap& pixmap)
{
    m_pixmapGraphicsItem = m_scene->addPixmap(pixmap);
}
