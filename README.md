# FeynmanDiagramEditor
FeynmanDiagramEditor is a program to easily draw Feynman diagrams.

![](https://i.stack.imgur.com/0gtDD.png)

# Installation
## Windows
On Windows, the recommended way of installing FeynmanDiagramEditor is to download and run [this installation program](https://github.com/Gustav-Lindberg/FeynmanDiagramEditor/raw/main/FeynmanDiagramEditor-setup-windows.exe). If you can't or don't want to use the installation program, you can also download [this zip file](https://github.com/Gustav-Lindberg/FeynmanDiagramEditor/raw/main/FeynmanDiagramEditor-windows.zip) containing the executable and its dependencies.

## Linux
On Linux, you can download the binary file for FeynmanDiagramEditor [here](https://github.com/Gustav-Lindberg/FeynmanDiagramEditor/raw/main/FeynmanDiagramEditor-linux). Place it in whatever folder you want and run it.

# Usage
## Drawing particles
To draw a particle, use the button in the toolbar that looks like the line in a Feynman diagram for that particle. For example, to draw a fermion, use the <img src="https://raw.githubusercontent.com/Gustav-Lindberg/FeynmanDiagramEditor/main/sources/icons/fermion.svg" height="20"/> button. Then once you've clicked the button, drag the cursor in the drawing area from where you want the line to start to where you want it to finish.

You can select a particle by clicking on it. You must click exactly on the line, close isn't enough. Once you've selected a particle, you can add a legend for it by filling out the text area in the toolbar. A very limited set of Latex commands are available:

- All Greek letters
- `\pm`
- `\ell`
- `\bar`
- Subscript (`_`) and superscript (`^`) are supported as long as they're not nested
- Escape sequences for spaces: `\:` and `\;`
- Escape sequences for characters with a special meaning: `\^`, `\_`, `\{`, `\}` and `\backslash`

If you don't want the particle to have a label, leave the text area blank.

You can also add labels to vertices. To do so, you first need to draw the vertex explicitly by clicking on the <img src="https://raw.githubusercontent.com/Gustav-Lindberg/FeynmanDiagramEditor/main/sources/icons/vertex.svg" height="20"/> button. Then add a label to the vertex just like you would for a particle.

You can delete the selected particle by pressing the Delete key.

## Saving and exporting diagrams
You can save a Feynman diagram in the FDG format (a format specific for FeynmanDiagramEditor) by pressing CTRL+S. You open FDG files in FeynmanDiagramEditor by pressing CTRL+O.

If you want to use your Feynman diagram elsewhere, you can also export it in more common formats (SVG, PNG or PDF). To do this, press CTRL+E. FeynmanDiagramEditor can only create files in these formats, it can't open them. So if you think you might want to edit the Feynman diagram later, you should also save a copy of it in the FDG format.
