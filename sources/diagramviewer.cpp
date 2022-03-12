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
    this->resetHistory();
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
    if(this->_currentPath != nullptr){
        delete this->_currentPath;
        this->_currentPath = nullptr;
    }
    if(this->_currentParticle != nullptr){
        delete this->_currentParticle;
        this->_currentParticle = nullptr;
    }
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

    const QList<QGraphicsPathItem*> paths = this->_particleList.fermions.keys() + this->_particleList.photons.keys() + this->_particleList.weakBosons.keys() + this->_particleList.gluons.keys() + this->_particleList.higgsBosons.keys() + this->_particleList.genericBosons.keys() + this->_particleList.hadrons.keys() + this->_particleList.vertices.keys();
    for(QGraphicsPathItem *path: paths){
        this->scene()->removeItem(path);
        delete path;
    }
    this->_particleList.fermions.clear();
    this->_particleList.photons.clear();
    this->_particleList.weakBosons.clear();
    this->_particleList.gluons.clear();
    this->_particleList.higgsBosons.clear();
    this->_particleList.genericBosons.clear();
    this->_particleList.hadrons.clear();
    this->_particleList.vertices.clear();
}

void DiagramViewer::resetHistory(){
    this->_history.clear();
    this->_history.append(this->_particleList);
    this->_currentHistoryItem = this->_history.end() - 1;
    emit this->undoAvailable(false);
    emit this->redoAvailable(false);
}

QDataStream &operator<<(QDataStream &dataStream, const DiagramViewer *diagramViewer){
    dataStream << diagramViewer->_particleList.fermions.values() << diagramViewer->_particleList.photons.values() << diagramViewer->_particleList.weakBosons.values() << diagramViewer->_particleList.gluons.values() << diagramViewer->_particleList.higgsBosons.values() << diagramViewer->_particleList.hadrons.values() << diagramViewer->_particleList.vertices.values() << diagramViewer->_particleList.genericBosons.values();
    return dataStream;
}

QDataStream &operator>>(QDataStream &dataStream, DiagramViewer *diagramViewer){
    QList<Fermion> fermions;
    QList<Photon> photons;
    QList<WeakBoson> weakBosons;
    QList<Gluon> gluons;
    QList<Higgs> higgsBosons;
    QList<GenericBoson> genericBosons;
    QList<Hadron> hadrons;
    QList<Vertex> vertices;
    dataStream >> fermions >> photons >> weakBosons >> gluons >> higgsBosons;
    if(!dataStream.atEnd()){
        dataStream >> hadrons >> vertices;
    }
    if(!dataStream.atEnd()){
        dataStream >> genericBosons;
    }
    diagramViewer->redrawAll(fermions, photons, weakBosons, gluons, higgsBosons, genericBosons, hadrons, vertices);
    diagramViewer->resetHistory();
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
    for(const Fermion &fermion: this->_particleList.fermions){
        svgCode += fermion.svgCode();
        adjustDimensions(fermion);
    }
    for(const Photon &photon: this->_particleList.photons){
        svgCode += photon.svgCode();
        adjustDimensions(photon);
    }
    for(const WeakBoson &weakBoson: this->_particleList.weakBosons){
        svgCode += weakBoson.svgCode();
        adjustDimensions(weakBoson);
    }
    for(const Gluon &gluon: this->_particleList.gluons){
        svgCode += gluon.svgCode();
        adjustDimensions(gluon);
    }
    for(const Higgs &higgs: this->_particleList.higgsBosons){
        svgCode += higgs.svgCode();
        adjustDimensions(higgs);
    }
    for(const GenericBoson &boson: this->_particleList.genericBosons){
        svgCode += boson.svgCode();
        adjustDimensions(boson);
    }
    for(const Hadron &hadron: this->_particleList.hadrons){
        svgCode += hadron.svgCode();
        adjustDimensions(hadron);
    }
    for(const Vertex &vertex: this->_particleList.vertices){
        svgCode += vertex.svgCode();
        adjustDimensions(vertex);
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
        if(this->_particleList.fermions.contains(this->_selectedPath)){
            this->_particleList.fermions.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.photons.contains(this->_selectedPath)){
            this->_particleList.photons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.weakBosons.contains(this->_selectedPath)){
            this->_particleList.weakBosons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.gluons.contains(this->_selectedPath)){
            this->_particleList.gluons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.higgsBosons.contains(this->_selectedPath)){
            this->_particleList.higgsBosons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.genericBosons.contains(this->_selectedPath)){
            this->_particleList.genericBosons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.hadrons.contains(this->_selectedPath)){
            this->_particleList.hadrons.find(this->_selectedPath).value().setLabelText(newText);
        }
        else if(this->_particleList.vertices.contains(this->_selectedPath)){
            this->_particleList.vertices.find(this->_selectedPath).value().setLabelText(newText);
        }
        this->_selectedPath = this->redrawPath(this->_selectedPath, selectionColor, selectionSize);
    }
}

void DiagramViewer::deleteSelectedParticle(){
    if(this->_selectedPath != nullptr){
        this->scene()->removeItem(this->_selectedPath);
        this->_particleList.fermions.remove(this->_selectedPath);
        this->_particleList.photons.remove(this->_selectedPath);
        this->_particleList.weakBosons.remove(this->_selectedPath);
        this->_particleList.gluons.remove(this->_selectedPath);
        this->_particleList.higgsBosons.remove(this->_selectedPath);
        this->_particleList.genericBosons.remove(this->_selectedPath);
        this->_particleList.hadrons.remove(this->_selectedPath);
        this->_particleList.vertices.remove(this->_selectedPath);
        delete this->_selectedPath;
        this->_selectedPath = nullptr;
        this->deselect();
        this->updateHistory();
    }
}

void DiagramViewer::undo(){
    if(!this->_history.isEmpty() && this->_currentHistoryItem != this->_history.begin()){
        this->stopDrawing();
        this->_currentHistoryItem--;
        this->redrawAll(*this->_currentHistoryItem);

        emit this->redoAvailable(true);
        if(this->_currentHistoryItem == this->_history.begin()){
            emit this->undoAvailable(false);
        }
    }
}

void DiagramViewer::redo(){
    if(!this->_history.isEmpty() && this->_currentHistoryItem != this->_history.end() - 1){
        this->stopDrawing();
        this->_currentHistoryItem++;
        this->redrawAll(*this->_currentHistoryItem);

        emit this->undoAvailable(true);
        if(this->_currentHistoryItem == this->_history.end() - 1){
            emit this->redoAvailable(false);
        }
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
            case Particle::GenericBoson:
                this->_currentParticle = new GenericBoson(from, event->pos());
                break;
            case Particle::Hadron:
                this->_currentParticle = new Hadron(from, event->pos());
                break;
            case Particle::Vertex:
                this->_currentParticle = new Vertex(from);
                break;
            }
            this->_currentPath = this->scene()->addPath(this->_currentParticle->painterPath(), Qt::NoPen, QBrush(Qt::black));
            if(this->_currentParticleType == Particle::Vertex){
                this->mouseReleaseEvent(event);
            }
        }
    }
}

template<typename T>
constexpr void mouseReleaseEvent_helper(QMap<QGraphicsPathItem*, T> &particles, Particle *currentParticle, QGraphicsPathItem *path, QGraphicsScene *scene){
    const T particle = *static_cast<T*>(currentParticle);
    if(particles.key(particle, nullptr)){
        scene->removeItem(path);
        delete path;
    }
    else{
        particles.insert(path, particle);
    }
}

void DiagramViewer::mouseReleaseEvent(QMouseEvent *event){
    if(this->_currentParticle != nullptr){
        const QPoint to = (QVector2D(event->pos()) / interval).toPoint() * interval;
        this->_currentParticle->setEndPoint(to);
        this->scene()->removeItem(this->_currentPath);
        if(this->_currentParticle->startingPoint() != to || this->_currentParticleType == Particle::Vertex){
            QGraphicsPathItem *path = this->scene()->addPath(this->_currentParticle->painterPath(), Qt::NoPen, QBrush(Qt::black));
            switch(this->_currentParticleType){
            case Particle::Fermion:
                mouseReleaseEvent_helper(this->_particleList.fermions, this->_currentParticle, path, this->scene());
                break;
            case Particle::Photon:
                mouseReleaseEvent_helper(this->_particleList.photons, this->_currentParticle, path, this->scene());
                break;
            case Particle::WeakBoson:
                mouseReleaseEvent_helper(this->_particleList.weakBosons, this->_currentParticle, path, this->scene());
                break;
            case Particle::Gluon:
                mouseReleaseEvent_helper(this->_particleList.gluons, this->_currentParticle, path, this->scene());
                break;
            case Particle::Higgs:
                mouseReleaseEvent_helper(this->_particleList.higgsBosons, this->_currentParticle, path, this->scene());
                break;
            case Particle::GenericBoson:
                mouseReleaseEvent_helper(this->_particleList.genericBosons, this->_currentParticle, path, this->scene());
                break;
            case Particle::Hadron:
                mouseReleaseEvent_helper(this->_particleList.hadrons, this->_currentParticle, path, this->scene());
                break;
            case Particle::Vertex:
                mouseReleaseEvent_helper(this->_particleList.vertices, this->_currentParticle, path, this->scene());
                break;
            }
        }
        this->stopDrawing();
        this->updateHistory();
    }
    else{
        QGraphicsPathItem *path = dynamic_cast<QGraphicsPathItem*>(this->scene()->itemAt(event->pos(), QTransform()));
        if(path != nullptr && path != this->_selectedPath){
            this->deselect();
            this->_selectedPath = this->redrawPath(path, selectionColor, selectionSize);
            if(this->_particleList.fermions.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.fermions.find(this->_selectedPath).value());
            }
            else if(this->_particleList.photons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.photons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.weakBosons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.weakBosons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.gluons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.gluons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.higgsBosons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.higgsBosons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.genericBosons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.genericBosons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.hadrons.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.hadrons.find(this->_selectedPath).value());
            }
            else if(this->_particleList.vertices.contains(this->_selectedPath)){
                emit this->particleSelected(this->_particleList.vertices.find(this->_selectedPath).value());
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

template<typename T>
constexpr QGraphicsPathItem *redrawPath_helper(QMap<QGraphicsPathItem*, T> &particles, QGraphicsPathItem *path, const QColor &color, const QPen &pen, QGraphicsScene *scene){
    if(particles.contains(path)){
        const T particle = particles.find(path).value();
        QGraphicsPathItem *newPath = scene->addPath(particle.painterPath(), pen, QBrush(color));
        particles.insert(newPath, particle);
        particles.remove(path);
        delete path;
        return newPath;
    }
    return nullptr;
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
    QGraphicsPathItem *toReturn = nullptr;
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.fermions, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.photons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.weakBosons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.gluons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.higgsBosons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.genericBosons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.hadrons, path, color, pen, this->scene());
    if(!toReturn) toReturn = redrawPath_helper(this->_particleList.vertices, path, color, pen, this->scene());
    this->updateHistory();
    return toReturn;
}

void DiagramViewer::redrawAll(const QList<Fermion> &fermions, const QList<Photon> &photons, const QList<WeakBoson> &weakBosons, const QList<Gluon> &gluons, const QList<Higgs> &higgsBosons, const QList<GenericBoson> &genericBosons, const QList<Hadron> &hadrons, const QList<Vertex> &vertices){
    this->clear();
    for(const Fermion &fermion: fermions){
        QGraphicsPathItem *path = this->scene()->addPath(fermion.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.fermions.insert(path, fermion);
    }
    for(const Photon &photon: photons){
        QGraphicsPathItem *path = this->scene()->addPath(photon.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.photons.insert(path, photon);
    }
    for(const WeakBoson &weakBoson: weakBosons){
        QGraphicsPathItem *path = this->scene()->addPath(weakBoson.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.weakBosons.insert(path, weakBoson);
    }
    for(const Gluon &gluon: gluons){
        QGraphicsPathItem *path = this->scene()->addPath(gluon.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.gluons.insert(path, gluon);
    }
    for(const Higgs &higgs: higgsBosons){
        QGraphicsPathItem *path = this->scene()->addPath(higgs.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.higgsBosons.insert(path, higgs);
    }
    for(const GenericBoson &boson: genericBosons){
        QGraphicsPathItem *path = this->scene()->addPath(boson.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.genericBosons.insert(path, boson);
    }
    for(const Hadron &hadron: hadrons){
        QGraphicsPathItem *path = this->scene()->addPath(hadron.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.hadrons.insert(path, hadron);
    }
    for(const Vertex &vertex: vertices){
        QGraphicsPathItem *path = this->scene()->addPath(vertex.painterPath(), Qt::NoPen, QBrush(Qt::black));
        this->_particleList.vertices.insert(path, vertex);
    }
}

void DiagramViewer::redrawAll(const ParticleList &particleList){
    this->redrawAll(particleList.fermions.values(), particleList.photons.values(), particleList.weakBosons.values(), particleList.gluons.values(), particleList.higgsBosons.values(), particleList.genericBosons.values(), particleList.hadrons.values(), particleList.vertices.values());
}

void DiagramViewer::updateHistory(){
    if(!this->_history.isEmpty()){
        this->_history.erase(this->_currentHistoryItem + 1, this->_history.end());
    }
    this->_history.append(this->_particleList);
    this->_currentHistoryItem = this->_history.end() - 1;

    emit this->undoAvailable(true);
    emit this->redoAvailable(false);
}
