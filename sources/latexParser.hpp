#ifndef LATEXPARSER_H
#define LATEXPARSER_H

#include <QString>
#include <QFont>
#include <QPoint>
#include <QList>

struct Text{
    Text(const QString &text, const QPoint &position, const QFont &font): text(text), position(position), font(font){}

    QString text;
    QPoint position;
    QFont font;
};

const QList<Text> parseLatex(const QString &latexCode, const QPoint &position, const QFont &font, bool centerHorizontally);

#endif // LATEXPARSER_H
