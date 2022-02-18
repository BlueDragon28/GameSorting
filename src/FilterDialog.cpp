/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2021 Erwan Saclier de la Bâtie
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "FilterDialog.h"
#include "UtilityInterfaceEditorModel.h"
#include "StarWidget.h"
#include "TableModel_UtilityInterface.h"
#include "TableModel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QStackedLayout>
#include <QLineEdit>
#include <QTableView>

FilterDialog::FilterDialog(
    TableModel* model,
    TableModel_UtilityInterface* interface,
    SqlUtilityTable& utility,
    QSqlDatabase& db,
    QWidget* parent) :
    QDialog(parent),
    m_model(model),
    m_interface(interface),
    m_utility(utility),
    m_db(db),
    m_stackedLayout(nullptr),
    m_nameText(new QLineEdit(this)),
    m_utilityVLayout(nullptr),
    m_utilityModel(nullptr),
    m_utilityView(nullptr),
    m_starWidget(new StarWidget(this)),
    m_lastIndex(0)
{
    resize(480, 380);
    setWindowTitle(tr("Filter selection"));
    createWidget();
}

void FilterDialog::createWidget()
{
    // Create the widgets inside the Dialog.
    QVBoxLayout* vLayout = new QVBoxLayout(this);

    QComboBox* comboBox = new QComboBox(this);
    comboBox->setEditable(false);
    if (m_model->listType() == ListType::GAMELIST)
    {
        comboBox->addItems(
            {
                "None",
                "Name",
                "Series",
                "Categories",
                "Developpers",
                "Publishers",
                "Platforms",
                "Services",
                "Rate"
            });
        comboBox->setCurrentIndex(0);
    }
    else if (m_model->listType() == ListType::MOVIESLIST)
    {
        comboBox->addItems(
            {
                "None",
                "Name",
                "Categories",
                "Directors",
                "Actors",
                "Productions",
                "Music",
                "Services",
                "Rate"
            });
    }
    else if (m_model->listType() == ListType::COMMONLIST)
    {
        comboBox->addItems(
            {
                "None",
                "Name",
                "Categories",
                "Authors",
                "Rate"
            });
    }
    else if (m_model->listType() == ListType::BOOKSLIST)
    {
        comboBox->addItems(
            {
                "None",
                "Name",
                "Categories",
                "Authors",
                "Publishers",
                "Services",
                "Rate"
            });
    }
    else
        comboBox->addItem("None");
    connect(comboBox, &QComboBox::currentIndexChanged, this, &FilterDialog::comboBoxChanged);
    vLayout->addWidget(comboBox);

    m_stackedLayout = new QStackedLayout(this);
    m_stackedLayout->setContentsMargins(0, 0, 0, 0);

    // Default, empty page.
    QWidget* emptyWidget = new QWidget(this);
    m_stackedLayout->addWidget(emptyWidget);

    // Name filter.
    QWidget* nameBaseWidget = new QWidget(this);
    QHBoxLayout* nameBaseWidgetLayout = new QHBoxLayout(nameBaseWidget);
    nameBaseWidgetLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* searchNameLabel = new QLabel(tr("Search:"), nameBaseWidget);
    nameBaseWidgetLayout->addWidget(searchNameLabel, 0);
    nameBaseWidgetLayout->addWidget(m_nameText, 1);
    m_stackedLayout->addWidget(nameBaseWidget);

    // Utilities
    QWidget* utilitiesBaseWidget = new QWidget(this);
    m_utilityVLayout = new QVBoxLayout(utilitiesBaseWidget);
    m_utilityVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* searchUtlitiesLabel = new QLabel(tr("Search:"), this);
    QLineEdit* searchUtilities = new QLineEdit(this);
    connect(this, &FilterDialog::tabChanged, searchUtilities, &QLineEdit::clear);
    QPushButton* searchUtilitiesBtn = new QPushButton(this);
    connect(searchUtilitiesBtn, &QPushButton::clicked, [this, searchUtilities](){this->searchUtilities(searchUtilities->text());});
    QIcon searchIcon(":/Images/Search.svg");
    searchUtilitiesBtn->setIcon(searchIcon);
    QHBoxLayout* searchLayout = new QHBoxLayout(this);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->addWidget(searchUtlitiesLabel, 0);
    searchLayout->addWidget(searchUtilities, 1);
    searchLayout->addWidget(searchUtilitiesBtn, 0);
    m_utilityVLayout->addLayout(searchLayout, 0);

    m_stackedLayout->addWidget(utilitiesBaseWidget);

    // Rate
    QWidget* rateBaseWidget = new QWidget(this);
    QHBoxLayout* rateBaseWidgetLayout = new QHBoxLayout(rateBaseWidget);
    rateBaseWidgetLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* searchRateLabel = new QLabel(tr("Rate:"), rateBaseWidget);
    rateBaseWidgetLayout->addStretch(1);
    rateBaseWidgetLayout->addWidget(searchRateLabel, 0);
    rateBaseWidgetLayout->addWidget(m_starWidget, 0);
    rateBaseWidgetLayout->addStretch(1);
    m_stackedLayout->addWidget(rateBaseWidget);

    vLayout->addLayout(m_stackedLayout);

    // Apply and Cancel button
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
    QPushButton* applyButton = new QPushButton(tr("Apply"), this);
    hLayout->addStretch(1);
    hLayout->addWidget(cancelButton, 0);
    hLayout->addWidget(applyButton, 0);
    vLayout->addLayout(hLayout);
    applyButton->setDefault(true);

    connect(applyButton, &QPushButton::clicked, this, &FilterDialog::applyFilter);
    connect(cancelButton, &QPushButton::clicked, this, &FilterDialog::reject);
}

void FilterDialog::applyFilter()
{
    // Applying the selected filter into the model TableModel.
    if (!m_model || !m_interface)
    {
        reject();
        return;
    }

    if (m_model->listType() == ListType::GAMELIST)
    {
        if (m_lastIndex == 1)
        {
            ListFilter filter = {};
            filter.column = Game::NAME;
            filter.pattern = m_nameText->text();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex >= 2 && m_lastIndex <= 7)
        {
            if (!m_utilityView || !m_utilityModel)
                reject();

            int columnID;
            if (m_lastIndex == 2)
                columnID = Game::SERIES;
            else if (m_lastIndex == 3)
                columnID = Game::CATEGORIES;
            else if (m_lastIndex == 4)
                columnID = Game::DEVELOPPERS;
            else if (m_lastIndex == 5)
                columnID = Game::PUBLISHERS;
            else if (m_lastIndex == 6)
                columnID = Game::PLATFORMS;
            else if (m_lastIndex == 7)
                columnID = Game::SERVICES;
            
            ListFilter filter = {};
            filter.column = columnID;
            filter.utilityList = m_utilityModel->getSelectedUtilities();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex == 7)
        {
            ListFilter filter = {};
            filter.column = Game::RATE;
            filter.rate = m_starWidget->getValue();
            m_model->setFilter(filter);
            accept();
        }
        else
        {
            ListFilter filter = {};
            filter.column = -1;
            m_model->setFilter(filter);
            reject();
        }
    }
    else if (m_model->listType() == ListType::MOVIESLIST)
    {
        if (m_lastIndex == 1)
        {
            ListFilter filter = {};
            filter.column = Movie::NAME;
            filter.pattern = m_nameText->text();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex >= 2 && m_lastIndex <= 7)
        {
            if (!m_utilityView || !m_utilityModel)
                reject();

            int columnID;
            if (m_lastIndex == 2)
                columnID = Movie::CATEGORIES;
            else if (m_lastIndex == 3)
                columnID = Movie::DIRECTORS;
            else if (m_lastIndex == 4)
                columnID = Movie::ACTORS;
            else if (m_lastIndex == 5)
                columnID = Movie::PRODUCTIONS;
            else if (m_lastIndex == 6)
                columnID = Movie::MUSIC;
            else if (m_lastIndex == 7)
                columnID = Movie::SERVICES;
            
            ListFilter filter = {};
            filter.column = columnID;
            filter.utilityList = m_utilityModel->getSelectedUtilities();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex == 8)
        {
            ListFilter filter = {};
            filter.column = Movie::RATE;
            filter.rate = m_starWidget->getValue();
            m_model->setFilter(filter);
            accept();
        }
        else
        {
            ListFilter filter = {};
            filter.column = -1;
            m_model->setFilter(filter);
            reject();
        }
    }
    else if (m_model->listType() == ListType::COMMONLIST)
    {
        if (m_lastIndex == 1)
        {
            ListFilter filter = {};
            filter.column = Common::NAME;
            filter.pattern = m_nameText->text();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex >= 2 && m_lastIndex <= 3)
        {
            if (!m_utilityView || m_lastIndex <= 7)
                reject();
            
            int columnID;
            if (m_lastIndex == 2)
                columnID = Common::CATEGORIES;
            else if (m_lastIndex == 3)
                columnID = Common::AUTHORS;
            
            ListFilter filter = {};
            filter.column = columnID;
            filter.utilityList = m_utilityModel->getSelectedUtilities();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex == 4)
        {
            ListFilter filter = {};
            filter.column = Common::RATE;
            filter.rate = m_starWidget->getValue();
            m_model->setFilter(filter);
            accept();
        }
        else
        {
            ListFilter filter = {};
            filter.column = -1;
            m_model->setFilter(filter);
            reject();
        }
    }
    else if (m_model->listType() == ListType::BOOKSLIST)
    {
        if (m_lastIndex == 1)
        {
            ListFilter filter = {};
            filter.column = Books::NAME;
            filter.pattern = m_nameText->text();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex >= 2 && m_lastIndex <= 5)
        {
            if(!m_utilityView || !m_utilityModel)
                reject();
            
            int columnID;
            if (m_lastIndex == 2)
                columnID = Books::CATEGORIES;
            else if (m_lastIndex == 3)
                columnID = Books::AUTHORS;
            else if (m_lastIndex == 4)
                columnID = Books::PUBLISHERS;
            else if (m_lastIndex == 5)
                columnID = Books::SERVICES;
            
            ListFilter filter = {};
            filter.column = columnID;
            filter.utilityList = m_utilityModel->getSelectedUtilities();
            m_model->setFilter(filter);
            accept();
        }
        else if (m_lastIndex == 6)
        {
            ListFilter filter = {};
            filter.column = Books::RATE;
            filter.rate = m_starWidget->getValue();
            m_model->setFilter(filter);
            accept();
        }
        else
        {
            ListFilter filter = {};
            filter.column = -1;
            m_model->setFilter(filter);
            reject();
        }
    }
    else 
        reject();
}

void FilterDialog::comboBoxChanged(int index)
{
    // Change the page of the stacked layout when the combo box index change.
    if (!m_model || !m_interface) return;
    if (index == m_lastIndex) return;

    if (m_utilityView)
    {
        delete m_utilityView;
        m_utilityView = nullptr;
    }
    if (m_utilityModel)
    {
        delete m_utilityModel;
        m_utilityModel = nullptr;
    }

    if (m_model->listType() == ListType::GAMELIST)
    {
        if (index == 0)
            m_stackedLayout->setCurrentIndex(0);
        else if (index == 1)
        {
            m_stackedLayout->setCurrentIndex(1);
            m_nameText->clear();
        }
        else if (index >= 2 && index <= 7)
        {
            UtilityTableName tableName;
            if (index == 2)
                tableName = UtilityTableName::SERIES;
            else if (index == 3)
                tableName = UtilityTableName::CATEGORIES;
            else if (index == 4)
                tableName = UtilityTableName::DEVELOPPERS;
            else if (index == 5)
                tableName = UtilityTableName::PUBLISHERS;
            else if (index == 6)
                tableName = UtilityTableName::PLATFORM;
            else if (index == 7)
                tableName = UtilityTableName::SERVICES;
            
            m_utilityModel = new UtilityInterfaceEditorModel(
                tableName,
                m_model,
                m_interface,
                m_utility,
                m_db,
                this);
            m_utilityView = new QTableView(this);
            m_utilityView->setModel(m_utilityModel);
            m_utilityView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            m_utilityView->horizontalHeader()->setSortIndicatorClearable(true);
            m_utilityView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
            m_utilityView->setSortingEnabled(true);
            m_utilityView->verticalHeader()->hide();
            m_utilityVLayout->addWidget(m_utilityView, 1);
            m_stackedLayout->setCurrentIndex(2);

            emit tabChanged();
        }
        else
        {
            m_stackedLayout->setCurrentIndex(3);
            m_starWidget->setValue(0);
        }
    }
    else if (m_model->listType() == ListType::MOVIESLIST)
    {
        if (index == 0)
            m_stackedLayout->setCurrentIndex(0);
        else if (index == 1)
        {
            m_stackedLayout->setCurrentIndex(1);
            m_nameText->clear();
        }
        else if (index >= 2 && index <= 7)
        {
            UtilityTableName tableName;
            if (index == 2)
                tableName = UtilityTableName::CATEGORIES;
            else if (index == 3)
                tableName = UtilityTableName::DIRECTOR;
            else if (index == 4)
                tableName = UtilityTableName::ACTORS;
            else if (index == 5)
                tableName = UtilityTableName::PRODUCTION;
            else if (index == 6)
                tableName = UtilityTableName::MUSIC;
            else if (index == 7)
                tableName = UtilityTableName::SERVICES;

            m_utilityModel = new UtilityInterfaceEditorModel(
                tableName,
                m_model,
                m_interface,
                m_utility,
                m_db,
                this);
            m_utilityView = new QTableView(this);
            m_utilityView->setModel(m_utilityModel);
            m_utilityView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            m_utilityView->horizontalHeader()->setSortIndicatorClearable(true);
            m_utilityView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
            m_utilityView->setSortingEnabled(true);
            m_utilityView->verticalHeader()->hide();
            m_utilityVLayout->addWidget(m_utilityView, 1);
            m_stackedLayout->setCurrentIndex(2);

            emit tabChanged();
        }
        else
        {
            m_stackedLayout->setCurrentIndex(3);
            m_starWidget->setValue(0);
        }
    }
    else if (m_model->listType() == ListType::COMMONLIST)
    {
        if (index == 0)
            m_stackedLayout->setCurrentIndex(0);
        else if (index == 1)
        {
            m_stackedLayout->setCurrentIndex(1);
            m_nameText->clear();
        }
        else if (index >= 2 && index <= 3)
        {
            UtilityTableName tableName;
            if (index == 2)
                tableName = UtilityTableName::CATEGORIES;
            else if (index == 3)
                tableName = UtilityTableName::AUTHORS;
            
            m_utilityModel = new UtilityInterfaceEditorModel(
                tableName,
                m_model,
                m_interface,
                m_utility,
                m_db,
                this);
            m_utilityView = new QTableView(this);
            m_utilityView->setModel(m_utilityModel);
            m_utilityView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            m_utilityView->horizontalHeader()->setSortIndicatorClearable(true);
            m_utilityView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
            m_utilityView->setSortingEnabled(true);
            m_utilityView->verticalHeader()->hide();
            m_utilityVLayout->addWidget(m_utilityView, 1);
            m_stackedLayout->setCurrentIndex(2);

            emit tabChanged();
        }
        else
        {
            m_stackedLayout->setCurrentIndex(3);
            m_starWidget->setValue(0);
        }
    }
    else if (m_model->listType() == ListType::BOOKSLIST)
    {
        if (index == 0)
            m_stackedLayout->setCurrentIndex(0);
        else if (index == 1)
        {
            m_stackedLayout->setCurrentIndex(1);
            m_nameText->clear();
        }
        else if (index >= 2 && index <= 5)
        {
            UtilityTableName tableName;
            if (index == 2)
                tableName = UtilityTableName::CATEGORIES;
            else if (index == 3)
                tableName = UtilityTableName::AUTHORS;
            else if (index == 4)
                tableName = UtilityTableName::PUBLISHERS;
            else if (index == 5)
                tableName = UtilityTableName::SERVICES;
            
            m_utilityModel = new UtilityInterfaceEditorModel(
                tableName,
                m_model,
                m_interface,
                m_utility,
                m_db,
                this);
            m_utilityView = new QTableView(this);
            m_utilityView->setModel(m_utilityModel);
            m_utilityView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            m_utilityView->horizontalHeader()->setSortIndicatorClearable(true);
            m_utilityView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
            m_utilityView->setSortingEnabled(true);
            m_utilityView->verticalHeader()->hide();
            m_utilityVLayout->addWidget(m_utilityView, 1);
            m_stackedLayout->setCurrentIndex(2);

            emit tabChanged();
        }
        else
        {
            m_stackedLayout->setCurrentIndex(3);
            m_starWidget->setValue(0);
        }
    }
    else
        m_stackedLayout->setCurrentIndex(0);

    m_lastIndex = index;
}

void FilterDialog::searchUtilities(const QString& pattern)
{
    if (m_utilityModel)
        m_utilityModel->setFilter(pattern);
}