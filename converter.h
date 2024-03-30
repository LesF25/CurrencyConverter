#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Converter : public QWidget
{
    Q_OBJECT

public:
    Converter(QWidget *parent = nullptr);
    ~Converter();

private:
    QLineEdit* _edSum;
    QLineEdit* _edResult;
    QComboBox* _cboxCurrencyFrom;
    QComboBox* _cboxCurrencyTo;
    QPushButton* _btCalculate;
    QPushButton* _btSwapCurrency;
    QVBoxLayout* _ltMain;

    QNetworkAccessManager _networkManager;
    QNetworkReply* _reply = nullptr;

private slots:
    void parseData();
    void searchCurrency();
    void swapCurrency();

private:
    void initStyle();

};
