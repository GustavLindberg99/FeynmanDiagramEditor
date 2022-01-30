#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QSvgRenderer>
#include <QPdfWriter>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QScreen>
#include <QDesktopServices>
#include "windowwithclosesignal.hpp"
#include "diagramviewer.h"
#include "version.h"

int main(int argc, char **argv){
    QApplication app(argc, argv);
    app.addLibraryPath("./");    //Otherwise weird things happen, see https://stackoverflow.com/a/25266269/4284627

    MainWindow *mainWindow = new MainWindow;
    mainWindow->setWindowTitle(QObject::tr("New document") + " - FeynmanDiagramEditor");
    mainWindow->setWindowIcon(QIcon(":/icon.ico"));

    DiagramViewer *diagramViewer = new DiagramViewer;
    diagramViewer->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    mainWindow->setCentralWidget(diagramViewer);

    QMenuBar menuBar;
    QMenu *fileMenu = menuBar.addMenu(QObject::tr("&File"));
    QAction *newAction = fileMenu->addAction(QIcon(":/icons/new.svg"), QObject::tr("&New Document"));
    QAction *openAction = fileMenu->addAction(QIcon(":/icons/open.svg"), QObject::tr("&Open..."));
    fileMenu->addSeparator();
    QAction *saveAction = fileMenu->addAction(QIcon(":/icons/save.svg"), QObject::tr("&Save"));
    QAction *saveAsAction = fileMenu->addAction(QObject::tr("Save &As..."));
    QAction *exportAction = fileMenu->addAction(QIcon(":/icons/export.svg"), QObject::tr("&Export..."));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(QObject::tr("&Quit"));

    newAction->setShortcut(QKeySequence("CTRL+N"));
    openAction->setShortcut(QKeySequence("CTRL+O"));
    saveAction->setShortcut(QKeySequence("CTRL+S"));
    saveAsAction->setShortcut(QKeySequence("CTRL+SHIFT+S"));
    exportAction->setShortcut(QKeySequence("CTRL+E"));
    quitAction->setShortcut(QKeySequence("CTRL+Q"));

    QString currentFile;
    QObject::connect(newAction, &QAction::triggered, [mainWindow, diagramViewer, &currentFile, saveAction](){
        if(mainWindow->windowTitle().startsWith("*")){
            switch(QMessageBox::warning(mainWindow, "", QObject::tr("Do you want to save before quitting?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)){
            case QMessageBox::Yes:
                saveAction->trigger();
                break;
            case QMessageBox::Cancel:
                return;
            }
        }
        diagramViewer->clear();
        currentFile.clear();
        mainWindow->setWindowTitle(QObject::tr("New document") + " - FeynmanDiagramEditor");
    });
    QObject::connect(openAction, &QAction::triggered, [mainWindow, diagramViewer, &currentFile, saveAction](){
        if(mainWindow->windowTitle().startsWith("*")){
            switch(QMessageBox::warning(mainWindow, "", QObject::tr("Do you want to save before quitting?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)){
            case QMessageBox::Yes:
                saveAction->trigger();
                break;
            case QMessageBox::Cancel:
                return;
            }
        }
        const QString chosenFile = QFileDialog::getOpenFileName(diagramViewer, QObject::tr("Open..."), "", QObject::tr("Feynman diagrams") + " (*.fdg)");
        if(!chosenFile.isEmpty()){
            diagramViewer->clear();
            QFile file(chosenFile);
            if(!file.open(QFile::ReadOnly)){
                QMessageBox::critical(diagramViewer, "", QObject::tr("Could not open the file %1. You might not have sufficient permissions to read at this location.").arg(chosenFile));
                return;
            }
            QDataStream dataStream(&file);
            dataStream >> diagramViewer;
            if(dataStream.status() == QDataStream::Ok){
                currentFile = chosenFile;
                mainWindow->setWindowTitle(currentFile + " - FeynmanDiagramEditor");
            }
            else{
                currentFile.clear();
                diagramViewer->clear();
                QMessageBox::critical(diagramViewer, "", QObject::tr("The file %1 is not a valid Feynman diagram file.").arg(chosenFile));
            }
        }
    });
    QObject::connect(saveAction, &QAction::triggered, [mainWindow, diagramViewer, &currentFile, saveAsAction](){
        if(currentFile.isEmpty()){
            saveAsAction->trigger();
        }
        else{
            QFile file(currentFile);
            if(file.open(QFile::WriteOnly)){
                QDataStream dataStream(&file);
                dataStream << diagramViewer;
                if(dataStream.status() == QDataStream::Ok){
                    mainWindow->setWindowTitle(currentFile + " - FeynmanDiagramEditor");
                    return;
                }
            }
            QMessageBox::critical(diagramViewer, "", QObject::tr("Could not save the file %1. You might not have sufficient permissions to write at this location.").arg(currentFile));
            saveAsAction->trigger();
        }
    });
    QObject::connect(saveAsAction, &QAction::triggered, [mainWindow, diagramViewer, &currentFile](){
        const QString chosenFile = QFileDialog::getSaveFileName(mainWindow, QObject::tr("Save as..."), "", QObject::tr("Feynman diagram") + " (*.fdg)");
        if(!chosenFile.isEmpty()){
            QFile file(chosenFile);
            if(file.open(QFile::WriteOnly)){
                QDataStream dataStream(&file);
                dataStream << diagramViewer;
                if(dataStream.status() == QDataStream::Ok){
                    currentFile = chosenFile;
                    mainWindow->setWindowTitle(currentFile + " - FeynmanDiagramEditor");
                }
                return;
            }
            QMessageBox::critical(diagramViewer, "", QObject::tr("Could not save the file %1. You might not have sufficient permissions to write at this location.").arg(chosenFile));
        }
    });
    QObject::connect(exportAction, &QAction::triggered, [diagramViewer](){
        const QString &svgCode = diagramViewer->toSvg();
        if(svgCode.isEmpty()){
            QMessageBox::critical(diagramViewer, "", QObject::tr("This diagram is empty. Please draw something before exporting."));
            return;
        }
        QString chosenFormat;
        const QString chosenFile = QFileDialog::getSaveFileName(diagramViewer, QObject::tr("Export..."), "", QObject::tr("SVG image") + " (*.svg);;" + QObject::tr("PNG image") + " (*.png);;" + QObject::tr("PDF document") + " (*.pdf)", &chosenFormat);
        if(!chosenFile.isEmpty()){
            QTemporaryDir temp;
            QFile svgFile(chosenFormat.endsWith("(*.svg)") ? chosenFile : temp.path() + "/export.svg");
            if(!svgFile.open(QFile::WriteOnly | QFile::Text) || svgFile.write(svgCode.toUtf8()) == -1){
                QMessageBox::critical(diagramViewer, "", QObject::tr("Could not save the file %1. You might not have sufficient permissions to write at this location.").arg(chosenFile));
            }
            else if(!chosenFormat.endsWith("(*.svg)")){
                svgFile.close();
                const int width = QRegularExpression(" width=\"([0-9]+)\"").match(svgCode).captured(1).toInt();
                const int height = QRegularExpression(" height=\"([0-9]+)\"").match(svgCode).captured(1).toInt();
                QSvgRenderer renderer(svgFile.fileName());
                if(chosenFormat.endsWith("(*.png)")){
                    QImage image(width, height, QImage::Format_ARGB32);
                    image.fill(Qt::white);
                    QPainter painter(&image);
                    renderer.render(&painter);
                    if(!image.save(chosenFile, "png")){
                        QMessageBox::critical(diagramViewer, "", QObject::tr("Could not save the file %1. You might not have sufficient permissions to write at this location.").arg(chosenFile));
                    }
                }
                else if(chosenFormat.endsWith("(*.pdf)")){
                    QPdfWriter pdfWriter(chosenFile);
                    pdfWriter.setPageSize(QPageSize(QSize(width, height) * 72 / QGuiApplication::primaryScreen()->physicalDotsPerInch()));
                    QPainter painter(&pdfWriter);
                    renderer.render(&painter);
                }
            }
        }
    });
    QObject::connect(quitAction, &QAction::triggered, [mainWindow, saveAction, &app](){
        if(mainWindow->windowTitle().startsWith("*")){
            switch(QMessageBox::warning(mainWindow, "", QObject::tr("Do you want to save before quitting?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)){
            case QMessageBox::Yes:
                saveAction->trigger();
                //No break
            case QMessageBox::No:
                app.quit();
                break;
            }
        }
        else{
            app.quit();
        }
    });

    QMenu *viewMenu = menuBar.addMenu(QObject::tr("&View"));
    QMenu *toolbarMenu = viewMenu->addMenu(QObject::tr("&Toolbars"));
    QAction *toggleFileToolbar = toolbarMenu->addAction(QObject::tr("&File"));
    QAction *toggleDrawToolbar = toolbarMenu->addAction(QObject::tr("&Draw particles"));
    QAction *toggleParticleToolbar = toolbarMenu->addAction(QObject::tr("&Manage particles"));

    QAction *gridAction = viewMenu->addAction(QObject::tr("Show &grid"));
    gridAction->setCheckable(true);
    gridAction->setChecked(true);
    QObject::connect(gridAction, &QAction::triggered, diagramViewer, &DiagramViewer::setGridVisibiliy);

    QMenu *helpMenu = menuBar.addMenu(QObject::tr("&Help"));
    QAction *helpAction = helpMenu->addAction(QObject::tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(QObject::tr("&About FeynmanDiagramEditor"));
    QAction *aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));

    helpAction->setShortcut(QKeySequence("F1"));
    QObject::connect(helpAction, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://github.com/Gustav-Lindberg/FeynmanDiagramEditor/blob/main/README.md"));
    });
    QObject::connect(aboutAction, &QAction::triggered, [mainWindow](){
        QMessageBox::about(mainWindow, QObject::tr("About FeynmanDiagramEditor"), "FeynmanDiagramEditor " PROGRAMVERSION "<br/><br/>" + QObject::tr("By Gustav Lindberg") + "<br/><br/>" + QObject::tr("This program is licensed under the GNU GPL 2.0.") + "<br/><br/>" + QObject::tr("Source code:") + " <a href=\"https://github.com/Gustav-Lindberg/FeynmanDiagramEditor\">https://github.com/Gustav-Lindberg/FeynmanDiagramEditor</a><br/><br/>" + QObject::tr("Icons made by %3 and %4 from %1 are licensed by %2.").arg("<a href=\"https://www.iconfinder.com/\">www.iconfinder.com</a>", "<a href=\"http://creativecommons.org/licenses/by/3.0/\">CC 3.0 BY</a>", "<a href=\"https://www.iconfinder.com/paomedia\">Paomedia</a>", "<a href=\"https://www.iconfinder.com/webkul\">Webkul Software</a>"));
    });
    QObject::connect(aboutQtAction, &QAction::triggered, [mainWindow](){
        QMessageBox::aboutQt(mainWindow);
    });

    mainWindow->setMenuBar(&menuBar);

    QToolBar fileToolbar(QObject::tr("&File"));
    toggleFileToolbar->setCheckable(true);
    toggleFileToolbar->setChecked(true);
    QObject::connect(toggleFileToolbar, &QAction::triggered, &fileToolbar, &QToolBar::setVisible);
    QObject::connect(&fileToolbar, &QToolBar::visibilityChanged, toggleFileToolbar, &QAction::setChecked);

    fileToolbar.addAction(newAction);
    fileToolbar.addAction(openAction);
    fileToolbar.addAction(saveAction);
    fileToolbar.addAction(exportAction);
    mainWindow->addToolBar(&fileToolbar);

    QToolBar drawToolbar(QObject::tr("&Draw particles"));
    toggleDrawToolbar->setCheckable(true);
    toggleDrawToolbar->setChecked(true);
    QObject::connect(toggleDrawToolbar, &QAction::triggered, &drawToolbar, &QToolBar::setVisible);
    QObject::connect(&drawToolbar, &QToolBar::visibilityChanged, toggleDrawToolbar, &QAction::setChecked);

    QAction *addFermion = drawToolbar.addAction(QIcon(":/icons/fermion.svg"), QObject::tr("Fermion"));
    QAction *addPhoton = drawToolbar.addAction(QIcon(":/icons/photon.svg"), QObject::tr("Photon"));
    QAction *addWeakBoson = drawToolbar.addAction(QIcon(":/icons/weakboson.svg"), QObject::tr("Weak Boson"));
    QAction *addGluon = drawToolbar.addAction(QIcon(":/icons/gluon.svg"), QObject::tr("Gluon"));
    QAction *addHiggs = drawToolbar.addAction(QIcon(":/icons/higgs.svg"), QObject::tr("Higgs Boson"));
    drawToolbar.addSeparator();
    QAction *addHadron = drawToolbar.addAction(QIcon(":/icons/hadron.svg"), QObject::tr("Group Quarks into Hadrons"));
    QAction *addVertex = drawToolbar.addAction(QIcon(":/icons/vertex.svg"), QObject::tr("Add Label to Vertex"));
    addFermion->setCheckable(true);
    addPhoton->setCheckable(true);
    addWeakBoson->setCheckable(true);
    addGluon->setCheckable(true);
    addHiggs->setCheckable(true);
    addHadron->setCheckable(true);
    addVertex->setCheckable(true);

    QObject::connect(addFermion, &QAction::triggered, [addFermion, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addFermion->setChecked(true);
            diagramViewer->startDrawing(Particle::Fermion);
        }
    });
    QObject::connect(addPhoton, &QAction::triggered, [addPhoton, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addPhoton->setChecked(true);
            diagramViewer->startDrawing(Particle::Photon);
        }
    });
    QObject::connect(addWeakBoson, &QAction::triggered, [addWeakBoson, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addWeakBoson->setChecked(true);
            diagramViewer->startDrawing(Particle::WeakBoson);
        }
    });
    QObject::connect(addGluon, &QAction::triggered, [addGluon, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addGluon->setChecked(true);
            diagramViewer->startDrawing(Particle::Gluon);
        }
    });
    QObject::connect(addHiggs, &QAction::triggered, [addHiggs, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addHiggs->setChecked(true);
            diagramViewer->startDrawing(Particle::Higgs);
        }
    });
    QObject::connect(addHadron, &QAction::triggered, [addHadron, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addHadron->setChecked(true);
            diagramViewer->startDrawing(Particle::Hadron);
        }
    });
    QObject::connect(addVertex, &QAction::triggered, [addVertex, diagramViewer](bool checked){
        diagramViewer->stopDrawing();
        diagramViewer->deselect();
        if(checked){
            addVertex->setChecked(true);
            diagramViewer->startDrawing(Particle::Vertex);
        }
    });
    QObject::connect(diagramViewer, &DiagramViewer::drawingStopped, [mainWindow, addFermion, addPhoton, addWeakBoson, addGluon, addHiggs, addHadron, addVertex](){
        if(!mainWindow->windowTitle().startsWith("*")){
            mainWindow->setWindowTitle("*" + mainWindow->windowTitle());
        }
        addFermion->setChecked(false);
        addPhoton->setChecked(false);
        addWeakBoson->setChecked(false);
        addGluon->setChecked(false);
        addHiggs->setChecked(false);
        addHadron->setChecked(false);
        addVertex->setChecked(false);
    });
    mainWindow->addToolBar(&drawToolbar);

    QToolBar particleToolbar(QObject::tr("&Manage particles"));
    toggleParticleToolbar->setCheckable(true);
    toggleParticleToolbar->setChecked(true);
    QObject::connect(toggleParticleToolbar, &QAction::triggered, &particleToolbar, &QToolBar::setVisible);
    QObject::connect(&particleToolbar, &QToolBar::visibilityChanged, toggleParticleToolbar, &QAction::setChecked);

    particleToolbar.addWidget(new QLabel(QObject::tr("Label") + ": "));
    QLineEdit *labelEditor = new QLineEdit;
    labelEditor->setEnabled(false);
    labelEditor->setMaximumWidth(200);
    particleToolbar.addWidget(labelEditor);
    particleToolbar.addSeparator();
    QAction *deleteAction = particleToolbar.addAction(QIcon(":/icons/delete.svg"), QObject::tr("Delete selected particle"));
    deleteAction->setEnabled(false);
    deleteAction->setShortcut(QKeySequence("Del"));

    QObject::connect(diagramViewer, &DiagramViewer::particleSelected, [labelEditor, deleteAction](const Particle &particle){
        labelEditor->setEnabled(true);
        deleteAction->setEnabled(true);
        labelEditor->setText(particle.labelText());
    });
    QObject::connect(diagramViewer, &DiagramViewer::particleDeselected, [labelEditor, deleteAction](){
        labelEditor->clear();
        labelEditor->setEnabled(false);
        deleteAction->setEnabled(false);
    });
    QObject::connect(labelEditor, &QLineEdit::textEdited, [mainWindow, diagramViewer](const QString &text){
        if(!mainWindow->windowTitle().startsWith("*")){
            mainWindow->setWindowTitle("*" + mainWindow->windowTitle());
        }
        diagramViewer->editSelectedLabel(text);
    });
    QObject::connect(deleteAction, &QAction::triggered, diagramViewer, [mainWindow, diagramViewer](){
        if(!mainWindow->windowTitle().startsWith("*")){
            mainWindow->setWindowTitle("*" + mainWindow->windowTitle());
        }
        diagramViewer->deleteSelectedParticle();
    });
    mainWindow->addToolBar(&particleToolbar);

    QObject::connect(mainWindow, &MainWindow::aboutToClose, [mainWindow, saveAction](QCloseEvent *event){
        if(mainWindow->windowTitle().startsWith("*")){
            switch(QMessageBox::warning(mainWindow, "", QObject::tr("Do you want to save before quitting?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)){
            case QMessageBox::Yes:
                saveAction->trigger();
                //No break
            case QMessageBox::No:
                event->accept();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
            }
        }
    });

    if(argc >= 2){
        QFile file(argv[1]);
        if(!file.open(QFile::ReadOnly)){
            QMessageBox::critical(diagramViewer, "", QObject::tr("Could not open the file %1. You might not have sufficient permissions to read at this location.").arg(argv[1]));
        }
        else{
            QDataStream dataStream(&file);
            dataStream >> diagramViewer;
            if(dataStream.status() == QDataStream::Ok){
                currentFile = argv[1];
                mainWindow->setWindowTitle(currentFile + " - FeynmanDiagramEditor");
            }
            else{
                currentFile.clear();
                diagramViewer->clear();
                QMessageBox::critical(diagramViewer, "", QObject::tr("The file %1 is not a valid Feynman diagram file.").arg(argv[1]));
            }
        }
    }

    mainWindow->showMaximized();

    return app.exec();
}
