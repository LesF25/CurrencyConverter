#include "converter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleValidator>
#include <QSpacerItem>
#include <QVector>
#include <QMessageBox>
#include <QApplication>
#include <QFrame>
#include <QFontDatabase>

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
    mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    this->setFixedSize(QSize(280, 135));

//=====================Summary Layout=========================
    QHBoxLayout* sumLayout = new QHBoxLayout;
    QLabel* sumLabel = new QLabel("Sum", this);
    m_sumEdit = new QLineEdit("", this);
    m_sumEdit->setFixedWidth(150);
    m_sumEdit->setValidator(new QDoubleValidator);

    sumLayout->addWidget(sumLabel);
    sumLayout->addWidget(m_sumEdit);

//=====================Currency Layout=========================
    QHBoxLayout* currencyLayout = new QHBoxLayout;
    QLabel* currencyLabelFrom = new QLabel("Convert currency", this);
    m_swapCurrencyButton = new QPushButton(this);
    m_currencyFrom = new QComboBox(this);
    m_currencyTo = new QComboBox(this);

    QVector<QString> currencyVec = {"RUB", "USD", "EUR", "CNY"};
    for(const auto& cur : currencyVec)
    {
        m_currencyFrom->addItem(cur);
        m_currencyTo->addItem(cur);
    }

    currencyLayout->addWidget(currencyLabelFrom);
    currencyLayout->addWidget(m_currencyFrom);
    currencyLayout->addWidget(m_swapCurrencyButton);
    currencyLayout->addWidget(m_currencyTo);

//======================Result Layout==========================
    QHBoxLayout* resultLayout = new QHBoxLayout;
    QLabel* resultLabel = new QLabel("Result", this);
    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setFixedWidth(150);

    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(m_resultEdit);

//======================Convert Layout==========================
    QHBoxLayout* convertLayout = new QHBoxLayout;
    m_calculateButton = new QPushButton("Calculate", this);
//    m_clearButton = new QPushButton("Clear", this);
    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    convertLayout->addItem(spacer);
//    convertLayout->addWidget(m_clearButton);
    convertLayout->addWidget(m_calculateButton);
//==============================================================
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    mainLayout->addLayout(sumLayout);
    mainLayout->addLayout(currencyLayout);
    mainLayout->addLayout(resultLayout);
    mainLayout->addWidget(line);
    mainLayout->addLayout(convertLayout);

    initStyle();

//========================= Connect =============================
    connect(m_swapCurrencyButton, SIGNAL(clicked()), this, SLOT(swapCurrency()));
    connect(m_calculateButton, SIGNAL(clicked()), this, SLOT(searchCurrency()));
}

void Converter::initStyle()
{
    int id = QFontDatabase::addApplicationFont(":/resource/fonts/AR.otf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont AR(family);

    this->setFont(AR);
    m_currencyFrom->setFont(AR);
    m_currencyTo->setFont(AR);

    m_calculateButton->setCursor(Qt::PointingHandCursor);
    m_swapCurrencyButton->setCursor(Qt::PointingHandCursor);
    m_currencyFrom->setCursor(Qt::PointingHandCursor);
    m_currencyTo->setCursor(Qt::PointingHandCursor);

    this->setWindowTitle("Converter");
    this->setWindowIcon((QIcon(":/resource/icons/iconApplication.png")));
    m_swapCurrencyButton->setIcon((QIcon(":/resource/icons/swapButton.png")));
    m_swapCurrencyButton->setStyleSheet("border: none;");

    this->setStyleSheet(
        "background-color: #fff;"
        "color: #000;"
    );

    m_currencyFrom->setStyleSheet(
        "QComboBox"
        "{"
        "background-color: #ECECEC;"
        "color: #000;"
        "}"

        "QComboBox::drop-down"
        "{"
        "width: 0px;"
        "height: 0px;"
        "border: 0px;"
        "}"
        );

    m_currencyTo->setStyleSheet(
        "QComboBox"
        "{"
        "background-color: #ECECEC;"
        "color: #000;"
        "}"

        "QComboBox::drop-down"
        "{"
        "width: 0px;"
        "height: 0px;"
        "border: 0px;"
        "}"
    );

    m_resultEdit->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border: none; "
        "border-radius: 2px; "
        "padding: 3px;"
        );

    m_sumEdit->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border: none; "
        "border-radius: 2px; "
        "padding: 3px;"
        "font-weight: 700;"
        );

    m_calculateButton->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border-radius: 10px; "
        "padding: 5px;"
        );
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

    if (m_sumEdit->displayText().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "The amount of currency has not been entered.");
        return;
    }

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

