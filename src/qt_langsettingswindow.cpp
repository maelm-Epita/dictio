#include "include/qt_langsettingswindow.h"
#include "include/qt_tbvcomboboxdelegate.h"
#include "ui_langsettingswindow.h"

#include <include/qt_tbvspinboxdelegate.h>


void LangSettingsWindow::SetupCustomUi(){
    ui->errLabel->setVisible(false);
    // scnd tab
    wattr_tbv = new AttributeTableView(ui->window);
    wattr_tbv->langwin = this;
    ui->gridLayout_4->replaceWidget(ui->wattrTblView, wattr_tbv);
    ui->wattrTblView->hide();
    ui->wattrTblView->deleteLater();
    ui->wattrTblView = wattr_tbv;
    ui->wattrTblView->horizontalHeader()->setStretchLastSection(true);
    wgrp_model = new QStandardItemModel(0, 2, 0);
    wattr_model = new QStandardItemModel(0, 3, 0);
    ui->wgrpTblView->setModel(wgrp_model);
    ui->wattrTblView->setModel(wattr_model);
    // third tab
    sylptn_prob_model = new QStandardItemModel(0,2,0);
    ltr_prob_model = new QStandardItemModel(0,2,0);
    wgrp_prob_model = new QStandardItemModel(0,2,0);
    wattr_prob_model = new QStandardItemModel(0,2,0);
    ui->sylptnprobTbl->setModel(sylptn_prob_model);
    ui->ltrprobTbl->setModel(ltr_prob_model);
    ui->wgrpprobTbl->setModel(wgrp_prob_model);
    ui->wattrprobTbl->setModel(wattr_prob_model);
}

void LangSettingsWindow::LoadContextSettings(){
    // first tab
    std::string lgrpstr = LetterGroup::LGStringFromLetterGroups(mainwin->context.generator.letter_groups);
    ui->ltrgrpTxtEdit->setPlainText(QString::fromStdString(lgrpstr));
    std::string spstr = LetterGroup::SPStringFromSyllablePatterns(mainwin->context.generator.syllable_patterns);
    ui->sylptnTxtEdit->setPlainText(QString::fromStdString(spstr));
    // second tab
    wgrp_model->clear();
    wgrp_model->setHorizontalHeaderLabels({"Name", "Attribute(s)"});
    wattr_model->clear();
    wattr_model->setHorizontalHeaderLabels({"Name", "Function", "Edit Function Parameter(s)"});
    ui->wattrTblView->setItemDelegateForColumn(1, new TBVComboBoxDelegate(wattr_tbv));
    std::vector<WordGroup> wgroups = mainwin->context.generator.word_groups;
    for (WordGroup grp : wgroups){
        std::string wgrp_id = grp.group_identifier;
        std::string wgrp_attr = "";
        for (unsigned int i=0; i<grp.possible_attributes.size(); i++){
            WordAttribute wattr = grp.possible_attributes[i];
            wgrp_attr += wattr.attribute_identifier;
            if (i<grp.possible_attributes.size()-1){
                wgrp_attr += ",";
            }
            bool alreadyAdded=0;
            for (unsigned int j=0; j<wattr_model->rowCount(); j++){
                std::string row_txt = wattr_model->data(wattr_model->index(j, 0)).toString().toStdString();
                if (row_txt == wattr.attribute_identifier) alreadyAdded=1;
            }
            if (!alreadyAdded){
                QList<QStandardItem*> row;
                row << new QStandardItem(QString::fromStdString(wattr.attribute_identifier));
                row << new QStandardItem(QString::fromStdString(WordAttributeFunctionPreset::GetWattrPresetName(wattr.attribute_func_preset.func_preset)));
                wattr_model->appendRow(row);
                std::vector<std::string> arg_fields;
                // Handle each preset type individually, donothing doesnt need anything pushed to the vector
                if (wattr.attribute_func_preset.func_preset == ENUM_WATTR_PRESET_ADD_PREFIX ||
                    wattr.attribute_func_preset.func_preset == ENUM_WATTR_PRESET_ADD_SUFFIX){
                    arg_fields.push_back(Syllable::ArgStringFromSyllables(std::get<std::vector<Syllable>>(wattr.attribute_func_arguments[0])));
                }
                pending_attribarg_fields.push_back(arg_fields);
            }
        }
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(wgrp_id));
        row << new QStandardItem(QString::fromStdString(wgrp_attr));
        wgrp_model->appendRow(row);
    }
    // third tab
    sylptn_prob_model->clear();
    ltr_prob_model->clear();
    wgrp_prob_model->clear();
    wattr_prob_model->clear();
    sylptn_prob_model->setHorizontalHeaderLabels({"Syllable pattern", "Probability"});
    ltr_prob_model->setHorizontalHeaderLabels({"Letter", "Probability"});
    wgrp_prob_model->setHorizontalHeaderLabels({"Word group", "Probability"});
    wattr_prob_model->setHorizontalHeaderLabels({"Word attribute", "Probability"});
    ui->sylptnprobTbl->setItemDelegateForColumn(1, new TBVSpinBoxDelegate(ui->sylptnprobTbl));
    ui->ltrprobTbl->setItemDelegateForColumn(1, new TBVSpinBoxDelegate(ui->ltrprobTbl));
    ui->wgrpprobTbl->setItemDelegateForColumn(1, new TBVSpinBoxDelegate(ui->wgrpprobTbl));
    ui->wattrprobTbl->setItemDelegateForColumn(1, new TBVSpinBoxDelegate(ui->wattrprobTbl));
    Generator gen = mainwin->context.generator;
    for (unsigned int i=0; i<gen.syllable_patterns.size(); i++){
        QList<QStandardItem*> sylptn_row;
        std::string sylptn_str = LetterGroup::SPStringFromSyllablePattern(gen.syllable_patterns[i]);
        sylptn_row << new QStandardItem(QString::fromStdString(sylptn_str));
        //
        sylptn_prob_model->appendRow(sylptn_row);
    }
    for (unsigned int i=0; i<gen.letter_groups.size(); i++){
        LetterGroup grp = gen.letter_groups[i];
        for (unsigned int j=0; j<grp.letters.size(); j++){
            QList<QStandardItem*> ltr_row;
            ltr_row << new QStandardItem(QString::fromStdString(grp.letters[j].character));
            //
            ltr_prob_model->appendRow(ltr_row);
        }
    }
    for (unsigned int i=0; i<gen.word_groups.size(); i++){
        WordGroup grp = gen.word_groups[i];
        QList<QStandardItem*> wgrp_row;
        wgrp_row << new QStandardItem(QString::fromStdString(grp.group_identifier));
        //
        wgrp_prob_model->appendRow(wgrp_row);
        for (unsigned int j=0; j<grp.possible_attributes.size(); j++){
            WordAttribute attr = grp.possible_attributes[j];
            bool alreadyAdded=0;
            for (unsigned int z=0; z<wattr_prob_model->rowCount(); z++){
                std::string row_txt = wattr_prob_model->data(wattr_prob_model->index(z, 0)).toString().toStdString();
                if (row_txt == attr.attribute_identifier) alreadyAdded=1;
            }
            if (!alreadyAdded){
                QList<QStandardItem*> wattr_row;
                wattr_row << new QStandardItem(QString::fromStdString(attr.attribute_identifier));
                //
                wattr_prob_model->appendRow(wattr_row);
            }
        }
    }
}

LangSettingsWindow::LangSettingsWindow(MainWindow* main, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LangSettingsWindow)
{
    mainwin = main;
    ui->setupUi(this);
    SetupCustomUi();
    LoadContextSettings();
}

LangSettingsWindow::~LangSettingsWindow()
{
    delete ui;
}

SETTINGS_ERROR LangSettingsWindow::ApplySettings(){
    SETTINGS_ERROR err = SETTINGS_ERROR_NOERR;
    std::string LGString = ui->ltrgrpTxtEdit->toPlainText().toStdString();
    std::string SPString = ui->sylptnTxtEdit->toPlainText().toStdString();
    std::vector<LetterGroup> pending_lettergroups = LetterGroup::LetterGroupsFromLGString(LGString);
    std::vector<LetterGroup> old_lettergroups = mainwin->context.generator.letter_groups;
    mainwin->context.generator.letter_groups = pending_lettergroups;
    std::vector<LetterGroup> *grps = &mainwin->context.generator.letter_groups;
    std::vector<SyllablePattern> pending_syllablepatterns;
    if (LetterGroup::SyllablePatternsFromSPString(SPString, grps, &pending_syllablepatterns)){
        err = SETTINGS_ERROR_SYLLABLE_PATTERN;
    }
    std::vector<WordAttribute> pending_wattrs;
    for (unsigned int i=0; i<pending_attribarg_fields.size(); i++){
        WordAttribute attr;
        attr.attribute_identifier = wattr_model->index(i, 0).data().toString().toStdString();
        WATTR_PRESET_FUNCTION func_preset = WordAttributeFunctionPreset::GetWattrPresetEnum(wattr_model->index(i, 1).data().toString().toStdString());
        attr.attribute_func_preset = WordAttributeFunctionPreset::WordAttributeFunctionPresets[func_preset];
        if (attr.attribute_func_preset.func_preset == ENUM_WATTR_PRESET_ADD_PREFIX){
            std::vector<Syllable> arg1;
            if (Syllable::SyllablesFromArgString(pending_attribarg_fields[i][0], &mainwin->context.generator.letter_groups, &arg1)){
                err = SETTINGS_ERROR_ATTRIBUTE_ARGUMENT;
            }
            attr.attribute_func_arguments.push_back(arg1);
            attr.attribute_function = [arg1](Word *word) {WordAttributeFunctionPreset::WATTR_PRESET_ADD_PREFIX(arg1, word);};
        }
        else if (attr.attribute_func_preset.func_preset == ENUM_WATTR_PRESET_ADD_SUFFIX){
            std::vector<Syllable> arg1;
            if (Syllable::SyllablesFromArgString(pending_attribarg_fields[i][0], &mainwin->context.generator.letter_groups, &arg1)){
                err = SETTINGS_ERROR_ATTRIBUTE_ARGUMENT;
            }
            attr.attribute_func_arguments.push_back(arg1);
            attr.attribute_function = [arg1](Word *word) {WordAttributeFunctionPreset::WATTR_PRESET_ADD_SUFFIX(arg1, word);};
        }
        else if (attr.attribute_func_preset.func_preset == ENUM_WATTR_PRESET_DONOTHING){
            attr.attribute_func_arguments = {};
            attr.attribute_function = WordAttributeFunctionPreset::WATTR_PRESET_DONOTHING;
        }
        else {
            break;
        }
        pending_wattrs.push_back(attr);
    }
    std::vector<WordGroup> pending_wordgroups;
    QStandardItemModel *mod = qobject_cast<QStandardItemModel*>(ui->wgrpTblView->model());
    for (unsigned int i=0; i<mod->rowCount(); i++){
        WordGroup grp;
        grp.group_identifier = mod->data(mod->index(i, 0)).toString().toStdString();
        if (WordAttribute::GetWordAttributesFromStr(mod->data(mod->index(i, 1)).toString().toStdString(), pending_wattrs, &grp.possible_attributes)){
            err = SETTINGS_ERROR_WORDGROUP_ATTRIBUTE_UNKNOWN;
        }
        if (grp.possible_attributes.size() == 0){
            err = SETTINGS_ERROR_NO_WORDGROUP_ATTRIBUTE;
        }
        pending_wordgroups.push_back(grp);
    }
    // apply if no err, when applying, need to reload all the config
    if (err == SETTINGS_ERROR_NOERR){
        mainwin->context.generator.syllable_patterns = pending_syllablepatterns;
        mainwin->context.generator.word_groups = pending_wordgroups;
        LoadContextSettings();
    }
    else{
        mainwin->context.generator.letter_groups = old_lettergroups;
    }
    return err;
}
void LangSettingsWindow::on_applyBtn_clicked()
{
    ui->errLabel->setVisible(false);
    SETTINGS_ERROR err = ApplySettings();
    if (err != SETTINGS_ERROR_NOERR){
        std::string errtxt = "Invalid Settings : " + GetSettingsErrorMessage(err);
        ui->errLabel->setText(QString::fromStdString(errtxt));
        ui->errLabel->setVisible(true);
    }

}

void LangSettingsWindow::on_quitBtn_clicked()
{
    close();
}


void LangSettingsWindow::on_addwgrpBtn_clicked()
{
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::fromStdString("GroupName"));
    row << new QStandardItem(QString::fromStdString("GroupAttributes"));
    wgrp_model->appendRow(row);
}


void LangSettingsWindow::on_delwgrpBtn_clicked()
{
    QModelIndexList selection = ui->wgrpTblView->selectionModel()->selectedIndexes();
    for (QModelIndex item : selection){
        wgrp_model->removeRow(item.row());
    }
}


void LangSettingsWindow::on_addwattrBtn_clicked()
{
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::fromStdString("AttributeName"));
    row << new QStandardItem(QString::fromStdString("Do nothing"));
    int row_index = wattr_model->rowCount();
    wattr_model->appendRow(row);
    wattr_tbv->showButtonInCell(row_index);
    std::vector<std::string> v;
    pending_attribarg_fields.push_back(v);
}


void LangSettingsWindow::on_delwattrBtn_clicked()
{
    QModelIndexList selection = wattr_tbv->selectionModel()->selectedIndexes();
    for (QModelIndex item : selection){
        wattr_model->removeRow(item.row());
        if (item.row() < pending_attribarg_fields.size()){
            pending_attribarg_fields.erase(pending_attribarg_fields.begin() + item.row());
        }
    }
}
