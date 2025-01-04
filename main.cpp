#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QInputDialog>
#include "clippingwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget window;
    window.setFixedSize(900, 700);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    ClippingWidget *clippingWidget = new ClippingWidget;
    layout->addWidget(clippingWidget);

    QComboBox *algorithmComboBox = new QComboBox;
    algorithmComboBox->addItem("Sutherland-Cohen", 0);
    algorithmComboBox->addItem("Liang-Barsky", 1);
    algorithmComboBox->addItem("Midpoint", 2);
    layout->addWidget(algorithmComboBox);

    QPushButton *loadButton = new QPushButton("Enter Data");
    layout->addWidget(loadButton);

    QObject::connect(loadButton, &QPushButton::clicked, [&](){
        bool ok;
        int n = QInputDialog::getInt(&window, "Input", "Enter number of lines:", 1, 1, 100, 1, &ok);
        if (ok) {
            clippingWidget->clearLines();
            for (int i = 0; i < n; ++i) {
                int x1 = QInputDialog::getInt(&window, "Input", "Enter x1:", 0, -1000, 1000, 1, &ok);
                int y1 = QInputDialog::getInt(&window, "Input", "Enter y1:", 0, -1000, 1000, 1, &ok);
                int x2 = QInputDialog::getInt(&window, "Input", "Enter x2:", 0, -1000, 1000, 1, &ok);
                int y2 = QInputDialog::getInt(&window, "Input", "Enter y2:", 0, -1000, 1000, 1, &ok);
                clippingWidget->addLine(x1, y1, x2, y2);
            }
            int xmin = QInputDialog::getInt(&window, "Input", "Enter xmin:", 0, -1000, 1000, 1, &ok);
            int ymin = QInputDialog::getInt(&window, "Input", "Enter ymin:", 0, -1000, 1000, 1, &ok);
            int xmax = QInputDialog::getInt(&window, "Input", "Enter xmax:", 0, -1000, 1000, 1, &ok);
            int ymax = QInputDialog::getInt(&window, "Input", "Enter ymax:", 0, -1000, 1000, 1, &ok);
            clippingWidget->setClippingWindow(xmin, ymin, xmax, ymax);
        }
    });
    QObject::connect(algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     clippingWidget, &ClippingWidget::setAlgorithm);

    window.show();
    return a.exec();
}
