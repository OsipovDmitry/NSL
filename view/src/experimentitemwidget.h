#ifndef EXPERIMENTITEMWIDGET_H
#define EXPERIMENTITEMWIDGET_H

#include <QWidget>

#include <model/forwarddecl.h>

namespace view
{

class ExperimentItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExperimentItemWidget(const model::Experiment&, QWidget* = nullptr);
};

}

#endif // EXPERIMENTITEMWIDGET_H
