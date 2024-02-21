#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Converter : public QWidget
{
    Q_OBJECT

public:
    Converter(QWidget *parent = nullptr);
    ~Converter();

private:
    QLineEdit* m_sumEdit;
    QLineEdit* m_resultEdit;
    QComboBox* m_currencyFrom;
    QComboBox* m_currencyTo;
    QPushButton* m_calculateButton;

    QNetworkAccessManager m_networkManager;
    QNetworkReply* m_reply = nullptr;
private slots:
    void parseData();
    void searchCurrency();

};
