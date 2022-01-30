#ifndef DIAGRAMVIEWER_H
#define DIAGRAMVIEWER_H

#include <QGraphicsView>
#include <QMap>
#include "particle.h"

class DiagramViewer : public QGraphicsView{
    Q_OBJECT

public:
    DiagramViewer();
    virtual ~DiagramViewer();

    void startDrawing(Particle::ParticleType particleType);
    void stopDrawing();

    void deselect();
    void clear();

    friend QDataStream &operator<<(QDataStream &dataStream, const DiagramViewer *diagramViewer);
    friend QDataStream &operator>>(QDataStream &dataStream, DiagramViewer *diagramViewer);
    QString toSvg() const;

public slots:
    void setGridVisibiliy(bool visible);

    void editSelectedLabel(const QString &newText);
    void deleteSelectedParticle();

signals:
    void drawingStopped();

    void particleSelected(const Particle &particle);
    void particleDeselected();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QGraphicsPathItem *redrawPath(QGraphicsPathItem *path, const QColor &color = Qt::black, int strokeWidth = 0);

    bool _isDrawing;
    Particle::ParticleType _currentParticleType;
    Particle *_currentParticle;
    QGraphicsPathItem *_currentPath;

    QMap<QGraphicsPathItem*, Fermion> _fermions;
    QMap<QGraphicsPathItem*, Photon> _photons;
    QMap<QGraphicsPathItem*, WeakBoson> _weakBosons;
    QMap<QGraphicsPathItem*, Gluon> _gluons;
    QMap<QGraphicsPathItem*, Higgs> _higgsBosons;
    QMap<QGraphicsPathItem*, Hadron> _hadrons;
    QMap<QGraphicsPathItem*, Vertex> _vertices;

    QList<QGraphicsLineItem*> _grid;

    QGraphicsPathItem *_selectedPath;

    static const int viewSize, interval;
    static const int selectionSize;
    static const QColor selectionColor;
};

#endif // DIAGRAMVIEWER_H
