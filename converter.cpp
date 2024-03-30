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
    _ltMain = new QVBoxLayout;
    this->setLayout(_ltMain);
    this->setFixedSize(QSize(300, 155));

//=====================Summary Layout=========================
    QHBoxLayout* sumLayout = new QHBoxLayout;
    QLabel* sumLabel = new QLabel("Sum", this);
    _edSum = new QLineEdit("", this);
    _edSum->setFixedWidth(150);
    _edSum->setValidator(new QDoubleValidator);

    sumLayout->addWidget(sumLabel);
    sumLayout->addWidget(_edSum);

//=====================Currency Layout=========================
    QHBoxLayout* currencyLayout = new QHBoxLayout;
    QLabel* currencyLabelFrom = new QLabel("Convert currency", this);
    QSpacerItem* spacerCurrency = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    _btSwapCurrency = new QPushButton(this);
    _cboxCurrencyFrom = new QComboBox(this);
    _cboxCurrencyTo = new QComboBox(this);

    QVector<QString> currencyVec = {"RUB", "USD", "EUR", "CNY"};
    for(const auto& cur : currencyVec)
    {
        _cboxCurrencyFrom->addItem(cur);
        _cboxCurrencyTo->addItem(cur);
    }

    currencyLayout->addWidget(currencyLabelFrom);
    currencyLayout->addItem(spacerCurrency);
    currencyLayout->addWidget(_cboxCurrencyFrom);
    currencyLayout->addWidget(_btSwapCurrency);
    currencyLayout->addWidget(_cboxCurrencyTo);

//======================Result Layout==========================
    QHBoxLayout* resultLayout = new QHBoxLayout;
    QLabel* resultLabel = new QLabel("Result", this);
    _edResult = new QLineEdit(this);
    _edResult->setReadOnly(true);
    _edResult->setFixedWidth(150);

    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(_edResult);

//======================Convert Layout==========================
    QHBoxLayout* convertLayout = new QHBoxLayout;
    _btCalculate = new QPushButton("Calculate", this);
    QSpacerItem* spacerConvert = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    convertLayout->addItem(spacerConvert);
    convertLayout->addWidget(_btCalculate);
//==============================================================
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    _ltMain->addLayout(sumLayout);
    _ltMain->addLayout(currencyLayout);
    _ltMain->addLayout(resultLayout);
    _ltMain->addWidget(line);
    _ltMain->addLayout(convertLayout);

    initStyle();

//========================= Connect =============================
    connect(_btSwapCurrency, SIGNAL(clicked()), this, SLOT(swapCurrency()));
    connect(_btCalculate, SIGNAL(clicked()), this, SLOT(searchCurrency()));
}

void Converter::initStyle()
{
    int idGSL = QFontDatabase::addApplicationFont(":/resource/fonts/GSL.ttf");
    QString familyGSL = QFontDatabase::applicationFontFamilies(idGSL).at(0);
    QFont GSL(familyGSL);

    int idRL = QFontDatabase::addApplicationFont(":/resource/fonts/RL.ttf");
    QString familyRL = QFontDatabase::applicationFontFamilies(idRL).at(0);
    QFont SR(familyRL);

    GSL.setBold(true);
    GSL.setPointSize(11);
    this->setFont(GSL);
    GSL.setPointSize(10);
    _cboxCurrencyFrom->setFont(GSL);
    _cboxCurrencyTo->setFont(GSL);

    _edResult->setFont(SR);
    _edSum->setFont(SR);

    _btCalculate->setCursor(Qt::PointingHandCursor);
    _btSwapCurrency->setCursor(Qt::PointingHandCursor);
    _cboxCurrencyFrom->setCursor(Qt::PointingHandCursor);
    _cboxCurrencyTo->setCursor(Qt::PointingHandCursor);

    this->setWindowTitle("Converter");
    this->setWindowIcon((QIcon(":/resource/icons/iconApplication.png")));
    _btSwapCurrency->setIcon((QIcon(":/resource/icons/swapButton.png")));
    _btSwapCurrency->setStyleSheet("border: none;");

    this->setStyleSheet(
        "background-color: #fff;"
        "color: #000;"
    );

    _cboxCurrencyFrom->setStyleSheet(
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

    _cboxCurrencyTo->setStyleSheet(
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

    _edResult->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border: none; "
        "border-radius: 2px; "
        "padding: 3px;"
        );

    _edSum->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border: none; "
        "border-radius: 2px; "
        "padding: 3px;"
        );

    _btCalculate->setStyleSheet(
        "background-color: #ECECEC;"
        "color: #000;"
        "border-radius: 10px; "
        "padding: 3px;"
        );
}

void Converter::swapCurrency()
{
    int indexCurrency = _cboxCurrencyFrom->currentIndex();
    _cboxCurrencyFrom->setCurrentIndex(_cboxCurrencyTo->currentIndex());
    _cboxCurrencyTo->setCurrentIndex(indexCurrency);
}

void Converter::searchCurrency()
{
    if (!(_edResult->displayText().isEmpty()))
        _edResult->setText("");

    if (_edSum->displayText().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "The amount of currency has not been entered.");
        return;
    }

    if (_reply)
    {
        _reply->abort();
        _reply->deleteLater();
        _reply = nullptr;
    }

    QUrlQuery query;
    query.addQueryItem("apikey", API::apikey);
    query.addQueryItem("currencies", _cboxCurrencyTo->currentText());
    query.addQueryItem("base_currency", _cboxCurrencyFrom->currentText());

    _reply = _networkManager.get(QNetworkRequest(API::url + "?" + query.toString()));
    connect(_reply, SIGNAL(finished()), this, SLOT(parseData()));
}

void Converter::parseData()
{
    if (_reply->error() == QNetworkReply::NoError)
    {
        QByteArray dataArray = _reply->readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(dataArray);
        QJsonObject data = jsonDocument["data"].toObject();

        QString currencyStr = _cboxCurrencyTo->currentText();
        QJsonObject currency = data[currencyStr].toObject();

        QString valueSum = _edSum->text();
        for (int i = 0; i < valueSum.length(); i++)
            if (valueSum[i] == ',')
                valueSum[i] = '.';

        QString result = QString::number(currency["value"].toDouble() * valueSum.toDouble());
        _edResult->setText(result);
    }
    else if (_reply->error() != QNetworkReply::OperationCanceledError)
    {
        qCritical() << "Reply failed, error:" << _reply->errorString();
    }

    _reply->deleteLater();
    _reply = nullptr;
}

Converter::~Converter()
{
}

