#include "include/qt_mainwindow.h"
#include "include/qt_langsettingswindow.h"
#include "ui_mainwindow.h"


void MainWindow::SetupContext(){
  Letter letter1, letter2, letter3, letter4, letter5, letter6;
  letter1.character = "g";
  letter2.character = "a";
  letter3.character = "r";
  letter4.character = "p";
  letter5.character = "i";
  letter6.character = "t";
  LetterGroup lgroup1, lgroup2;
  lgroup1.group_identifier = "C";
  lgroup2.group_identifier = "V";
  lgroup1.letters.push_back(letter1);
  lgroup1.letters.push_back(letter3);
  lgroup1.letters.push_back(letter4);
  lgroup1.letters.push_back(letter6);
  lgroup2.letters.push_back(letter2);
  lgroup2.letters.push_back(letter5);
  context.generator.letter_groups.push_back(lgroup1);
  context.generator.letter_groups.push_back(lgroup2);
  SyllablePattern pat1, pat2, pat3;
  /*pat1.push_back(&context.generator.letter_groups[0]);
  pat1.push_back(&context.generator.letter_groups[1]);
  pat1.push_back(&context.generator.letter_groups[1]);
  pat1.push_back(&context.generator.letter_groups[0]);
  pat2.push_back(&context.generator.letter_groups[0]);
  pat2.push_back(&context.generator.letter_groups[1]);
  pat2.push_back(&context.generator.letter_groups[0]);
  pat3.push_back(&context.generator.letter_groups[0]);
  pat3.push_back(&context.generator.letter_groups[1]);
  context.generator.syllable_patterns.push_back(pat1);
  context.generator.syllable_patterns.push_back(pat2);
  context.generator.syllable_patterns.push_back(pat3);
  Syllable wattr3syl, wattr4syl;
  std::vector<Syllable> wattr3suf, wattr4pref;
  wattr3syl.letters.push_back(&context.generator.letter_groups[0].letters[1]);
  wattr3syl.letters.push_back(&context.generator.letter_groups[1].letters[1]);
  wattr3syl.pattern.push_back(&context.generator.letter_groups[0]);
  wattr3syl.pattern.push_back(&context.generator.letter_groups[1]);
  wattr3suf = {wattr3syl};
  wattr4syl.letters.push_back(&context.generator.letter_groups[0].letters[0]);
  wattr4syl.letters.push_back(&context.generator.letter_groups[1].letters[0]);
  wattr4syl.pattern.push_back(&context.generator.letter_groups[0]);
  wattr4syl.pattern.push_back(&context.generator.letter_groups[1]);
  wattr4pref = {wattr4syl, wattr4syl};
  WordAttribute wattr1, wattr2, wattr3, wattr4;
  wattr1.attribute_identifier = "masculine";
  wattr2.attribute_identifier = "feminine";
  wattr3.attribute_identifier = "godan";
  wattr4.attribute_identifier = "ichidan";
  wattr1.attribute_function = WordAttributeFunctionPreset::WATTR_PRESET_DONOTHING;
  wattr1.attribute_func_preset = WordAttributeFunctionPreset::WordAttributeFunctionPresets[ENUM_WATTR_PRESET_DONOTHING];
  wattr2.attribute_function = WordAttributeFunctionPreset::WATTR_PRESET_DONOTHING;
  wattr2.attribute_func_preset = WordAttributeFunctionPreset::WordAttributeFunctionPresets[ENUM_WATTR_PRESET_DONOTHING];
  wattr3.attribute_function = [wattr3suf](Word *word) {WordAttributeFunctionPreset::WATTR_PRESET_ADD_SUFFIX(wattr3suf, word);};
  wattr3.attribute_func_arguments.push_back(wattr3suf);
  wattr3.attribute_func_preset = WordAttributeFunctionPreset::WordAttributeFunctionPresets[ENUM_WATTR_PRESET_ADD_SUFFIX];
  wattr4.attribute_function = [wattr4pref](Word *word) {WordAttributeFunctionPreset::WATTR_PRESET_ADD_PREFIX(wattr4pref, word);};
  wattr4.attribute_func_arguments.push_back(wattr4pref);
  wattr4.attribute_func_preset = WordAttributeFunctionPreset::WordAttributeFunctionPresets[ENUM_WATTR_PRESET_ADD_PREFIX];
  WordGroup wgroup1, wgroup2;
  wgroup1.group_identifier = "Noun";
  wgroup2.group_identifier = "Verb";
  wgroup1.possible_attributes.push_back(wattr1);
  wgroup1.possible_attributes.push_back(wattr2);
  wgroup2.possible_attributes.push_back(wattr3);
  wgroup2.possible_attributes.push_back(wattr4);
  context.generator.word_groups.push_back(wgroup1);
  context.generator.word_groups.push_back(wgroup2);
  context.generator.min_syllable_count = 2;
  context.generator.max_syllable_count = 6;
  context.generator.nb_words = 10;
*/
}

SETTINGS_ERROR MainWindow::ApplySettings(){
    unsigned int minNbSyl = ui->minNbSSpin->value();
    unsigned int maxNbSyl = ui->maxNbSSpin->value();
    unsigned int nbWords = ui->nbWSpin->value();
    if (minNbSyl > maxNbSyl){
        return SETTINGS_ERROR_SYLLABLECOUNT_MAX_MIN;
    }
    if (context.generator.letter_groups.size() == 0){
        return SETTINGS_ERROR_NO_LETTERGROUP;
    }
    if (context.generator.syllable_patterns.size() == 0){
        return SETTINGS_ERROR_NO_SYLLABLEPATTERN;
    }
    if (context.generator.word_groups.size() == 0){
        return SETTINGS_ERROR_NO_WORDGROUP;
    }
    context.generator.min_syllable_count = minNbSyl;
    context.generator.max_syllable_count = maxNbSyl;
    context.generator.nb_words = nbWords;
    return SETTINGS_ERROR_NOERR;
}

void MainWindow::SetupCustomUi(){
    model = new QStandardItemModel(0, 2, 0);
    model->setHorizontalHeaderLabels({"Word", "Group(s)"});
    ui->outpuTblView->setModel(model);
    ui->errLabel->setVisible(false);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    SetupCustomUi();
    //SetupContext();
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_generateBtn_clicked()
{
    ui->errLabel->setVisible(false);
    SETTINGS_ERROR err = ApplySettings();
    if (err != SETTINGS_ERROR_NOERR){
        ui->errLabel->setVisible(true);
        ui->errLabel->setText(QString::fromStdString("Invalid settings : " + GetSettingsErrorMessage(err)));
        return;
    }
    std::vector<Word> generated_words = context.generator.GenerateWords();
    for (Word word : generated_words){
        std::string group_string = word.group->group_identifier + " (";
        for (unsigned int i=0; i<word.attributes.size(); i++){
            group_string += word.attributes[i]->attribute_identifier;
            if (i!=word.attributes.size()-1){
                group_string += ", ";
            }
        }
        group_string += ")";
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(word.GetString()));
        row << new QStandardItem(QString::fromStdString(group_string));
        model->appendRow(row);
    }
}


void MainWindow::on_clearOutBtn_clicked()
{
    model->clear();
    model->setHorizontalHeaderLabels({"Word", "Group(s)"});
}


void MainWindow::on_langSetngsBtn_clicked()
{
    LangSettingsWindow *win = new LangSettingsWindow(this, this);
    win->exec();
}

