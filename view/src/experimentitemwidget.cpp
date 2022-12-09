#include <QLabel>
#include <QHBoxLayout>

#include <model/types.h>

#include "experimentitemwidget.h"

namespace view
{

ExperimentItemWidget::ExperimentItemWidget(const model::Experiment& experiment, QWidget *parent)
    : QWidget(parent)
{
    QPixmap colorPixmap(16, 16);
    colorPixmap.fill(experiment.color);

    auto layout = new QHBoxLayout;

    auto label1 = new QLabel;
    label1->setPixmap(colorPixmap);
    layout->addWidget(label1);

    auto label2 = new QLabel(QString::fromStdWString(experiment.name));
    layout->addWidget(label2);

    if (experiment.mark != model::Mark::NoMark)
    {
        QPixmap likePixmap(":res/like.png");
        likePixmap = likePixmap.scaled(16, 16);
        if (experiment.mark == model::Mark::Dislike)
            likePixmap = likePixmap.transformed(QTransform::fromScale(1., -1.));

        auto label3 = new QLabel;
        label3->setPixmap(likePixmap);
        layout->addWidget(label3);
    }

    layout->addStretch();
    layout->setMargin(1);

    setLayout(layout);
}

}
