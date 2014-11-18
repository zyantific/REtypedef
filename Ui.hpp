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

#ifndef UI_HPP
#define UI_HPP

#include "ui_SubstitutionEditor.h"
#include "SubstitutionManager.hpp"

#include <QDialog>
#include <QAbstractListModel>

// ============================================================================================= //
// [SubstitutionModel]                                                                           //
// ============================================================================================= //

class SubstitutionModel : public QAbstractItemModel
{
    SubstitutionManager *m_substMgr;
public:
    explicit SubstitutionModel(SubstitutionManager* data=nullptr, QObject* parent=nullptr);
public: // Implementation of QAbstractItemModel interface.
    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    int columnCount(const QModelIndex& parent=QModelIndex()) const override;
    QModelIndex index(int row, int column, 
        const QModelIndex& parent=QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, 
        int role=Qt::DisplayRole) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
public: // Public interface.
    const Substitution* substitutionByIndex(const QModelIndex& index);
    void update();
public: // Accessors.
    SubstitutionManager* substitutionManager() { return m_substMgr; }
};

// ============================================================================================= //
// [SubstitutionEditor]                                                                          //
// ============================================================================================= //

class SubstitutionEditor : public QDialog
{
    Q_OBJECT

    Ui::SubstitutionEditor m_widgets;
    const Substitution* m_contextMenuSelectedItem;
public:
    SubstitutionEditor(QWidget* parent=nullptr);
    virtual ~SubstitutionEditor() {}
public:
    void setModel(SubstitutionModel* model);
    SubstitutionModel* model();
protected slots:
    void addSubstitution(bool); // any idea how ofen I wrote "substitution" today? goddamit.
    void displayContextMenu(const QPoint& point);
    void deleteSubstitution(bool);
};

// ============================================================================================= //

#endif // UI_HPP
