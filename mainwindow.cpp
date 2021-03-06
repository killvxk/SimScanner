#include "mainwindow.h"
#include "ui_mainwindow.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ss(ssNotInitialized),

    menu_PE(new QPeFileMenu(this)),
    menu_Dir(new QMenu(this)),
    menu_Other(new QMenu(this)),

    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(tr(":/icons/icons/SimScanner.png")));

    ScanModel *model = new ScanModel;
    ui->treeView->setModel(model);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::tvContextMenuRequested);

    QAction *fileInfoAction = new QAction("File information", this);
    connect(fileInfoAction, &QAction::triggered, this, &MainWindow::peInfoExecute);
    menu_PE->addAction(fileInfoAction);
    menu_PE->addAction(new QAction("View PE", this));

    menu_Dir->addAction(new QAction("Scan immediately", this));
    //menu_Dir->addAction(new QAction("Add", this));

    ui->actionScan->setIcon(QIcon(tr(":/icons/icons/actionStart.png")));
    ui->actionStop->setIcon(QIcon(tr(":/icons/icons/actionStop.png")));
    ui->btnScan->setIcon(QIcon(tr(":/icons/icons/actionStart.png")));
    ui->btnStop->setIcon(QIcon(tr(":/icons/icons/actionStop.png")));

    ui->actionAbout->setIcon(QIcon(tr(":/icons/icons/Info.png")));
    ui->actionHelp_content->setIcon(QIcon(tr(":/icons/icons/Help.png")));
    ui->actionQuit->setIcon(QIcon(tr(":/icons/icons/quit.png")));
    ui->actionNew->setIcon(QIcon(tr(":/icons/icons/actionNew.png")));
    ui->actionEdit->setIcon(QIcon(tr(":/icons/icons/actionNew.png")));
//Temporary
    menu_Other->addAction(new QAction("Action 1", this));
    menu_Other->addAction(new QAction("Action 2", this));
    menu_Other->addAction(new QAction("Action 3", this));

    ssnnProfiles = DbProfile::loadProfiles();

    SS = ScanState::ssNotInitialized;

    ui->treeView->setColumnWidth(0, 200);

    ui->twModes->findChild<QTabBar *>()->hide();
}

void MainWindow::peInfoExecute(bool checked) {
    if(QFileInfo *info = menu_PE->PeFileInfo) {
        FilePropertiesDialog(*info, SsMode::ssmScan, this).exec();
        menu_PE->PeFileInfo = Q_NULLPTR;
    }
}

void MainWindow::setStatus(QString &status) {
    ui->statusBar->showMessage(status, 2000);
    QStringListModel *model = nullptr;
    if(ui->lvResults->model())
        model = dynamic_cast<QStringListModel *>(ui->lvResults->model());
    if(!model) {
        model = new QStringListModel(this);
        ui->lvResults->setModel(model);
    }
    model->insertRow(model->rowCount());
    model->setData(model->index(
                       PRED(
                           model->rowCount(
                               )
                           ), 0
                       ), status
                   );
}

void MainWindow::tvContextMenuRequested(const QPoint &pos) {
    if(sender() == ui->treeView) {
        ScanModel *model = dynamic_cast<ScanModel *>(ui->treeView->model());
        QModelIndex idx = ui->treeView->indexAt(pos);
        if(idx.column())
            //idx = model->index(idx.row(), 0);
            return;
        //QModelIndex idx = model->index(item_index.row(), 0);

        QFileInfo file_info = model->get_file_info(idx);
        if(file_info.isDir()) {
            menu_Dir->popup(ui->treeView->viewport()->mapToGlobal(pos));
        } else {
            try {
                {
                    QPeFile file(file_info);
                }
                menu_PE->PeFileInfo = &file_info;
                menu_PE->popup(ui->treeView->viewport()->mapToGlobal(pos));
            } catch(...) {
                menu_Other->popup(ui->treeView->viewport()->mapToGlobal(pos));
                menu_PE->PeFileInfo = Q_NULLPTR;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete menu_PE;
    delete menu_Dir;
    delete menu_Other;
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox().exec();
}

void MainWindow::on_actionAbout_triggered()
{
    aboutDialog().exec();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_btnScan_clicked()
{
    emit ui->actionScan->triggered();
}

void MainWindow::on_btnStop_clicked()
{
    emit ui->actionStop->triggered();
}

void MainWindow::on_actionNew_triggered()
{
    ScanState ssPrev = SS;
    SS = ScanState::ssEdit;
    addNewDialog dlg;
    if(dlg.exec()) {
        QString &family = dlg.getSampleFamily();
        const int cIdx = dlg.getSampleIndex();

        for(DbProfile::nnType t = DbProfile::nnType::nntFullFileImage;
            t < DbProfile::nnType::nntCount; t = (DbProfile::nnType)SUCC((int)t))
        {
            for(DbProfile::nnKind k = DbProfile::nnKind::nnkFixed;
                k < DbProfile::nnKind::nnkCount;
                k = (DbProfile::nnKind)SUCC((int)k))
            {
                try {
                    ssnnProfiles.push_back(new DbProfile(family, cIdx, t, k));
                } catch(...) {
                    //
                }
            }
        }
    } else {
        //
    }
    SS = ssPrev;
}

SsMode MainWindow::get_ssm() {
    if(ui->actionEdit_mode->isChecked())
        return SsMode::ssmEdit;
    if(ui->actionScan_mode->isChecked())
        return SsMode::ssmScan;
    SS = ScanState::ssFailure;
    return SsMode::ssmFailure;
}

SsMode MainWindow::set_ssm(SsMode _ssm) {
    //TODO: what is it?
    switch(_ssm)
    {
    case ssmScan:
        ui->actionEdit_mode->setChecked(false);
        ui->actionScan_mode->setChecked(true);
        break;
    case ssmEdit:
        ui->actionEdit_mode->setChecked(true);
        ui->actionScan_mode->setChecked(false);
        break;
    default:
        SS = ScanState::ssFailure;
        break;
    }
    return SSmode;
}

MainWindow::ScanState MainWindow::set_ss(ScanState _ss) {
    ss = _ss;
    switch(ss)
    {
    case MainWindow::ScanState::ssNotInitialized:
        ui->actionQuit->setEnabled(true);
        ui->actionAbout->setEnabled(true);
        ui->actionHelp_content->setEnabled(true);

        ui->actionEdit->setEnabled(true);
        ui->actionScan->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionNew->setEnabled(true);
        ui->btnScan->setEnabled(false);
        ui->btnStop->setEnabled(false);
        break;
    case MainWindow::ScanState::ssReady:
        ui->actionQuit->setEnabled(true);
        ui->actionAbout->setEnabled(true);
        ui->actionHelp_content->setEnabled(true);

        ui->actionEdit->setEnabled(true);
        ui->actionScan->setEnabled(true);
        ui->actionStop->setEnabled(false);
        ui->actionNew->setEnabled(true);
        ui->btnScan->setEnabled(true);
        ui->btnStop->setEnabled(false);
        break;
    case MainWindow::ScanState::ssScan:
        ui->actionQuit->setEnabled(true);
        ui->actionAbout->setEnabled(true);
        ui->actionHelp_content->setEnabled(true);

        ui->actionEdit->setEnabled(false);
        ui->actionScan->setEnabled(false);
        ui->actionStop->setEnabled(true);
        ui->actionNew->setEnabled(false);
        ui->btnScan->setEnabled(false);
        ui->btnStop->setEnabled(true);
        break;
    case MainWindow::ScanState::ssEdit:
        ui->actionQuit->setEnabled(false);
        ui->actionAbout->setEnabled(false);
        ui->actionHelp_content->setEnabled(false);

        ui->actionEdit->setEnabled(false);
        ui->actionScan->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionNew->setEnabled(false);
        ui->btnScan->setEnabled(false);
        ui->btnStop->setEnabled(false);
        break;
    case MainWindow::ScanState::ssFailure:
    default:
        ui->actionQuit->setEnabled(true);
        ui->actionAbout->setEnabled(true);
        ui->actionHelp_content->setEnabled(true);

        ui->actionEdit->setEnabled(false);
        ui->actionScan->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionNew->setEnabled(false);
        ui->btnScan->setEnabled(false);
        ui->btnStop->setEnabled(false);
        setStatus(tr("SimScanner failure"));
        break;
    }
    return ss;
}

void MainWindow::on_actionScan_triggered()
{
    SS = ScanState::ssScan;
    QVector<SimScanNN *> nns;
    foreach(DbProfile *db_profile, ssnnProfiles) nns.push_back(db_profile->getNN());
    //TODO: scan ui->treeView for checked elements; try checked directories and PE
}

void MainWindow::on_actionStop_triggered()
{
    SS = ScanState::ssReady;
}

void MainWindow::on_actionScan_mode_triggered(bool checked)
{
    if(checked) {
        ui->actionEdit_mode->setChecked(false);
        //TODO: set enabled with FS conditions
        ui->actionScan->setVisible(true);
        ui->actionScan->setEnabled(true);
        ui->actionStop->setVisible(true);
        ui->actionStop->setEnabled(false);

        ui->btnScan->setVisible(true);
        ui->btnScan->setEnabled(true);
        ui->btnStop->setVisible(true);
        ui->btnStop->setEnabled(false);

        ui->actionNew->setVisible(false);
        ui->actionNew->setEnabled(false);
        ui->actionEdit->setVisible(false);
        ui->actionEdit->setEnabled(false);

        ui->twModes->setCurrentIndex(0);

        setStatus(tr("SimScanner switched to scan mode"));
    } else {
        ui->actionScan_mode->setChecked(true);
    }
}

void MainWindow::on_actionEdit_mode_triggered(bool checked)
{
    if(checked) {
        ui->actionScan_mode->setChecked(false);

        ui->actionNew->setVisible(true);
        ui->actionNew->setEnabled(true);
        ui->actionEdit->setVisible(true);
        ui->actionEdit->setEnabled(true);

        ui->actionScan->setVisible(false);
        ui->actionScan->setEnabled(false);
        ui->actionStop->setVisible(false);
        ui->actionStop->setEnabled(false);

        ui->btnScan->setVisible(true);
        ui->btnScan->setEnabled(false);
        ui->btnStop->setVisible(true);
        ui->btnStop->setEnabled(false);

        ui->twModes->setCurrentIndex(1);

        setStatus(tr("SimScanner switched to learn mode"));
    } else {
        ui->actionEdit_mode->setChecked(true);
    }
}

