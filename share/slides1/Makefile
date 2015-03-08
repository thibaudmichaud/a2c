all: slides.pdf

slides.pdf: slides.tex
	latexmk --pdf -interaction=nonstopmode slides.tex

clean:
	rm -f *.{toc,snm,out,nav,aux,fls,log,fdb_latexmk}
