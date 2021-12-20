#include <QRegularExpression>
#include <QFontMetrics>
#include <QFontInfo>
#include "latexParser.h"

const QList<Text> parseLatex(QString latexCode, const QPoint &position, const QFont &font, bool centerHorizontally){
    QList<Text> toReturn;
    latexCode.replace(QRegularExpression("\\\\alpha(?![a-zA-Z])"), "α")
             .replace(QRegularExpression("\\\\beta(?![a-zA-Z])"), "β")
             .replace(QRegularExpression("\\\\gamma(?![a-zA-Z])"), "γ")
             .replace(QRegularExpression("\\\\Gamma(?![a-zA-Z])"), "Γ")
             .replace(QRegularExpression("\\\\delta(?![a-zA-Z])"), "δ")
             .replace(QRegularExpression("\\\\Delta(?![a-zA-Z])"), "Δ")
             .replace(QRegularExpression("\\\\epsilon(?![a-zA-Z])"), "ϵ")
             .replace(QRegularExpression("\\\\varepsilon(?![a-zA-Z])"), "ε")
             .replace(QRegularExpression("\\\\zeta(?![a-zA-Z])"), "ζ")
             .replace(QRegularExpression("\\\\eta(?![a-zA-Z])"), "η")
             .replace(QRegularExpression("\\\\theta(?![a-zA-Z])"), "θ")
             .replace(QRegularExpression("\\\\vartheta(?![a-zA-Z])"), "ϑ")
             .replace(QRegularExpression("\\\\Theta(?![a-zA-Z])"), "Θ")
             .replace(QRegularExpression("\\\\iota(?![a-zA-Z])"), "ι")
             .replace(QRegularExpression("\\\\kappa(?![a-zA-Z])"), "κ")
             .replace(QRegularExpression("\\\\lambda(?![a-zA-Z])"), "λ")
             .replace(QRegularExpression("\\\\Lambda(?![a-zA-Z])"), "Λ")
             .replace(QRegularExpression("\\\\mu(?![a-zA-Z])"), "µ")
             .replace(QRegularExpression("\\\\nu(?![a-zA-Z])"), "ν")
             .replace(QRegularExpression("\\\\xi(?![a-zA-Z])"), "ξ")
             .replace(QRegularExpression("\\\\Xi(?![a-zA-Z])"), "Ξ")
             .replace(QRegularExpression("\\\\pi(?![a-zA-Z])"), "π")
             .replace(QRegularExpression("\\\\Pi(?![a-zA-Z])"), "Π")
             .replace(QRegularExpression("\\\\rho(?![a-zA-Z])"), "ρ")
             .replace(QRegularExpression("\\\\varrho(?![a-zA-Z])"), "ϱ")
             .replace(QRegularExpression("\\\\sigma(?![a-zA-Z])"), "σ")
             .replace(QRegularExpression("\\\\Sigma(?![a-zA-Z])"), "Σ")
             .replace(QRegularExpression("\\\\tau(?![a-zA-Z])"), "τ")
             .replace(QRegularExpression("\\\\upsilon(?![a-zA-Z])"), "υ")
             .replace(QRegularExpression("\\\\Upsilon(?![a-zA-Z])"), "ϒ")
             .replace(QRegularExpression("\\\\phi(?![a-zA-Z])"), "ϕ")
             .replace(QRegularExpression("\\\\varphi(?![a-zA-Z])"), "φ")
             .replace(QRegularExpression("\\\\Phi(?![a-zA-Z])"), "Φ")
             .replace(QRegularExpression("\\\\chi(?![a-zA-Z])"), "χ")
             .replace(QRegularExpression("\\\\psi(?![a-zA-Z])"), "ψ")
             .replace(QRegularExpression("\\\\Psi(?![a-zA-Z])"), "Ψ")
             .replace(QRegularExpression("\\\\omega(?![a-zA-Z])"), "ω")
             .replace(QRegularExpression("\\\\Omega(?![a-zA-Z])"), "Ω");

    const QRegularExpression subSuperscriptRegex("(?<!\\\\)([\\^_])([^{\\\\]|\\\\.|\\{[^}]+\\})");
    QFont subSuperScriptFont = font;
    subSuperScriptFont.setPointSizeF(QFontInfo(font).pointSizeF() * 0.75);
    int positionInString = 0;
    int graphicalPosition = 0;
    const QStringList &mainTexts = latexCode.split(subSuperscriptRegex);
    for(int i = 0; i < mainTexts.length(); i++){
        toReturn.append(Text(mainTexts[i], position + QPoint(graphicalPosition, 0), font));
        graphicalPosition += QFontMetrics(font).size(0, mainTexts[i]).width();

        QRegularExpressionMatch match = subSuperscriptRegex.match(latexCode, positionInString);
        const bool isSuperscript = match.captured(1) == "^";
        QString subSuperscriptText = match.captured(2);
        if(subSuperscriptText.startsWith('\\')){
            subSuperscriptText.remove(0, 1);
        }
        else if(subSuperscriptText.startsWith('{')){
            subSuperscriptText.replace(QRegularExpression("^\\{|\\}$"), "");
        }
        toReturn.append(Text(subSuperscriptText, position + QPoint(graphicalPosition, isSuperscript ? -QFontInfo(font).pixelSize() / 2 : 0), subSuperScriptFont));
        graphicalPosition += QFontMetrics(subSuperScriptFont).size(0, subSuperscriptText).width();
        positionInString = match.capturedStart(2);
    }
    if(centerHorizontally){
        for(Text &text: toReturn){
            text.position -= QPoint(graphicalPosition / 2, 0);
        }
    }
    return toReturn;
}
