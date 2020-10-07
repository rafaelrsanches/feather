// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020, The Monero Project.

#include "settings.h"
#include "ui_settings.h"
#include "appcontext.h"
#include "utils/config.h"
#include "mainwindow.h"

Settings::Settings(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("://assets/images/appicons/64x64.png"));

    if(m_ctx == nullptr)
        m_ctx = MainWindow::getContext();

    ui->tabWidget->setTabVisible(2, false);
    ui->tabWidget->setTabVisible(4, false);

    connect(ui->btnCopyToClipboard, &QPushButton::clicked, this, &Settings::copyToClipboard);
    connect(ui->checkBox_checkForAppUpdates, &QCheckBox::clicked, this, &Settings::checkboxExternalLinkWarn);
    connect(ui->checkBox_externalLink, &QCheckBox::clicked, this, &Settings::checkboxExternalLinkWarn);
    connect(ui->closeButton, &QDialogButtonBox::accepted, this, &Settings::close);

    // nodes
    ui->nodeWidget->setupUI(m_ctx);
    connect(ui->nodeWidget, &NodeWidget::nodeSourceChanged, m_ctx->nodes, &Nodes::onNodeSourceChanged);
    connect(ui->nodeWidget, &NodeWidget::connectToNode, m_ctx->nodes, QOverload<FeatherNode>::of(&Nodes::connectToNode));

    // setup checkboxes
    ui->checkBox_externalLink->setChecked(config()->get(Config::warnOnExternalLink).toBool());
    ui->checkBox_checkForAppUpdates->setChecked(config()->get(Config::checkForAppUpdates).toBool());

    // setup comboboxes
    auto settingsHomeWidget = config()->get(Config::homeWidget).toString();
    if (m_homeWidgets.contains(settingsHomeWidget))
        ui->comboBox_homeWidget->setCurrentIndex(m_homeWidgets.indexOf(settingsHomeWidget));

    this->setupSkinCombobox();
    auto settingsSkin = config()->get(Config::skin).toString();
    if (m_skins.contains(settingsSkin))
        ui->comboBox_skin->setCurrentIndex(m_skins.indexOf(settingsSkin));

    connect(ui->comboBox_homeWidget, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Settings::comboBox_homeWidgetChanged);
    connect(ui->comboBox_skin, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Settings::comboBox_skinChanged);
    connect(ui->comboBox_blockExplorer, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Settings::comboBox_blockExplorerChanged);

    // setup preferred fiat currency combobox
    QStringList fiatCurrencies;
    for (int index = 0; index < ui->comboBox_fiatCurrency->count(); index++)
        fiatCurrencies << ui->comboBox_fiatCurrency->itemText(index);

    auto preferredFiatCurrency = config()->get(Config::preferredFiatCurrency).toString();
    if(!preferredFiatCurrency.isEmpty())
        if(fiatCurrencies.contains(preferredFiatCurrency))
            ui->comboBox_fiatCurrency->setCurrentText(preferredFiatCurrency);

    connect(ui->comboBox_fiatCurrency, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Settings::fiatCurrencySelected);

    // setup path tab
#if defined(Q_OS_LINUX)
    ui->textEditPaths->appendPlainText(m_ctx->applicationPath);
#endif
    ui->textEditPaths->appendPlainText(m_ctx->configDirectory);
    ui->textEditPaths->appendPlainText(m_ctx->defaultWalletDir);

    this->adjustSize();
}

void Settings::fiatCurrencySelected(int index) {
    QString selection = ui->comboBox_fiatCurrency->itemText(index);
    config()->set(Config::preferredFiatCurrency, selection);
    emit preferredFiatCurrencyChanged(selection);
}

void Settings::checkboxCheckForAppUpdates() {
    bool state = ui->checkBox_checkForAppUpdates->isChecked();
    config()->set(Config::checkForAppUpdates, state);
}

void Settings::comboBox_skinChanged(int pos) {
    emit skinChanged(m_skins.at(pos));
}

void Settings::comboBox_homeWidgetChanged(int pos) {
    config()->set(Config::homeWidget, m_homeWidgets.at(pos));

    emit homeWidgetChanged(m_homeWidgets.at(pos));
}

void Settings::comboBox_blockExplorerChanged(int pos) {
    QString blockExplorer = ui->comboBox_blockExplorer->currentText();
    config()->set(Config::blockExplorer, blockExplorer);
    emit blockExplorerChanged(blockExplorer);
}

void Settings::copyToClipboard() {
    ui->textLogs->copy();
}

void Settings::checkboxExternalLinkWarn() {
    bool state = ui->checkBox_externalLink->isChecked();
    config()->set(Config::warnOnExternalLink, state);
}

void Settings::setupSkinCombobox() {
#if defined(Q_OS_WIN)
    m_skins.removeOne("Breeze/Dark");
    m_skins.removeOne("Breeze/Light");
#elif defined(Q_OS_MAC)
    m_skins.removeOne("QDarkStyle");
#endif

    ui->comboBox_skin->insertItems(0, m_skins);
}

Settings::~Settings() {
    delete ui;
}
