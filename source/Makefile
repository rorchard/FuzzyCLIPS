# Top-level Makefile for FZ_CLIPS and FZ_XCLIPS


all:
	@echo
	@echo Please specify fzclips, fzxclips or both.
	@echo
	@echo If you specify both, then the object files will
	@echo be cleaned up prior to both builds. 
	@echo This is because the compilation flags and options
	@echo vary for the different builds. 
	@echo
	@echo If you have built one before, and now wish to build
	@echo the other then you MUST either clean up or build
	@echo both executables for this makefile to work.
	@echo

fzclips:
	make -f Makefile.cl fz_clips

fzxclips:
	make -f Makefile.x  fz_xclips

both: 	dist_clean fzclips clean1 fzxclips

clean:
	rm -f *.o 

clean1:
	rm -f *.o

dist_clean: clean
	rm -f fz_clips
	rm -f fz_xclips

dist:
	tar -cvf FZCLIPS-6.10.tar *.c *.h Makefile Makefile.cl Makefile.x
	compress  FZCLIPS-6.10.tar
