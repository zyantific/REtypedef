/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 athre0z
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Ui.hpp"

#include "Config.hpp"

#include <cassert>
#include <QMessageBox>
#include <QMenu>

// ============================================================================================== //
// [SubstitutionModel]                                                                            //
// ============================================================================================== //

SubstitutionModel::SubstitutionModel(SubstitutionManager *data, QObject *parent)
    : QAbstractItemModel(parent)
    , m_substMgr(data)
{
    
}

int SubstitutionModel::rowCount(const QModelIndex &parent) const
{
    return m_substMgr->rules().size();
}

int SubstitutionModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

QModelIndex SubstitutionModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, nullptr);
}

QVariant SubstitutionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    assert(index.row() >= m_substMgr->substitutions().size());
    auto sbst = m_substMgr->rules().at(index.row());
    return index.column() == 0 ? QString::fromStdString(sbst->regexpPattern)
        : QString::fromStdString(sbst->replacement);
}

QVariant SubstitutionModel::headerData(int section, 
    Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
        case 0:
            return "Search text";
        case 1:
            return "Replacement";
        default:
            return QVariant();
    }
}

QModelIndex SubstitutionModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

Qt::ItemFlags SubstitutionModel::flags(const QModelIndex &index) const
{
    return Qt::NoItemFlags;
}

const Substitution* SubstitutionModel::substitutionByIndex(const QModelIndex& index)
{
    assert(m_substMgr);
    assert(m_substMgr->substitutions().size() > index.row());
    return m_substMgr->rules().at(index.row()).get();
}

void SubstitutionModel::update()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

// ============================================================================================== //
// [SubstitutionEditor]                                                                           //
// ============================================================================================== //

SubstitutionEditor::SubstitutionEditor(QWidget* parent)
    : QDialog(parent)
{
    m_widgets.setupUi(this);
    
    connect(m_widgets.tvSubstitutions, 
        SIGNAL(customContextMenuRequested(const QPoint&)),
        SLOT(displayContextMenu(const QPoint&)));
    connect(m_widgets.btnAdd, SIGNAL(clicked(bool)), SLOT(addSubstitution(bool)));
    m_widgets.tvSubstitutions->setContextMenuPolicy(Qt::CustomContextMenu);
}

void SubstitutionEditor::setModel(SubstitutionModel* model)
{
    assert(model);
    m_widgets.tvSubstitutions->setModel(model);
}

SubstitutionModel* SubstitutionEditor::model()
{
    return static_cast<SubstitutionModel*>(m_widgets.tvSubstitutions->model());
}

void SubstitutionEditor::addSubstitution(bool)
{
    assert(model());
    assert(model()->substitutionManager());

    auto regexp = m_widgets.leSearchText->text().toStdString();

    // Empty?
    if (regexp.empty())
    {
        QMessageBox::warning(qApp->activeWindow(), PLUGIN_NAME, "Search text may not be empty!");
        return;
    }

    // Unique?
    const auto& substs = model()->substitutionManager()->rules();
    auto it = std::find_if(substs.cbegin(), substs.cend(), 
        [&regexp](const std::shared_ptr<Substitution>& cur) -> bool
    {
        assert(cur);
        return cur->regexpPattern == regexp;
    });

    if (it != substs.cend())
    {
        QMessageBox::warning(qApp->activeWindow(), PLUGIN_NAME, "Search texts must be unique!");
        return;
    }

    // Valid?
    auto newSubst = std::make_shared<Substitution>();
    try
    {
        newSubst->regexp = std::regex(regexp, std::regex_constants::optimize);
    }
    catch (const std::regex_error& e)
    {
        QMessageBox::warning(qApp->activeWindow(), PLUGIN_NAME,
            QString("The given regexp does not seem to be valid:\n") + e.what());
        return;
    }
    
    newSubst->replacement = m_widgets.leReplacement->text().toStdString();
    newSubst->regexpPattern = regexp;

    // Sane, add to list.
    m_widgets.leSearchText->clear();
    m_widgets.leReplacement->clear();
    model()->substitutionManager()->addRule(std::move(newSubst));
    model()->update();
}

void SubstitutionEditor::displayContextMenu(const QPoint& point)
{
    QModelIndex index = m_widgets.tvSubstitutions->indexAt(point);

    if (!index.isValid())
        return;

    assert(model());
    m_contextMenuSelectedItem = model()->substitutionByIndex(index);
    assert(m_contextMenuSelectedItem);

    QMenu menu(this);
    QAction *deleteAction = menu.addAction("&Delete");
    connect(deleteAction, SIGNAL(triggered(bool)), SLOT(deleteSubstitution(bool)));
    QAction *editAction = menu.addAction("&Edit");
    connect(editAction, SIGNAL(triggered(bool)), SLOT(editSubstitution(bool)));

    menu.exec(m_widgets.tvSubstitutions->viewport()->mapToGlobal(point));
}

void SubstitutionEditor::deleteSubstitution(bool)
{
    if (QMessageBox::question(qApp->activeWindow(), PLUGIN_NAME, 
        "Are you sure your want to delete this entry?", QMessageBox::Yes | QMessageBox::No) 
            != QMessageBox::Yes)
    {
        return;
    }

    assert(model());
    assert(model()->substitutionManager());
    assert(m_contextMenuSelectedItem);
    model()->substitutionManager()->removeRule(m_contextMenuSelectedItem);
    m_contextMenuSelectedItem = nullptr;
    model()->update();
}

void SubstitutionEditor::editSubstitution(bool)
{
    assert(model());
    assert(model()->substitutionManager());
    assert(m_contextMenuSelectedItem);
    m_widgets.leSearchText->setText(QString::fromStdString(
        m_contextMenuSelectedItem->regexpPattern));
    m_widgets.leReplacement->setText(QString::fromStdString(
        m_contextMenuSelectedItem->replacement));
    model()->substitutionManager()->removeRule(m_contextMenuSelectedItem);
    m_contextMenuSelectedItem = nullptr;
    model()->update();
}

// ============================================================================================== //
