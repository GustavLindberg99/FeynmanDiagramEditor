#include "latexParser.hpp"

#include <QRegularExpression>
#include <QFontMetrics>
#include <QFontInfo>

const QStringList tokenize(const QString &latexCode){
    QStringList toReturn("");
    for(const QChar &character: latexCode){
        if(toReturn.last().startsWith('{')){
            if(toReturn.last().endsWith('}') && !toReturn.last().endsWith("\\}")){
                toReturn.append("");
            }
        }
        else if(character == '\\' || toReturn.last() == "^" || toReturn.last() == "_"){
            toReturn.append("");
        }
        else if((character == '^' || character == '_') && !toReturn.last().endsWith("\\")){
            toReturn.append("");
        }
        else if(toReturn.last().startsWith('\\')){
            static const QRegularExpression escapeRegex("^\\\\[_^{}:;]$");
            if(toReturn.last().contains(escapeRegex)){
                toReturn.append("");
            }
            else if(!character.isLetter() && (toReturn.last().length() >= 2 || (character != '_' && character != '^' && character != '{' && character != '}' && character != ':' && character != ';'))){
                toReturn.append("");
            }
        }
        toReturn.last() += character;
        if(toReturn.length() >= 2 && !toReturn.last().startsWith('{') && !toReturn.last().startsWith('\\') && (toReturn[toReturn.length() - 2] == "^" || toReturn[toReturn.length() - 2] == "_")){
            toReturn.append("");
        }
    }
    return toReturn;
}

QString latexSequenceToUnicode(const QString &token){
    if(token == "\\alpha") return "α";
    else if(token == "\\beta") return "β";
    else if(token == "\\gamma") return "γ";
    else if(token == "\\Gamma") return "Γ";
    else if(token == "\\delta") return "δ";
    else if(token == "\\Delta") return "Δ";
    else if(token == "\\epsilon") return "ϵ";
    else if(token == "\\varepsilon") return "ε";
    else if(token == "\\zeta") return "ζ";
    else if(token == "\\eta") return "η";
    else if(token == "\\theta") return "θ";
    else if(token == "\\vartheta") return "ϑ";
    else if(token == "\\Theta") return "Θ";
    else if(token == "\\iota") return "ι";
    else if(token == "\\kappa") return "κ";
    else if(token == "\\lambda") return "λ";
    else if(token == "\\Lambda") return "Λ";
    else if(token == "\\mu") return "µ";
    else if(token == "\\nu") return "ν";
    else if(token == "\\xi") return "ξ";
    else if(token == "\\Xi") return "Ξ";
    else if(token == "\\pi") return "π";
    else if(token == "\\pi") return "Π";
    else if(token == "\\rho") return "ρ";
    else if(token == "\\varrho") return "ϱ";
    else if(token == "\\sigma") return "σ";
    else if(token == "\\Sigma") return "Σ";
    else if(token == "\\tau") return "τ";
    else if(token == "\\upsilon") return "υ";
    else if(token == "\\Upsilon") return "ϒ";
    else if(token == "\\phi") return "ϕ";
    else if(token == "\\varphi") return "φ";
    else if(token == "\\Phi") return "Φ";
    else if(token == "\\chi") return "χ";
    else if(token == "\\psi") return "ψ";
    else if(token == "\\Psi") return "Ψ";
    else if(token == "\\omega") return "ω";
    else if(token == "\\Omega") return "Ω";
    else if(token == "\\ell") return "ℓ";
    else if(token == "\\pm") return "±";
    else if(token == "\\:") return " ";
    else if(token == "\\;") return "  ";
    else if(token == "\\^") return "^";
    else if(token == "\\_") return "_";
    else if(token == "\\{") return "{";
    else if(token == "\\}") return "}";
    else if(token == "\\backslash") return "\\";
    else return token;
}

int width(Text text){
    return QFontMetrics(text.font).size(0, text.text).width();
}

const QList<Text> parseLatex(const QString &latexCode, const QPoint &position, const QFont &font, bool centerHorizontally){
    QFont subSuperScriptFont = font;
    subSuperScriptFont.setPointSizeF(QFontInfo(font).pointSizeF() * 0.75);

    QList<Text> toReturn;
    int graphicalPosition = 0;
    bool inSubOrSuperScript = true;
    bool subscript = false, superscript = false;
    bool bar = false;
    QStringList tokens;
    for(const QString &token: tokenize(latexCode)){
        if(token.startsWith('{') && token.endsWith('}')){
            if(tokens.last() == "^" || tokens.last() == "_"){
                tokens.append(QString(token).replace(" ", ""));
            }
            else for(const QString &subtoken: tokenize(token.mid(1, token.length() - 2))){
                tokens.append(QString(subtoken).replace(" ", ""));
            }
        }
        else{
            tokens.append(QString(token).replace(" ", ""));
        }
    }
    for(const QString &token: qAsConst(tokens)){
        if(token == "^" || token == "_"){
            if(token == "^") superscript = true;
            else subscript = true;
        }
        else if(token == "\\bar"){
            bar = true;
        }
        else if(subscript || superscript){
            if(!toReturn.isEmpty()){
                graphicalPosition += width(toReturn.last());
            }
            if(token.startsWith('{') && token.endsWith('}')){
                toReturn.append(Text("", position + QPoint(graphicalPosition, superscript ? -QFontInfo(font).pixelSize() / 2 : QFontInfo(font).pixelSize() / 3), subSuperScriptFont));
                for(const QString &subtoken: tokenize(token.mid(1, token.length() - 2))){
                    if(subtoken == "\\bar"){
                        bar = true;
                    }
                    else{
                        QString text = latexSequenceToUnicode(subtoken);
                        if(bar){
                            text.insert(1, "̅");
                            bar = false;
                        }
                        toReturn.last().text += text;
                    }
                }
            }
            else{
                QString text = latexSequenceToUnicode(token);
                if(bar){
                    text.insert(1, "̅");
                    bar = false;
                }
                toReturn.append(Text(text, position + QPoint(graphicalPosition, superscript ? -QFontInfo(font).pixelSize() / 2 : QFontInfo(font).pixelSize() / 3), subSuperScriptFont));
            }
            subscript = superscript = false;
            inSubOrSuperScript = true;
        }
        else if(inSubOrSuperScript){    //If the previous one was subscript or superscript, this one should be normal text
            if(!toReturn.isEmpty()){
                graphicalPosition += width(toReturn.last());
            }
            QString text = latexSequenceToUnicode(token);
            if(bar){
                text.insert(1, "̅");
                bar = false;
            }
            toReturn.append(Text(text, position + QPoint(graphicalPosition, 0), font));
            inSubOrSuperScript = false;
        }
        else{
            if(toReturn.isEmpty()){
                toReturn.append(Text("", position, font));
            }
            QString text = latexSequenceToUnicode(token);
            if(bar){
                text.insert(1, "̅");    //The text editor might not render this very clearly, but "̅" is a string containing the Unicode 773 (0x305) character that places a bar on top of the previous character.
                bar = false;
            }
            toReturn.last().text += text;
        }
    }
    if(centerHorizontally){
        graphicalPosition += width(toReturn.last());
        for(Text &text: toReturn){
            text.position -= QPoint(graphicalPosition / 2, 0);
        }
    }
    return toReturn;
}
