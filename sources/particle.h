#ifndef PARTICLE_H
#define PARTICLE_H

#include <QString>
#include <QPainterPath>
#include <QVector2D>
#include <functional>

class Particle{
public:
    enum ParticleType{Fermion, Photon, WeakBoson, Gluon, Higgs};

    Particle(const QPoint &from = QPoint(), const QPoint &to = QPoint());
    virtual ~Particle();

    bool operator==(const Particle &other) const;

    QPoint startingPoint() const;
    void setEndPoint(const QPoint &to);

    virtual QString svgCode() const = 0;
    virtual QPainterPath painterPath() const = 0;

    void setLabelText(const QString &text);
    QString labelText() const;

    friend QDataStream &operator<<(QDataStream &dataStream, const Particle &particle);
    friend QDataStream &operator>>(QDataStream &dataStream, Particle &particle);

protected:
    QVector2D direction() const;
    QVector2D normal() const;

    void addLabel(QPainterPath *path, QString *svgCode) const;

    QPoint _from, _to;

private:
    QString _labelText;
};

class Fermion: public Particle{
public:
    using Particle::Particle;

    QString svgCode() const override;
    QPainterPath painterPath() const override;

private:
    const QList<QPoint> arrowPoints() const;

    static const int arrowSize;
};

class Boson: public Particle{
public:
    using Particle::Particle;

protected:
    const QList<QPoint> points() const;

    static const int spacing;
};

class MasslessBoson: public Boson{
public:
    using Boson::Boson;

    QString svgCode() const override;
    QPainterPath painterPath() const override;

protected:
    virtual void iterateOverPoints(const std::function<void(const QPoint&, const QPoint&, const QPoint&)> &callback) const = 0;
};

class Photon: public MasslessBoson{
public:
    using MasslessBoson::MasslessBoson;

protected:
    void iterateOverPoints(const std::function<void(const QPoint&, const QPoint&, const QPoint&)> &callback) const override;
};

class Gluon: public MasslessBoson{
public:
    using MasslessBoson::MasslessBoson;

protected:
    void iterateOverPoints(const std::function<void(const QPoint&, const QPoint&, const QPoint&)> &callback) const override;
};

class WeakBoson: public Boson{
public:
    using Boson::Boson;

    QString svgCode() const override;
    QPainterPath painterPath() const override;
};

class Higgs: public Particle{
public:
    using Particle::Particle;

    QString svgCode() const override;
    QPainterPath painterPath() const override;

private:
    static const int dashLength;
};

#endif // PARTICLE_H
