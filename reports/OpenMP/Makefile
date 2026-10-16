.PHONY: all clean

all: report.pdf

report.pdf: report.tex
	mkdir -p build
	latexmk -pdf -interaction=nonstopmode -halt-on-error -outdir=build report.tex
	cp build/report.pdf report.pdf

clean:
	latexmk -C -outdir=build report.tex
	rm -f report.pdf
