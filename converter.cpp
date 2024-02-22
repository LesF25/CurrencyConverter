#include "converter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDoubleValidator>
#include <QSpacerItem>

#include <QUrlQuery>
#include <QByteArray>
#include <QJsonDocument>
#include<QJsonObject>
#include <QJsonArray>

namespace API
{
    const QString& url = "https://api.currencyapi.com/v3/latest";
    const QString& apikey = "cur_live_ejIwwf5IvGqLcFxjK0eJwqc6X7ccKXKuhPAxf1BO";
}

Converter::Converter(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    QHBoxLayout* sumLayout = new QHBoxLayout;
    QLabel* sumLabel = new QLabel("Sum", this);
    m_sumEdit = new QLineEdit("", this);
    m_sumEdit->setFixedWidth(150);

    m_sumEdit->setValidator(new QDoubleValidator);
    sumLayout->addWidget(sumLabel);
    sumLayout->addWidget(m_sumEdit);

    QHBoxLayout* currencyLayout = new QHBoxLayout;
    QLabel* currencyLabelFrom = new QLabel("Convert currency", this);
    m_swapCurrencyButton = new QPushButton("Swap", this);
    m_currencyFrom = new QComboBox(this);
    m_currencyTo = new QComboBox(this);

    m_currencyFrom->addItem("RUB");
    m_currencyFrom->addItem("USD");
    m_currencyFrom->addItem("EUR");
    m_currencyFrom->addItem("CNY");

    m_currencyTo->addItem("RUB");
    m_currencyTo->addItem("USD");
    m_currencyTo->addItem("EUR");
    m_currencyTo->addItem("CNY");

    currencyLayout->addWidget(currencyLabelFrom);
    currencyLayout->addWidget(m_currencyFrom);
    currencyLayout->addWidget(m_swapCurrencyButton);
    currencyLayout->addWidget(m_currencyTo);

    QHBoxLayout* resultLayout = new QHBoxLayout;
    QLabel* resultLabel = new QLabel("Result", this);
    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setFixedWidth(150);
    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(m_resultEdit);

    QHBoxLayout* convertLayout = new QHBoxLayout;
    m_calculateButton = new QPushButton("Calculate", this);
    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    convertLayout->addItem(spacer);
    convertLayout->addWidget(m_calculateButton);

    mainLayout->addLayout(sumLayout);
    mainLayout->addLayout(currencyLayout);
    mainLayout->addLayout(resultLayout);
    mainLayout->addLayout(convertLayout);

    connect(m_calculateButton, SIGNAL(clicked()), this, SLOT(searchCurrency()));
    connect(m_swapCurrencyButton, SIGNAL(clicked()), this, SLOT(swapCurrency()));
}

void Converter::swapCurrency()
{
    int indexCurrency = m_currencyFrom->currentIndex();
    m_currencyFrom->setCurrentIndex(m_currencyTo->currentIndex());
    m_currencyTo->setCurrentIndex(indexCurrency);
}

void Converter::searchCurrency()
{
    if (!(m_resultEdit->displayText().isEmpty()))
        m_resultEdit->setText("");

    if (m_reply)
    {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    QUrlQuery query;
    query.addQueryItem("apikey", API::apikey);
    query.addQueryItem("currencies", m_currencyTo->currentText());
    query.addQueryItem("base_currency", m_currencyFrom->currentText());

    m_reply = m_networkManager.get(QNetworkRequest(API::url + "?" + query.toString()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(parseData()));

// https://api.currencyapi.com/v3/latest?apikey=cur_live_ejIwwf5IvGqLcFxjK0eJwqc6X7ccKXKuhPAxf1BO&currencies=EUR&base_currency=RUB
}

void Converter::parseData()
{
    if (m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray dataArray = m_reply->readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(dataArray);
        QJsonObject data = jsonDocument["data"].toObject();

        QString currencyStr = m_currencyTo->currentText();
        QJsonObject currency = data[currencyStr].toObject();

        QString result = QString::number(currency["value"].toDouble() * m_sumEdit->text().toDouble());
        m_resultEdit->setText(result);
    }
    else if (m_reply->error() != QNetworkReply::OperationCanceledError)
    {
        qCritical() << "Reply failed, error:" << m_reply->errorString();
    }

    m_reply->deleteLater();
    m_reply = nullptr;
}

Converter::~Converter()
{
}

