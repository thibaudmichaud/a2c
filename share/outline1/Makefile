all: outline.pdf

outline.pdf: outline.tex
	latexmk --pdf -interaction=nonstopmode outline.tex

clean:
	rm -f *.{toc,snm,out,nav,aux,fls,log,fdb_latexmk}
