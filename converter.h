#ifndef CONVERTER_H
#define CONVERTER_H

#include <QMainWindow>

class Converter : public QMainWindow
{
    Q_OBJECT

public:
    Converter(QWidget *parent = nullptr);
    ~Converter();
};
#endif // CONVERTER_H
