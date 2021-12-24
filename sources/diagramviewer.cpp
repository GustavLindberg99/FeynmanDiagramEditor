#include <QMouseEvent>
#include <QGraphicsPathItem>
#include "diagramviewer.h"

const int DiagramViewer::viewSize = 2000;
const int DiagramViewer::interval = 100;

const int DiagramViewer::selectionSize = 3;
const QColor DiagramViewer::selectionColor(80, 131, 193);

DiagramViewer::DiagramViewer():
    QGraphicsView(new QGraphicsScene),
    _isDrawing(false),
    _currentParticle(nullptr),
    _currentPath(nullptr),
    _selectedPath(nullptr)
{
    this->setGridVisibiliy(true);
}

DiagramViewer::~DiagramViewer(){
    if(this->_currentParticle != nullptr){
        delete this->_currentParticle;
    }
}

void DiagramViewer::startDrawing(Particle::ParticleType particleType){
    this->_isDrawing = true;
    this->_currentParticleType = particleType;
}

void DiagramViewer::stopDrawing(){
    this->_isDrawing = false;
    emit this->drawingStopped();
}

void DiagramViewer::deselect(){
    if(this->_selectedPath != nullptr){
        redrawPath(this->_selectedPath);
        emit this->particleDeselected();
    }
    this->_selectedPath = nullptr;
}

void DiagramViewer::clear(){
    this->stopDrawing();
    this->deselect();

    const QList<QGraphicsPathItem*> paths = this->_fermions.keys() + this->_photons.keys() + this->_weakBosons.keys() + this->_gluons.keys() + this->_higgsBosons.keys();
    for(QGraphicsPathItem *path: paths){
        this->scene()->removeItem(path);
        delete path;
    }
    this->_fermions.clear();
    this->_photons.clear();
    this->_weakBosons.clear();
    this->_gluons.clear();
    this->_higgsBosons.clear();
}

QDataStream &operator<<(QDataStream &dataStream, const DiagramViewer *diagramViewer){
    dataStream << diagramViewer->_fermions.values() << diagramViewer->_photons.values() << diagramViewer->_weakBosons.values() << diagramViewer->_gluons.values() << diagramViewer->_higgsBosons.values();
    return dataStream;
}

QDataStream &operator>>(QDataStream &dataStream, DiagramViewer *diagramViewer){
    QList<Fermion> fermions;
    QList<Photon> photons;
    QList<WeakBoson> weakBosons;
    QList<Gluon> gluons;
    QList<Higgs> higgsBosons;
    dataStream >> fermions >> photons >> weakBosons >> gluons >> higgsBosons;
    for(const Fermion &fermion: qAsConst(fermions)){
        QGraphicsPathItem *path = diagramViewer->scene()->addPath(fermion.painterPath(), Qt::NoPen, QBrush(Qt::black));
        diagramViewer->_fermions.insert(path, fermion);
    }
    for(const Photon &photon: qAsConst(photons)){
        QGraphicsPathItem *path = diagramViewer->scene()->addPath(photon.painterPath(), Qt::NoPen, QBrush(Qt::black));
        diagramViewer->_photons.insert(path, photon);
    }
    for(const WeakBoson &weakBoson: qAsConst(weakBosons)){
        QGraphicsPathItem *path = diagramViewer->scene()->addPath(weakBoson.painterPath(), Qt::NoPen, QBrush(Qt::black));
        diagramViewer->_weakBosons.insert(path, weakBoson);
    }
    for(const Gluon &gluon: qAsConst(gluons)){
        QGraphicsPathItem *path = diagramViewer->scene()->addPath(gluon.painterPath(), Qt::NoPen, QBrush(Qt::black));
        diagramViewer->_gluons.insert(path, gluon);
    }
    for(const Higgs &higgs: qAsConst(higgsBosons)){
        QGraphicsPathItem *path = diagramViewer->scene()->addPath(higgs.painterPath(), Qt::NoPen, QBrush(Qt::black));
        diagramViewer->_higgsBosons.insert(path, higgs);
    }
    return dataStream;
}

QString DiagramViewer::toSvg() const{
    int x1 = viewSize, y1 = viewSize, x2 = 0, y2 = 0;
    const auto adjustDimensions = [&](const Particle &particle){
        const QRect rect = particle.painterPath().boundingRect().toRect();
        x1 = qMin(x1, rect.x());
        y1 = qMin(y1, rect.y());
        x2 = qMax(x2, rect.x() + rect.width());
        y2 = qMax(y2, rect.y() + rect.height());
    };
    QString svgCode;
    for(const Fermion &fermion: this->_fermions){
        svgCode += fermion.svgCode();
        adjustDimensions(fermion);
    }
    for(const Photon &photon: this->_photons){
        svgCode += photon.svgCode();
        adjustDimensions(photon);
    }
    for(const WeakBoson &weakBoson: this->_weakBosons){
        svgCode += weakBoson.svgCode();
        adjustDimensions(weakBoson);
    }
    for(const Gluon &gluon: this->_gluons){
        svgCode += gluon.svgCode();
        adjustDimensions(gluon);
    }
    for(const Higgs &higgs: this->_higgsBosons){
        svgCode += higgs.svgCode();
        adjustDimensions(higgs);
    }
    if(svgCode.isEmpty()){
        return "";
    }
    x1 -= 10;
    y1 -= 10;
    x2 += 10;
    y2 += 10;
    return QString("<?xml version=\"1.0\"?><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%1\" height=\"%2\" viewBox=\"%3 %4 %1 %2\"><rect x=\"%3\" y=\"%4\" width=\"%1\" height=\"%2\" fill=\"white\"/>%5</svg>").arg(x2 - x1).arg(y2 - y1).arg(x1).arg(y1).arg(svgCode);
}

void DiagramViewer::setGridVisibiliy(bool visible){
    for(QGraphicsLineItem *line: qAsConst(this->_grid)){
        this->scene()->removeItem(line);
        delete line;
    }
    this->_grid.clear();
    if(visible){
        this->scene()->setSceneRect(0, 0, viewSize, viewSize);
        for(int i = interval; i < viewSize; i += interval){
            this->_grid.append(this->scene()->addLine(0, i, viewSize, i, QPen(Qt::gray)));
            this->_grid.append(this->scene()->addLine(i, 0, i, viewSize, QPen(Qt::gray)));
        }
    }
}

void DiagramViewer::editSelectedLabel(const QString &newText){
    if(this->_selectedPath != nullptr){
        if(this->_fermions.contains(this->_selectedPath)){
            this->_fermions.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_photons.contains(this->_selectedPath)){
            this->_photons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_weakBosons.contains(this->_selectedPath)){
            this->_weakBosons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_gluons.contains(this->_selectedPath)){
            this->_gluons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_higgsBosons.contains(this->_selectedPath)){
            this->_higgsBosons.find(this->_selectedPath).value().setLabelText(newText);
        }
        this->_selectedPath = this->redrawPath(this->_selectedPath, selectionColor, selectionSize);
    }
}

void DiagramViewer::deleteSelectedParticle(){
    if(this->_selectedPath != nullptr){
        this->scene()->removeItem(this->_selectedPath);
        this->_fermions.remove(this->_selectedPath);
        this->_photons.remove(this->_selectedPath);
        this->_weakBosons.remove(this->_selectedPath);
        this->_gluons.remove(this->_selectedPath);
        this->_higgsBosons.remove(this->_selectedPath);
        delete this->_selectedPath;
        this->_selectedPath = nullptr;
        this->deselect();
    }
}

void DiagramViewer::mousePressEvent(QMouseEvent *event){
    if(this->_isDrawing){
        if(this->_currentParticle == nullptr){
            const QPoint from = (QVector2D(event->pos()) / interval).toPoint() * interval;
            switch(this->_currentParticleType){
            case Particle::Fermion:
                this->_currentParticle = new Fermion(from, event->pos());
                break;
            case Particle::Photon:
                this->_currentParticle = new Photon(from, event->pos());
                break;
            case Particle::WeakBoson:
                this->_currentParticle = new WeakBoson(from, event->pos());
                break;
            case Particle::Gluon:
                this->_currentParticle = new Gluon(from, event->pos());
                break;
            case Particle::Higgs:
                this->_currentParticle = new Higgs(from, event->pos());
                break;
            }
            this->_currentPath = this->scene()->addPath(this->_currentParticle->painterPath(), Qt::NoPen, QBrush(Qt::black));
        }
    }
}

void DiagramViewer::mouseReleaseEvent(QMouseEvent *event){
    if(this->_currentParticle != nullptr){
        const QPoint to = (QVector2D(event->pos()) / interval).toPoint() * interval;
        this->_currentParticle->setEndPoint(to);
        this->scene()->removeItem(this->_currentPath);
        if(this->_currentParticle->startingPoint() != to){
            QGraphicsPathItem *path = this->scene()->addPath(this->_currentParticle->painterPath(), Qt::NoPen, QBrush(Qt::black));
            switch(this->_currentParticleType){
            case Particle::Fermion:
            {
                const Fermion fermion = *static_cast<Fermion*>(this->_currentParticle);
                if(this->_fermions.key(fermion, nullptr)){
                    this->scene()->removeItem(path);
                    delete path;
                }
                else{
                    this->_fermions.insert(path, fermion);
                }
                break;
            }
            case Particle::Photon:
            {
                const Photon photon = *static_cast<Photon*>(this->_currentParticle);
                if(this->_photons.key(photon, nullptr)){
                    this->scene()->removeItem(path);
                    delete path;
                }
                else{
                    this->_photons.insert(path, photon);
                }
                break;
            }
            case Particle::WeakBoson:
            {
                const WeakBoson weakBoson = *static_cast<WeakBoson*>(this->_currentParticle);
                if(this->_weakBosons.key(weakBoson, nullptr)){
                    this->scene()->removeItem(path);
                    delete path;
                }
                else{
                    this->_weakBosons.insert(path, weakBoson);
                }
                break;
            }
            case Particle::Gluon:
            {
                const Gluon gluon = *static_cast<Gluon*>(this->_currentParticle);
                if(this->_gluons.key(gluon, nullptr)){
                    this->scene()->removeItem(path);
                    delete path;
                }
                else{
                    this->_gluons.insert(path, gluon);
                }
                break;
            }
            case Particle::Higgs:
            {
                const Higgs higgs = *static_cast<Higgs*>(this->_currentParticle);
                if(this->_higgsBosons.key(higgs, nullptr)){
                    this->scene()->removeItem(path);
                    delete path;
                }
                else{
                    this->_higgsBosons.insert(path, higgs);
                }
                break;
            }
            }
        }
        delete this->_currentPath;
        delete this->_currentParticle;
        this->_currentPath = nullptr;
        this->_currentParticle = nullptr;
        this->stopDrawing();
    }
    else{
        QGraphicsPathItem *path = dynamic_cast<QGraphicsPathItem*>(this->scene()->itemAt(event->pos(), QTransform()));
        if(path != nullptr && path != this->_selectedPath){
            this->deselect();
            this->_selectedPath = this->redrawPath(path, selectionColor, selectionSize);
            if(this->_fermions.contains(this->_selectedPath)){
                emit this->particleSelected(this->_fermions.find(this->_selectedPath).value());
            }
            else if(this->_photons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_photons.find(this->_selectedPath).value());
            }
            else if(this->_weakBosons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_weakBosons.find(this->_selectedPath).value());
            }
            else if(this->_gluons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_gluons.find(this->_selectedPath).value());
            }
            else if(this->_higgsBosons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_higgsBosons.find(this->_selectedPath).value());
            }
        }
        else{
            this->deselect();
        }
    }
}

void DiagramViewer::mouseMoveEvent(QMouseEvent *event){
    if(this->_currentParticle != nullptr){
        this->_currentParticle->setEndPoint(event->pos());
        this->scene()->removeItem(this->_currentPath);
        delete this->_currentPath;
        this->_currentPath = this->scene()->addPath(this->_currentParticle->painterPath(), Qt::NoPen, QBrush(Qt::black));
    }
}

QGraphicsPathItem *DiagramViewer::redrawPath(QGraphicsPathItem *path, const QColor &color, int strokeWidth){
    QPen pen;
    if(strokeWidth == 0){
        pen = Qt::NoPen;
    }
    else{
        pen.setWidth(strokeWidth);
        pen.setColor(color);
    }
    if(this->_fermions.contains(path)){
        const Fermion fermion = this->_fermions.find(path).value();
        QGraphicsPathItem *newPath = this->scene()->addPath(fermion.painterPath(), pen, QBrush(color));
        this->_fermions.insert(newPath, fermion);
        this->_fermions.remove(path);
        delete path;
        return newPath;
    }
    else if(this->_photons.contains(path)){
        const Photon photon = this->_photons.find(path).value();
        QGraphicsPathItem *newPath = this->scene()->addPath(photon.painterPath(), pen, QBrush(color));
        this->_photons.insert(newPath, photon);
        this->_photons.remove(path);
        delete path;
        return newPath;
    }
    else if(this->_weakBosons.contains(path)){
        const WeakBoson weakBoson = this->_weakBosons.find(path).value();
        QGraphicsPathItem *newPath = this->scene()->addPath(weakBoson.painterPath(), pen, QBrush(color));
        this->_weakBosons.insert(newPath, weakBoson);
        this->_weakBosons.remove(path);
        delete path;
        return newPath;
    }
    else if(this->_gluons.contains(path)){
        const Gluon gluon = this->_gluons.find(path).value();
        QGraphicsPathItem *newPath = this->scene()->addPath(gluon.painterPath(), pen, QBrush(color));
        this->_gluons.insert(newPath, gluon);
        this->_gluons.remove(path);
        delete path;
        return newPath;
    }
    else if(this->_higgsBosons.contains(path)){
        const Higgs higgs = this->_higgsBosons.find(path).value();
        QGraphicsPathItem *newPath = this->scene()->addPath(higgs.painterPath(), pen, QBrush(color));
        this->_higgsBosons.insert(newPath, higgs);
        this->_higgsBosons.remove(path);
        delete path;
        return newPath;
    }
    return nullptr;
}
