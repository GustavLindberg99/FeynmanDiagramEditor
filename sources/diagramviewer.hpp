#ifndef DIAGRAMVIEWER_H
#define DIAGRAMVIEWER_H

#include <QGraphicsView>
#include <QMap>

#include <memory>

#include "particle.hpp"

class DiagramViewer : public QGraphicsView {
    Q_OBJECT

public:
    DiagramViewer(QWidget* parent);

    void startDrawing(Particle::ParticleType particleType);
    void stopDrawing();

    void clear();
    void resetHistory();

    friend QDataStream &operator<<(QDataStream &dataStream, const DiagramViewer *diagramViewer);
    friend QDataStream &operator>>(QDataStream &dataStream, DiagramViewer *diagramViewer);
    QString toSvg() const;

public slots:
    void setGridVisibiliy(bool visible);

    void editSelectedLabel(const QString &newText);
    void deleteSelectedParticle();

    void deselect();

    void undo();
    void redo();

signals:
    void drawingStopped();

    void particleSelected(const Particle &particle);
    void particleDeselected();

    void undoAvailable(bool available);
    void redoAvailable(bool available);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    struct ParticleList{
        QMap<QGraphicsPathItem*, Fermion> fermions;
        QMap<QGraphicsPathItem*, Photon> photons;
        QMap<QGraphicsPathItem*, WeakBoson> weakBosons;
        QMap<QGraphicsPathItem*, Gluon> gluons;
        QMap<QGraphicsPathItem*, Higgs> higgsBosons;
        QMap<QGraphicsPathItem*, GenericBoson> genericBosons;
        QMap<QGraphicsPathItem*, Hadron> hadrons;
        QMap<QGraphicsPathItem*, Vertex> vertices;
    };

    QGraphicsPathItem *redrawPath(QGraphicsPathItem *path, const QColor &color = Qt::black, int strokeWidth = 0);
    void redrawAll(const QList<Fermion> &fermions, const QList<Photon> &photons, const QList<WeakBoson> &weakBosons, const QList<Gluon> &gluons, const QList<Higgs> &higgsBosons, const QList<GenericBoson> &genericBosons, const QList<Hadron> &hadrons, const QList<Vertex> &vertices);
    void redrawAll(const ParticleList &particleList);
    void updateHistory();

    ParticleList _particleList;

    QList<ParticleList> _history;
    QList<ParticleList>::iterator _currentHistoryItem;

    QList<QGraphicsLineItem*> _grid;

    bool _isDrawing;
    Particle::ParticleType _currentParticleType;
    std::unique_ptr<Particle> _currentParticle;
    QGraphicsPathItem *_currentPath;
    QGraphicsPathItem *_selectedPath;

    static const int viewSize, interval;
    static const int selectionSize;
    static const QColor selectionColor;
};

#endif // DIAGRAMVIEWER_H
