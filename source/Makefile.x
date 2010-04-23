# This makefile is setup for HPUX 10.20.
# Please ammend the paths to the include files
# and libraries according to local requirements.
#
# You will probably also want to run makedepend as well!

CC = cc
#CFLAGS = -c # see comments in Makefile.cl
# On systems that use dirent.h rather than dir.h, include 
# -DUSE_DIRENT in CFLAGS.
CFLAGS= -c -DUSE_DIRENT

#DEBUG = -g
DEBUG = 

LIBS = -L/usr/contrib/X11R6/lib -lXaw -lXmu -lXt -lXext -lX11 -lm

INCLUDES = -I/usr/contrib/X11R6/include

OBJS = agenda.o analysis.o argacces.o bload.o bmathfun.o bsave.o \
 	classcom.o classexm.o classfun.o classinf.o classini.o \
 	classpsr.o clsltpsr.o commline.o conscomp.o constrct.o \
 	constrnt.o crstrtgy.o cstrcbin.o cstrccom.o cstrcpsr.o \
 	cstrnbin.o cstrnchk.o cstrncmp.o cstrnops.o cstrnpsr.o \
 	cstrnutl.o default.o defins.o developr.o dffctbin.o dffctbsc.o \
 	dffctcmp.o dffctdef.o dffctpsr.o dffnxbin.o dffnxcmp.o \
 	dffnxexe.o dffnxfun.o dffnxpsr.o dfinsbin.o dfinscmp.o drive.o \
 	edbasic.o edmain.o edmisc.o edstruct.o edterm.o emathfun.o \
 	engine.o evaluatn.o expressn.o exprnbin.o exprnops.o \
 	exprnpsr.o extnfunc.o factbin.o factbld.o factcmp.o factcom.o \
 	factfun.o factgen.o facthsh.o factlhs.o factmch.o factmngr.o \
 	factprt.o factrete.o factrhs.o filecom.o filertr.o generate.o \
 	genrcbin.o genrccmp.o genrccom.o genrcexe.o genrcfun.o \
 	genrcpsr.o globlbin.o globlbsc.o globlcmp.o globlcom.o \
 	globldef.o globlpsr.o immthpsr.o incrrset.o inherpsr.o \
 	inscom.o insfile.o insfun.o insmngr.o insmoddp.o insmult.o \
 	inspsr.o insquery.o insqypsr.o iofun.o lgcldpnd.o \
 	memalloc.o miscfun.o modulbin.o modulbsc.o modulcmp.o moduldef.o \
 	modulpsr.o modulutl.o msgcom.o msgfun.o msgpass.o msgpsr.o \
 	multifld.o multifun.o objbin.o objcmp.o objrtbin.o objrtbld.o \
 	objrtcmp.o objrtfnx.o objrtgen.o objrtmch.o parsefun.o pattern.o \
 	pprint.o prccode.o prcdrfun.o prcdrpsr.o prdctfun.o prntutil.o \
 	proflfun.o reorder.o reteutil.o retract.o router.o rulebin.o \
 	rulebld.o rulebsc.o rulecmp.o rulecom.o rulecstr.o ruledef.o \
 	ruledlt.o rulelhs.o rulepsr.o scanner.o sortfun.o strngfun.o \
 	strngrtr.o symblbin.o symblcmp.o symbol.o sysdep.o textpro.o \
 	tmpltbin.o tmpltbsc.o tmpltcmp.o tmpltdef.o tmpltfun.o tmpltlhs.o \
 	tmpltpsr.o tmpltrhs.o tmpltutl.o userdata.o utility.o watch.o


FUZZY_OBJS = fuzzycom.o  fuzzydef.o  fuzzylhs.o  fuzzymod.o  fuzzypsr.o \
		 fuzzyrhs.o  fuzzyutl.o cfdef.o

X_OBJS = xclips.o xclipstext.o xedit.o xmain.o xmenu.o xmenu_exec.o \
         xmenu_file.o xmenu_opt.o xmenu_watch.o xmenu_wind.o

ALL_OBJS = $(OBJS) $(FUZZY_OBJS) $(X_OBJS)


.c.o :
	$(CC) $(CFLAGS) -DWINDOW_INTERFACE=1 $(INCLUDES) $(DEBUG) $<

fz_xclips : $(ALL_OBJS) 
	$(CC) -o fz_xclips $(ALL_OBJS) $(LIBS)

# DO NOT DELETE

agenda.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
agenda.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
agenda.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
agenda.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
agenda.o: fuzzylv.h dffnxfun.h extnfunc.h crstrtgy.h engine.h memalloc.h
agenda.o: modulutl.h reteutil.h retract.h router.h prntutil.h rulebsc.h
agenda.o: strngrtr.h sysdep.h watch.h cfdef.h
analysis.o: setup.h constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h
analysis.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h scanner.h
analysis.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
analysis.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
analysis.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h generate.h
analysis.o: router.h prntutil.h cstrnchk.h cstrnutl.h cstrnops.h rulecstr.h
analysis.o: modulutl.h analysis.h globldef.h
argacces.o: setup.h extnfunc.h userdata.h router.h prntutil.h cstrnchk.h
argacces.o: insfun.h evaluatn.h constant.h moduldef.h object.h constrct.h
argacces.o: scanner.h pprint.h constrnt.h expressn.h exprnops.h multifld.h
argacces.o: symbol.h fuzzyval.h tmpltdef.h factbld.h pattern.h match.h
argacces.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
argacces.o: fuzzylv.h dffnxfun.h argacces.h
bload.o: setup.h memalloc.h exprnpsr.h argacces.h symbol.h fuzzyval.h
bload.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
bload.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
bload.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
bload.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
bload.o: prntutil.h bsave.h cstrnbin.h utility.h bload.h exprnbin.h sysdep.h
bload.o: symblbin.h
bmathfun.o: setup.h exprnpsr.h argacces.h symbol.h fuzzyval.h tmpltdef.h
bmathfun.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
bmathfun.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
bmathfun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
bmathfun.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
bmathfun.o: prntutil.h bmathfun.h
bsave.o: setup.h memalloc.h exprnpsr.h argacces.h symbol.h fuzzyval.h
bsave.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
bsave.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
bsave.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
bsave.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
bsave.o: prntutil.h cstrnbin.h symblbin.h bload.h exprnbin.h sysdep.h bsave.h
cfdef.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
cfdef.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
cfdef.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h ruledef.h
cfdef.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
cfdef.o: dffnxfun.h extnfunc.h cfdef.h fuzzyutl.h fuzzypsr.h fuzzyrhs.h
cfdef.o: fuzzymod.h prntutil.h argacces.h engine.h router.h exprnpsr.h
classcom.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h argacces.h
classcom.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
classcom.o: constant.h userdata.h constrct.h moduldef.h scanner.h pprint.h
classcom.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
classcom.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
classcom.o: dffnxfun.h extnfunc.h classfun.h object.h classini.h modulutl.h
classcom.o: msgcom.h router.h prntutil.h classcom.h
classexm.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
classexm.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
classexm.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
classexm.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
classexm.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h
classexm.o: object.h classini.h memalloc.h insfun.h msgcom.h msgfun.h
classexm.o: msgpass.h router.h prntutil.h strngrtr.h classexm.h
classfun.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h classcom.h
classfun.o: classini.h memalloc.h constant.h constrct.h moduldef.h userdata.h
classfun.o: scanner.h pprint.h cstrccom.h cstrcpsr.h evaluatn.h inscom.h
classfun.o: object.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
classfun.o: fuzzyval.h tmpltdef.h factbld.h pattern.h match.h network.h
classfun.o: ruledef.h agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h
classfun.o: extnfunc.h insfun.h insmngr.h modulutl.h msgfun.h msgpass.h
classfun.o: router.h prntutil.h utility.h classfun.h
classinf.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
classinf.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
classinf.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
classinf.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
classinf.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h classexm.h
classinf.o: classfun.h object.h classini.h memalloc.h insfun.h msgfun.h
classinf.o: msgpass.h prntutil.h classinf.h
classini.o: setup.h classcom.h classexm.h classfun.h object.h constrct.h
classini.o: moduldef.h userdata.h scanner.h pprint.h constrnt.h expressn.h
classini.o: exprnops.h multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h
classini.o: constant.h factbld.h pattern.h match.h network.h ruledef.h
classini.o: cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h
classini.o: extnfunc.h classinf.h classpsr.h memalloc.h cstrcpsr.h inscom.h
classini.o: modulpsr.h modulutl.h msgcom.h watch.h defins.h insquery.h
classini.o: objbin.h objcmp.h objrtbld.h classini.h
classpsr.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h classcom.h
classpsr.o: classfun.h object.h constrct.h moduldef.h userdata.h scanner.h
classpsr.o: pprint.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
classpsr.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h pattern.h
classpsr.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
classpsr.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h cstrcpsr.h
classpsr.o: inherpsr.h modulpsr.h modulutl.h msgpsr.h clsltpsr.h router.h
classpsr.o: prntutil.h classpsr.h
clsltpsr.o: setup.h classfun.h object.h constrct.h moduldef.h userdata.h
clsltpsr.o: scanner.h pprint.h constrnt.h expressn.h exprnops.h multifld.h
clsltpsr.o: symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h
clsltpsr.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
clsltpsr.o: reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h
clsltpsr.o: cstrnchk.h cstrnpsr.h cstrnutl.h default.h insfun.h prntutil.h
clsltpsr.o: router.h clsltpsr.h
commline.o: setup.h constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h
commline.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h scanner.h
commline.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
commline.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
commline.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h argacces.h
commline.o: router.h prntutil.h strngrtr.h prcdrfun.h prcdrpsr.h utility.h
commline.o: filecom.h cstrcpsr.h commline.h
conscomp.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
conscomp.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
conscomp.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
conscomp.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
conscomp.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h argacces.h
conscomp.o: cstrncmp.h router.h prntutil.h utility.h modulcmp.h dffnxcmp.h
conscomp.o: tmpltcmp.h globlcmp.h genrccmp.h genrcfun.h object.h objcmp.h
conscomp.o: conscomp.h symblcmp.h
constrct.o: setup.h constant.h memalloc.h router.h prntutil.h scanner.h
constrct.o: pprint.h watch.h prcdrfun.h prcdrpsr.h argacces.h symbol.h
constrct.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h userdata.h
constrct.o: constrct.h moduldef.h constrnt.h factbld.h pattern.h match.h
constrct.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
constrct.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h exprnpsr.h utility.h
constrct.o: commline.h
constrnt.o: setup.h constant.h memalloc.h router.h prntutil.h extnfunc.h
constrnt.o: userdata.h scanner.h pprint.h multifld.h constrnt.h argacces.h
constrnt.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
constrnt.o: constrct.h moduldef.h factbld.h pattern.h match.h network.h
constrnt.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h
constrnt.o: dffnxfun.h
crstrtgy.o: setup.h constant.h pattern.h match.h network.h ruledef.h
crstrtgy.o: cstrccom.h agenda.h reorder.h reteutil.h argacces.h symbol.h
crstrtgy.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h userdata.h
crstrtgy.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
crstrtgy.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h crstrtgy.h
cstrcbin.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h moduldef.h
cstrcbin.o: cstrcbin.h constrct.h userdata.h scanner.h pprint.h
cstrccom.o: setup.h constant.h memalloc.h moduldef.h argacces.h symbol.h
cstrccom.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h userdata.h
cstrccom.o: constrct.h scanner.h pprint.h constrnt.h factbld.h pattern.h
cstrccom.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
cstrccom.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h modulutl.h
cstrccom.o: router.h prntutil.h utility.h commline.h bload.h exprnbin.h
cstrccom.o: sysdep.h symblbin.h cstrcpsr.h
cstrcpsr.o: setup.h router.h prntutil.h watch.h constrct.h moduldef.h
cstrcpsr.o: userdata.h scanner.h pprint.h prcdrpsr.h exprnpsr.h modulutl.h
cstrcpsr.o: modulpsr.h utility.h cstrcpsr.h evaluatn.h constant.h
cstrnbin.o: setup.h constant.h memalloc.h router.h prntutil.h bload.h
cstrnbin.o: exprnbin.h sysdep.h symblbin.h bsave.h cstrnbin.h
cstrnchk.o: setup.h router.h prntutil.h multifld.h extnfunc.h userdata.h
cstrnchk.o: cstrnutl.h cstrnchk.h
cstrncmp.o: setup.h constant.h memalloc.h router.h prntutil.h conscomp.h
cstrncmp.o: symblcmp.h cstrncmp.h
cstrnops.o: setup.h constant.h memalloc.h router.h prntutil.h extnfunc.h
cstrnops.o: userdata.h scanner.h pprint.h multifld.h constrnt.h cstrnchk.h
cstrnops.o: cstrnutl.h cstrnops.h
cstrnpsr.o: setup.h constant.h memalloc.h router.h prntutil.h scanner.h
cstrnpsr.o: pprint.h cstrnutl.h cstrnchk.h cstrnpsr.h tmpltdef.h expressn.h
cstrnpsr.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h constrnt.h
cstrnpsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
cstrnpsr.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
cstrnpsr.o: extnfunc.h modulutl.h
cstrnutl.o: setup.h constant.h memalloc.h router.h prntutil.h extnfunc.h
cstrnutl.o: userdata.h scanner.h pprint.h multifld.h argacces.h symbol.h
cstrnutl.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constrct.h
cstrnutl.o: moduldef.h constrnt.h factbld.h pattern.h match.h network.h
cstrnutl.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h
cstrnutl.o: dffnxfun.h cstrnutl.h
default.o: setup.h constant.h tmpltdef.h expressn.h exprnops.h evaluatn.h
default.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
default.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
default.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
default.o: extnfunc.h cstrnchk.h inscom.h object.h symbol.h fuzzyval.h
default.o: exprnpsr.h router.h prntutil.h cstrnutl.h default.h
defins.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h dfinsbin.h
defins.o: dfinscmp.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
defins.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
defins.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
defins.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
defins.o: fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h object.h
defins.o: memalloc.h cstrcpsr.h insfun.h inspsr.h modulpsr.h router.h
defins.o: prntutil.h utility.h defins.h
developr.o: setup.h router.h prntutil.h argacces.h symbol.h fuzzyval.h
developr.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
developr.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
developr.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
developr.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
developr.o: modulutl.h facthsh.h classcom.h classfun.h object.h objrtmch.h
developr.o: developr.h
dffctbin.o: setup.h memalloc.h dffctdef.h symbol.h fuzzyval.h tmpltdef.h
dffctbin.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
dffctbin.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
dffctbin.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
dffctbin.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h bload.h
dffctbin.o: exprnbin.h sysdep.h symblbin.h bsave.h dffctbin.h modulbin.h
dffctbin.o: cstrcbin.h
dffctbsc.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
dffctbsc.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
dffctbsc.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
dffctbsc.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
dffctbsc.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h
dffctbsc.o: prntutil.h factrhs.h cstrcpsr.h dffctpsr.h dffctdef.h dffctbin.h
dffctbsc.o: modulbin.h cstrcbin.h dffctcmp.h dffctbsc.h
dffctcmp.o: setup.h conscomp.h symblcmp.h dffctdef.h symbol.h fuzzyval.h
dffctcmp.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
dffctcmp.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
dffctcmp.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
dffctcmp.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
dffctcmp.o: dffctcmp.h
dffctdef.o: setup.h memalloc.h dffctpsr.h dffctbsc.h bload.h exprnbin.h
dffctdef.o: sysdep.h symblbin.h dffctbin.h modulbin.h cstrcbin.h constrct.h
dffctdef.o: moduldef.h userdata.h scanner.h pprint.h dffctcmp.h dffctdef.h
dffctdef.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
dffctdef.o: constant.h constrnt.h factbld.h pattern.h match.h network.h
dffctdef.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
dffctdef.o: fuzzylv.h dffnxfun.h extnfunc.h
dffctpsr.o: setup.h memalloc.h router.h prntutil.h cstrcpsr.h evaluatn.h
dffctpsr.o: constant.h userdata.h factrhs.h bload.h exprnbin.h sysdep.h
dffctpsr.o: symblbin.h dffctdef.h symbol.h fuzzyval.h tmpltdef.h expressn.h
dffctpsr.o: exprnops.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
dffctpsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
dffctpsr.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
dffctpsr.o: extnfunc.h dffctbsc.h dffctpsr.h
dffnxbin.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h memalloc.h
dffnxbin.o: cstrcbin.h constrct.h moduldef.h userdata.h scanner.h pprint.h
dffnxbin.o: modulbin.h dffnxbin.h dffnxfun.h
dffnxcmp.o: setup.h conscomp.h symblcmp.h dffnxcmp.h dffnxfun.h
dffnxexe.o: setup.h constrct.h moduldef.h userdata.h scanner.h pprint.h
dffnxexe.o: prcdrfun.h prccode.h expressn.h exprnops.h evaluatn.h constant.h
dffnxexe.o: symbol.h fuzzyval.h tmpltdef.h constrnt.h factbld.h pattern.h
dffnxexe.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
dffnxexe.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h proflfun.h
dffnxexe.o: router.h prntutil.h utility.h watch.h dffnxexe.h
dffnxfun.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h dffnxbin.h
dffnxfun.o: dffnxfun.h dffnxcmp.h constrct.h moduldef.h userdata.h scanner.h
dffnxfun.o: pprint.h cstrcpsr.h evaluatn.h constant.h dffnxpsr.h modulpsr.h
dffnxfun.o: extnfunc.h dffnxexe.h watch.h argacces.h symbol.h fuzzyval.h
dffnxfun.o: tmpltdef.h expressn.h exprnops.h constrnt.h factbld.h pattern.h
dffnxfun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
dffnxfun.o: factmngr.h multifld.h fuzzylv.h memalloc.h router.h prntutil.h
dffnxpsr.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h network.h
dffnxpsr.o: ruledef.h cstrccom.h agenda.h genrccom.h constrct.h moduldef.h
dffnxpsr.o: userdata.h scanner.h pprint.h evaluatn.h constant.h genrcfun.h
dffnxpsr.o: object.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
dffnxpsr.o: fuzzyval.h tmpltdef.h factbld.h pattern.h match.h reorder.h
dffnxpsr.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h cstrcpsr.h
dffnxpsr.o: exprnpsr.h prccode.h router.h prntutil.h dffnxpsr.h
dfinsbin.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h memalloc.h
dfinsbin.o: cstrcbin.h constrct.h moduldef.h userdata.h scanner.h pprint.h
dfinsbin.o: defins.h modulbin.h dfinsbin.h
dfinscmp.o: setup.h conscomp.h symblcmp.h defins.h dfinscmp.h
drive.o: setup.h constant.h memalloc.h reteutil.h prntutil.h router.h
drive.o: agenda.h retract.h lgcldpnd.h match.h network.h ruledef.h cstrccom.h
drive.o: pattern.h reorder.h incrrset.h drive.h expressn.h exprnops.h
edbasic.o: setup.h ed.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
edbasic.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
edbasic.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
edbasic.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
edbasic.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h
edbasic.o: prntutil.h
edmain.o: setup.h ed.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
edmain.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
edmain.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
edmain.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
edmain.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h prntutil.h
edmain.o: sysdep.h
edmisc.o: setup.h ed.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
edmisc.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
edmisc.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
edmisc.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
edmisc.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h prntutil.h
edmisc.o: cstrcpsr.h
edstruct.o: setup.h ed.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
edstruct.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
edstruct.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
edstruct.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
edstruct.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h
edstruct.o: prntutil.h
edterm.o: setup.h ed.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
edterm.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
edterm.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
edterm.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
edterm.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h prntutil.h
emathfun.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
emathfun.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
emathfun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
emathfun.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
emathfun.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h prntutil.h
emathfun.o: emathfun.h
engine.o: setup.h agenda.h argacces.h symbol.h fuzzyval.h tmpltdef.h
engine.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
engine.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
engine.o: match.h network.h ruledef.h cstrccom.h reorder.h factmngr.h
engine.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h inscom.h object.h
engine.o: memalloc.h modulutl.h prccode.h prcdrfun.h proflfun.h reteutil.h
engine.o: retract.h router.h prntutil.h ruledlt.h sysdep.h utility.h watch.h
engine.o: engine.h cfdef.h fuzzyutl.h
evaluatn.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
evaluatn.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
evaluatn.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
evaluatn.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
evaluatn.o: fuzzylv.h dffnxfun.h extnfunc.h fuzzyrhs.h fuzzymod.h fuzzypsr.h
evaluatn.o: commline.h memalloc.h router.h prntutil.h prcdrfun.h exprnpsr.h
evaluatn.o: utility.h proflfun.h sysdep.h genrccom.h genrcfun.h object.h
expressn.o: setup.h memalloc.h router.h prntutil.h extnfunc.h userdata.h
expressn.o: exprnops.h evaluatn.h constant.h expressn.h
exprnbin.o: setup.h memalloc.h dffctdef.h symbol.h fuzzyval.h tmpltdef.h
exprnbin.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
exprnbin.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
exprnbin.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
exprnbin.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h bload.h
exprnbin.o: exprnbin.h sysdep.h symblbin.h bsave.h genrcbin.h genrcfun.h
exprnbin.o: object.h dffnxbin.h tmpltbin.h cstrcbin.h modulbin.h globlbin.h
exprnbin.o: globldef.h objbin.h insfun.h inscom.h
exprnops.o: setup.h memalloc.h router.h prntutil.h extnfunc.h userdata.h
exprnops.o: cstrnchk.h cstrnutl.h cstrnops.h exprnops.h
exprnpsr.o: setup.h constant.h router.h prntutil.h strngrtr.h scanner.h
exprnpsr.o: pprint.h memalloc.h argacces.h symbol.h fuzzyval.h tmpltdef.h
exprnpsr.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
exprnpsr.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
exprnpsr.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
exprnpsr.o: dffnxfun.h extnfunc.h cstrnchk.h exprnpsr.h modulutl.h prcdrfun.h
exprnpsr.o: genrccom.h genrcfun.h object.h
extnfunc.o: setup.h constant.h router.h prntutil.h memalloc.h evaluatn.h
extnfunc.o: userdata.h extnfunc.h
factbin.o: setup.h memalloc.h tmpltdef.h expressn.h exprnops.h evaluatn.h
factbin.o: constant.h userdata.h constrct.h moduldef.h scanner.h pprint.h
factbin.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
factbin.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
factbin.o: dffnxfun.h extnfunc.h bload.h exprnbin.h sysdep.h symblbin.h
factbin.o: bsave.h rulebin.h modulbin.h cstrcbin.h factbin.h
factbld.o: setup.h memalloc.h reteutil.h router.h prntutil.h reorder.h
factbld.o: factcmp.h pattern.h match.h network.h ruledef.h cstrccom.h
factbld.o: agenda.h factmch.h evaluatn.h constant.h userdata.h factmngr.h
factbld.o: multifld.h factbld.h factgen.h factlhs.h symbol.h fuzzyval.h
factbld.o: tmpltdef.h expressn.h exprnops.h constrct.h moduldef.h scanner.h
factbld.o: pprint.h constrnt.h fuzzylv.h dffnxfun.h extnfunc.h argacces.h
factbld.o: modulutl.h
factcmp.o: setup.h factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
factcmp.o: agenda.h reorder.h conscomp.h symblcmp.h factcmp.h tmpltdef.h
factcmp.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
factcmp.o: moduldef.h scanner.h pprint.h constrnt.h factmngr.h multifld.h
factcmp.o: fuzzylv.h dffnxfun.h extnfunc.h
factcom.o: setup.h memalloc.h exprnpsr.h factmngr.h multifld.h argacces.h
factcom.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
factcom.o: constant.h userdata.h constrct.h moduldef.h scanner.h pprint.h
factcom.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
factcom.o: cstrccom.h agenda.h reorder.h fuzzylv.h dffnxfun.h extnfunc.h
factcom.o: router.h prntutil.h factrhs.h factmch.h tmpltpsr.h tmpltutl.h
factcom.o: facthsh.h modulutl.h strngrtr.h tmpltfun.h cfdef.h fuzzyutl.h
factcom.o: bload.h exprnbin.h sysdep.h symblbin.h factcom.h
factfun.o: setup.h extnfunc.h userdata.h argacces.h symbol.h fuzzyval.h
factfun.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h constrct.h
factfun.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
factfun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
factfun.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h prntutil.h tmpltutl.h
factfun.o: factfun.h
factgen.o: setup.h constant.h memalloc.h router.h prntutil.h scanner.h
factgen.o: pprint.h exprnpsr.h constrct.h moduldef.h userdata.h network.h
factgen.o: ruledef.h cstrccom.h agenda.h reteutil.h factmch.h evaluatn.h
factgen.o: factmngr.h multifld.h factbld.h pattern.h match.h reorder.h
factgen.o: factrete.h factprt.h tmpltdef.h expressn.h exprnops.h constrnt.h
factgen.o: fuzzylv.h dffnxfun.h extnfunc.h tmpltlhs.h symbol.h fuzzyval.h
factgen.o: factgen.h
facthsh.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
facthsh.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
facthsh.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
facthsh.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
facthsh.o: fuzzylv.h dffnxfun.h extnfunc.h fuzzyutl.h memalloc.h router.h
facthsh.o: prntutil.h lgcldpnd.h facthsh.h cfdef.h
factlhs.o: setup.h router.h prntutil.h reorder.h pattern.h match.h network.h
factlhs.o: ruledef.h cstrccom.h agenda.h tmpltpsr.h tmpltdef.h expressn.h
factlhs.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
factlhs.o: scanner.h pprint.h constrnt.h factbld.h factmngr.h multifld.h
factlhs.o: fuzzylv.h dffnxfun.h extnfunc.h tmpltlhs.h symbol.h fuzzyval.h
factlhs.o: tmpltutl.h modulutl.h modulpsr.h cstrcpsr.h factlhs.h
factmch.o: setup.h memalloc.h extnfunc.h userdata.h router.h prntutil.h
factmch.o: incrrset.h reteutil.h drive.h expressn.h exprnops.h factgen.h
factmch.o: factrete.h tmpltdef.h evaluatn.h constant.h constrct.h moduldef.h
factmch.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
factmch.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
factmch.o: multifld.h fuzzylv.h dffnxfun.h factmch.h
factmngr.o: setup.h constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h
factmngr.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h scanner.h
factmngr.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
factmngr.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
factmngr.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h argacces.h
factmngr.o: router.h prntutil.h strngrtr.h reteutil.h retract.h filecom.h
factmngr.o: factfun.h factrhs.h factmch.h watch.h utility.h factbin.h
factmngr.o: facthsh.h default.h commline.h engine.h lgcldpnd.h drive.h
factmngr.o: ruledlt.h tmpltbsc.h tmpltutl.h tmpltfun.h fuzzyutl.h cfdef.h
factprt.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
factprt.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
factprt.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
factprt.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
factprt.o: fuzzylv.h dffnxfun.h extnfunc.h router.h prntutil.h factgen.h
factprt.o: factprt.h
factrete.o: setup.h memalloc.h extnfunc.h userdata.h router.h prntutil.h
factrete.o: incrrset.h reteutil.h drive.h expressn.h exprnops.h factgen.h
factrete.o: factmch.h evaluatn.h constant.h factmngr.h multifld.h factbld.h
factrete.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
factrete.o: reorder.h factrete.h fuzzyutl.h fuzzyval.h tmpltdef.h constrct.h
factrete.o: moduldef.h scanner.h pprint.h constrnt.h fuzzylv.h dffnxfun.h
factrete.o: fuzzydef.h symbol.h
factrhs.o: setup.h constant.h prntutil.h extnfunc.h userdata.h pattern.h
factrhs.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
factrhs.o: modulutl.h modulpsr.h cstrcpsr.h evaluatn.h bload.h exprnbin.h
factrhs.o: sysdep.h symblbin.h tmpltpsr.h tmpltrhs.h scanner.h pprint.h
factrhs.o: expressn.h exprnops.h tmpltdef.h constrct.h moduldef.h constrnt.h
factrhs.o: factbld.h factmngr.h multifld.h fuzzylv.h dffnxfun.h tmpltutl.h
factrhs.o: exprnpsr.h strngrtr.h router.h factrhs.h rulepsr.h fuzzyrhs.h
factrhs.o: fuzzymod.h cfdef.h
filecom.o: setup.h memalloc.h argacces.h symbol.h fuzzyval.h tmpltdef.h
filecom.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
filecom.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
filecom.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
filecom.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
filecom.o: prntutil.h strngrtr.h cstrcpsr.h utility.h commline.h prcdrfun.h
filecom.o: filecom.h bsave.h bload.h exprnbin.h sysdep.h symblbin.h
filertr.o: setup.h constant.h memalloc.h router.h prntutil.h filertr.h
fuzzycom.o: setup.h extnfunc.h userdata.h evaluatn.h constant.h router.h
fuzzycom.o: prntutil.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
fuzzycom.o: exprnops.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
fuzzycom.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
fuzzycom.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
fuzzycom.o: memalloc.h factrhs.h modulutl.h fuzzycom.h fuzzymod.h fuzzydef.h
fuzzycom.o: fuzzyrhs.h fuzzyutl.h
fuzzydef.o: setup.h fuzzydef.h fuzzycom.h scanner.h pprint.h fuzzyval.h
fuzzydef.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
fuzzydef.o: constrct.h moduldef.h constrnt.h factbld.h pattern.h match.h
fuzzydef.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
fuzzydef.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h fuzzypsr.h fuzzymod.h
fuzzylhs.o: setup.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
fuzzylhs.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
fuzzylhs.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
fuzzylhs.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
fuzzylhs.o: extnfunc.h fuzzylhs.h fuzzyrhs.h fuzzymod.h
fuzzymod.o: setup.h fuzzyrhs.h fuzzymod.h fuzzyutl.h fuzzypsr.h memalloc.h
fuzzymod.o: router.h prntutil.h
fuzzypsr.o: setup.h extnfunc.h userdata.h prntutil.h scanner.h pprint.h
fuzzypsr.o: constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
fuzzypsr.o: evaluatn.h constrct.h moduldef.h constrnt.h factbld.h pattern.h
fuzzypsr.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
fuzzypsr.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h memalloc.h modulutl.h
fuzzypsr.o: fuzzypsr.h fuzzyrhs.h fuzzymod.h fuzzyutl.h
fuzzyrhs.o: setup.h fuzzyrhs.h fuzzymod.h fuzzyval.h tmpltdef.h expressn.h
fuzzyrhs.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
fuzzyrhs.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
fuzzyrhs.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
fuzzyrhs.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h fuzzyutl.h fuzzypsr.h
fuzzyrhs.o: symbol.h exprnpsr.h memalloc.h prntutil.h router.h
fuzzyutl.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
fuzzyutl.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
fuzzyutl.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
fuzzyutl.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
fuzzyutl.o: fuzzylv.h dffnxfun.h extnfunc.h fuzzydef.h fuzzyutl.h fuzzyrhs.h
fuzzyutl.o: fuzzymod.h fuzzypsr.h reteutil.h memalloc.h engine.h router.h
fuzzyutl.o: prntutil.h argacces.h cfdef.h
generate.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
generate.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
generate.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
generate.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
generate.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h exprnpsr.h argacces.h
generate.o: router.h prntutil.h generate.h globlpsr.h
genrcbin.o: setup.h constant.h memalloc.h bload.h exprnbin.h sysdep.h
genrcbin.o: symblbin.h bsave.h cstrcbin.h constrct.h moduldef.h userdata.h
genrcbin.o: scanner.h pprint.h objbin.h genrccom.h evaluatn.h genrcfun.h
genrcbin.o: object.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
genrcbin.o: fuzzyval.h tmpltdef.h factbld.h pattern.h match.h network.h
genrcbin.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h
genrcbin.o: dffnxfun.h extnfunc.h modulbin.h genrcbin.h router.h prntutil.h
genrccmp.o: setup.h network.h ruledef.h cstrccom.h agenda.h genrccom.h
genrccmp.o: constrct.h moduldef.h userdata.h scanner.h pprint.h evaluatn.h
genrccmp.o: constant.h genrcfun.h object.h constrnt.h expressn.h exprnops.h
genrccmp.o: multifld.h symbol.h fuzzyval.h tmpltdef.h factbld.h pattern.h
genrccmp.o: match.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
genrccmp.o: conscomp.h symblcmp.h objcmp.h genrccmp.h
genrccom.o: setup.h network.h ruledef.h cstrccom.h agenda.h bload.h
genrccom.o: exprnbin.h sysdep.h symblbin.h genrcbin.h genrcfun.h object.h
genrccom.o: constrct.h moduldef.h userdata.h scanner.h pprint.h constrnt.h
genrccom.o: expressn.h exprnops.h multifld.h symbol.h fuzzyval.h tmpltdef.h
genrccom.o: evaluatn.h constant.h factbld.h pattern.h match.h reorder.h
genrccom.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h genrccmp.h genrcpsr.h
genrccom.o: classcom.h inscom.h watch.h argacces.h memalloc.h cstrcpsr.h
genrccom.o: genrcexe.h modulpsr.h router.h prntutil.h genrccom.h
genrcexe.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
genrcexe.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
genrcexe.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
genrcexe.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
genrcexe.o: agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
genrcexe.o: insfun.h argacces.h genrccom.h genrcfun.h prcdrfun.h prccode.h
genrcexe.o: proflfun.h router.h prntutil.h utility.h genrcexe.h
genrcfun.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h classcom.h
genrcfun.o: classfun.h object.h constrct.h moduldef.h userdata.h scanner.h
genrcfun.o: pprint.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
genrcfun.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h pattern.h
genrcfun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
genrcfun.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h argacces.h memalloc.h
genrcfun.o: cstrcpsr.h genrccom.h genrcfun.h genrcexe.h prccode.h router.h
genrcfun.o: prntutil.h
genrcpsr.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h dffnxfun.h
genrcpsr.o: classfun.h object.h constrct.h moduldef.h userdata.h scanner.h
genrcpsr.o: pprint.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
genrcpsr.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h pattern.h
genrcpsr.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
genrcpsr.o: factmngr.h fuzzylv.h extnfunc.h classcom.h memalloc.h cstrcpsr.h
genrcpsr.o: exprnpsr.h genrccom.h genrcfun.h immthpsr.h modulutl.h prcdrpsr.h
genrcpsr.o: prccode.h router.h prntutil.h genrcpsr.h
globlbin.o: setup.h memalloc.h multifld.h globldef.h bload.h exprnbin.h
globlbin.o: sysdep.h symblbin.h bsave.h moduldef.h globlbsc.h evaluatn.h
globlbin.o: constant.h userdata.h globlbin.h modulbin.h cstrcbin.h constrct.h
globlbin.o: scanner.h pprint.h
globlbsc.o: setup.h constrct.h moduldef.h userdata.h scanner.h pprint.h
globlbsc.o: extnfunc.h watch.h globlcom.h globldef.h globlbin.h modulbin.h
globlbsc.o: cstrcbin.h globlcmp.h globlbsc.h evaluatn.h constant.h
globlcmp.o: setup.h conscomp.h symblcmp.h globldef.h globlcmp.h
globlcom.o: setup.h extnfunc.h userdata.h argacces.h symbol.h fuzzyval.h
globlcom.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h constrct.h
globlcom.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
globlcom.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
globlcom.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h prntutil.h router.h
globlcom.o: globldef.h globlcom.h
globldef.o: setup.h memalloc.h modulpsr.h multifld.h router.h prntutil.h
globldef.o: strngrtr.h modulutl.h globlbsc.h evaluatn.h constant.h userdata.h
globldef.o: globlpsr.h globlcom.h utility.h commline.h bload.h exprnbin.h
globldef.o: sysdep.h symblbin.h globlbin.h modulbin.h cstrcbin.h constrct.h
globldef.o: moduldef.h scanner.h pprint.h globldef.h globlcmp.h
globlpsr.o: setup.h pprint.h router.h prntutil.h memalloc.h scanner.h
globlpsr.o: evaluatn.h constant.h userdata.h exprnpsr.h constrct.h moduldef.h
globlpsr.o: multifld.h watch.h modulutl.h modulpsr.h cstrcpsr.h globldef.h
globlpsr.o: globlbsc.h bload.h exprnbin.h sysdep.h symblbin.h globlpsr.h
immthpsr.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
immthpsr.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
immthpsr.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
immthpsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
immthpsr.o: agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
immthpsr.o: memalloc.h cstrnutl.h genrcpsr.h genrcfun.h prccode.h immthpsr.h
incrrset.o: setup.h constant.h agenda.h router.h prntutil.h drive.h
incrrset.o: expressn.h exprnops.h pattern.h match.h network.h ruledef.h
incrrset.o: cstrccom.h reorder.h evaluatn.h userdata.h argacces.h symbol.h
incrrset.o: fuzzyval.h tmpltdef.h constrct.h moduldef.h scanner.h pprint.h
incrrset.o: constrnt.h factbld.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
incrrset.o: extnfunc.h incrrset.h
inherpsr.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
inherpsr.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
inherpsr.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
inherpsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
inherpsr.o: agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
inherpsr.o: memalloc.h modulutl.h router.h prntutil.h inherpsr.h
inscom.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
inscom.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
inscom.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
inscom.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
inscom.o: fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h object.h
inscom.o: classinf.h memalloc.h exprnpsr.h insfile.h insfun.h insmngr.h
inscom.o: insmoddp.h insmult.h inspsr.h msgfun.h msgpass.h router.h
inscom.o: prntutil.h strngrtr.h utility.h commline.h inscom.h
insfile.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
insfile.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
insfile.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
insfile.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
insfile.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h
insfile.o: object.h memalloc.h inscom.h insfun.h insmngr.h inspsr.h router.h
insfile.o: prntutil.h strngrtr.h symblbin.h sysdep.h insfile.h
insfun.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
insfun.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
insfun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
insfun.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
insfun.o: fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h object.h
insfun.o: memalloc.h cstrnchk.h inscom.h insmngr.h modulutl.h msgfun.h
insfun.o: msgpass.h prccode.h router.h prntutil.h utility.h drive.h
insfun.o: objrtmch.h insfun.h
insmngr.o: setup.h network.h ruledef.h cstrccom.h agenda.h drive.h expressn.h
insmngr.o: exprnops.h objrtmch.h lgcldpnd.h match.h pattern.h reorder.h
insmngr.o: classcom.h classfun.h object.h constrct.h moduldef.h userdata.h
insmngr.o: scanner.h pprint.h constrnt.h multifld.h symbol.h fuzzyval.h
insmngr.o: tmpltdef.h evaluatn.h constant.h factbld.h factmngr.h fuzzylv.h
insmngr.o: dffnxfun.h extnfunc.h memalloc.h insfun.h modulutl.h msgfun.h
insmngr.o: msgpass.h prccode.h router.h prntutil.h utility.h insmngr.h
insmngr.o: inscom.h
insmoddp.o: setup.h network.h ruledef.h cstrccom.h agenda.h objrtmch.h
insmoddp.o: argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
insmoddp.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
insmoddp.o: pprint.h constrnt.h factbld.h pattern.h match.h reorder.h
insmoddp.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h
insmoddp.o: insfun.h object.h insmngr.h inspsr.h msgfun.h msgpass.h prccode.h
insmoddp.o: router.h prntutil.h insmoddp.h
insmult.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
insmult.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
insmult.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
insmult.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
insmult.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h insfun.h object.h
insmult.o: msgfun.h msgpass.h multifun.h router.h prntutil.h insmult.h
inspsr.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
inspsr.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
inspsr.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
inspsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
inspsr.o: reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h classinf.h
inspsr.o: exprnpsr.h prntutil.h router.h inspsr.h
insquery.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
insquery.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
insquery.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
insquery.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
insquery.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h
insquery.o: object.h memalloc.h exprnpsr.h insfun.h insmngr.h insqypsr.h
insquery.o: prcdrfun.h router.h prntutil.h utility.h insquery.h
insqypsr.o: setup.h classcom.h exprnpsr.h extnfunc.h userdata.h insquery.h
insqypsr.o: prcdrpsr.h prntutil.h router.h scanner.h pprint.h strngrtr.h
insqypsr.o: insqypsr.h
iofun.o: setup.h router.h prntutil.h strngrtr.h filertr.h argacces.h symbol.h
iofun.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
iofun.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
iofun.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
iofun.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
iofun.o: memalloc.h commline.h sysdep.h utility.h iofun.h
lgcldpnd.o: setup.h memalloc.h router.h prntutil.h evaluatn.h constant.h
lgcldpnd.o: userdata.h engine.h reteutil.h pattern.h match.h network.h
lgcldpnd.o: ruledef.h cstrccom.h agenda.h reorder.h argacces.h symbol.h
lgcldpnd.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h constrct.h moduldef.h
lgcldpnd.o: scanner.h pprint.h constrnt.h factbld.h factmngr.h multifld.h
lgcldpnd.o: fuzzylv.h dffnxfun.h extnfunc.h insfun.h object.h lgcldpnd.h
main.o: setup.h sysdep.h extnfunc.h userdata.h commline.h
memalloc.o: setup.h constant.h memalloc.h router.h prntutil.h utility.h
miscfun.o: setup.h memalloc.h sysdep.h multifld.h exprnpsr.h argacces.h
miscfun.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
miscfun.o: constant.h userdata.h constrct.h moduldef.h scanner.h pprint.h
miscfun.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
miscfun.o: cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h
miscfun.o: extnfunc.h router.h prntutil.h utility.h miscfun.h
modulbin.o: setup.h memalloc.h constrct.h moduldef.h userdata.h scanner.h
modulbin.o: pprint.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h
modulbin.o: modulbin.h
modulbsc.o: setup.h constrct.h moduldef.h userdata.h scanner.h pprint.h
modulbsc.o: extnfunc.h modulbin.h prntutil.h modulcmp.h router.h argacces.h
modulbsc.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
modulbsc.o: constant.h constrnt.h factbld.h pattern.h match.h network.h
modulbsc.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
modulbsc.o: fuzzylv.h dffnxfun.h bload.h exprnbin.h sysdep.h symblbin.h
modulbsc.o: modulbsc.h
modulcmp.o: setup.h conscomp.h symblcmp.h moduldef.h modulcmp.h
moduldef.o: setup.h memalloc.h constant.h router.h prntutil.h extnfunc.h
moduldef.o: userdata.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
moduldef.o: exprnops.h evaluatn.h constrct.h moduldef.h scanner.h pprint.h
moduldef.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
moduldef.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
moduldef.o: dffnxfun.h modulpsr.h modulcmp.h modulbsc.h utility.h bload.h
moduldef.o: exprnbin.h sysdep.h symblbin.h modulbin.h
modulpsr.o: setup.h memalloc.h constant.h router.h prntutil.h extnfunc.h
modulpsr.o: userdata.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
modulpsr.o: exprnops.h evaluatn.h constrct.h moduldef.h scanner.h pprint.h
modulpsr.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
modulpsr.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
modulpsr.o: dffnxfun.h cstrcpsr.h modulutl.h utility.h bload.h exprnbin.h
modulpsr.o: sysdep.h symblbin.h modulpsr.h
modulutl.o: setup.h memalloc.h router.h prntutil.h modulpsr.h modulutl.h
msgcom.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
msgcom.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
msgcom.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
msgcom.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
msgcom.o: fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h object.h
msgcom.o: classinf.h insfun.h insmoddp.h msgfun.h msgpass.h prccode.h
msgcom.o: router.h prntutil.h bload.h exprnbin.h sysdep.h symblbin.h msgpsr.h
msgcom.o: watch.h msgcom.h
msgfun.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
msgfun.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
msgfun.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
msgfun.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
msgfun.o: reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h
msgfun.o: insfun.h prccode.h router.h prntutil.h msgfun.h msgpass.h
msgpass.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
msgpass.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
msgpass.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
msgpass.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
msgpass.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h classfun.h
msgpass.o: object.h memalloc.h exprnpsr.h insfun.h msgfun.h msgpass.h
msgpass.o: prcdrfun.h prccode.h proflfun.h router.h prntutil.h utility.h
msgpass.o: commline.h inscom.h
msgpsr.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h classcom.h
msgpsr.o: classfun.h object.h constrct.h moduldef.h userdata.h scanner.h
msgpsr.o: pprint.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
msgpsr.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h pattern.h
msgpsr.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
msgpsr.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h cstrcpsr.h
msgpsr.o: cstrnchk.h exprnpsr.h insfun.h msgfun.h msgpass.h prccode.h
msgpsr.o: router.h prntutil.h strngrtr.h msgpsr.h
multifld.o: setup.h constant.h memalloc.h evaluatn.h userdata.h scanner.h
multifld.o: pprint.h router.h prntutil.h strngrtr.h utility.h multifld.h
multifld.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h constrct.h
multifld.o: moduldef.h constrnt.h factbld.h pattern.h match.h network.h
multifld.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h
multifld.o: dffnxfun.h extnfunc.h fuzzyutl.h cfdef.h
multifun.o: setup.h memalloc.h argacces.h symbol.h fuzzyval.h tmpltdef.h
multifun.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
multifun.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
multifun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
multifun.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
multifun.o: prntutil.h multifun.h exprnpsr.h prcdrpsr.h prcdrfun.h utility.h
multifun.o: object.h
objbin.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h classcom.h
objbin.o: classfun.h object.h constrct.h moduldef.h userdata.h scanner.h
objbin.o: pprint.h constrnt.h expressn.h exprnops.h multifld.h symbol.h
objbin.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h pattern.h
objbin.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
objbin.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h classini.h memalloc.h
objbin.o: cstrcbin.h cstrnbin.h insfun.h modulbin.h msgfun.h msgpass.h
objbin.o: prntutil.h router.h objbin.h
objcmp.o: setup.h conscomp.h symblcmp.h classcom.h classfun.h object.h
objcmp.o: constrct.h moduldef.h userdata.h scanner.h pprint.h constrnt.h
objcmp.o: expressn.h exprnops.h multifld.h symbol.h fuzzyval.h tmpltdef.h
objcmp.o: evaluatn.h constant.h factbld.h pattern.h match.h network.h
objcmp.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h
objcmp.o: dffnxfun.h extnfunc.h classini.h cstrncmp.h objcmp.h
objrtbin.o: setup.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h memalloc.h
objrtbin.o: insfun.h evaluatn.h constant.h userdata.h moduldef.h object.h
objrtbin.o: constrct.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
objrtbin.o: multifld.h symbol.h fuzzyval.h tmpltdef.h factbld.h pattern.h
objrtbin.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
objrtbin.o: factmngr.h fuzzylv.h dffnxfun.h extnfunc.h objrtmch.h rulebin.h
objrtbin.o: modulbin.h cstrcbin.h objrtbin.h
objrtbld.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
objrtbld.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
objrtbld.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
objrtbld.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
objrtbld.o: agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
objrtbld.o: memalloc.h cstrnutl.h cstrnchk.h cstrnops.h drive.h exprnpsr.h
objrtbld.o: insfun.h insmngr.h reteutil.h rulepsr.h utility.h objrtmch.h
objrtbld.o: objrtgen.h objrtfnx.h router.h prntutil.h objrtcmp.h objrtbin.h
objrtbld.o: objrtbld.h
objrtcmp.o: setup.h conscomp.h symblcmp.h objrtmch.h pattern.h match.h
objrtcmp.o: network.h ruledef.h cstrccom.h agenda.h reorder.h objrtcmp.h
objrtfnx.o: setup.h classcom.h classfun.h object.h constrct.h moduldef.h
objrtfnx.o: userdata.h scanner.h pprint.h constrnt.h expressn.h exprnops.h
objrtfnx.o: multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h
objrtfnx.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
objrtfnx.o: agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
objrtfnx.o: drive.h objrtmch.h reteutil.h router.h prntutil.h objrtfnx.h
objrtgen.o: setup.h classfun.h object.h constrct.h moduldef.h userdata.h
objrtgen.o: scanner.h pprint.h constrnt.h expressn.h exprnops.h multifld.h
objrtgen.o: symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h
objrtgen.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
objrtgen.o: reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h objrtfnx.h
objrtgen.o: objrtgen.h
objrtmch.o: setup.h classfun.h object.h constrct.h moduldef.h userdata.h
objrtmch.o: scanner.h pprint.h constrnt.h expressn.h exprnops.h multifld.h
objrtmch.o: symbol.h fuzzyval.h tmpltdef.h evaluatn.h constant.h factbld.h
objrtmch.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
objrtmch.o: reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h
objrtmch.o: drive.h engine.h lgcldpnd.h incrrset.h reteutil.h ruledlt.h
objrtmch.o: retract.h router.h prntutil.h objrtfnx.h objrtmch.h insmngr.h
parsefun.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
parsefun.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
parsefun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
parsefun.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
parsefun.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h cstrcpsr.h exprnpsr.h
parsefun.o: memalloc.h prcdrpsr.h router.h prntutil.h strngrtr.h utility.h
parsefun.o: parsefun.h
pattern.o: setup.h constant.h memalloc.h match.h network.h ruledef.h
pattern.o: cstrccom.h agenda.h reteutil.h constrnt.h exprnpsr.h router.h
pattern.o: prntutil.h cstrnchk.h cstrnutl.h rulecmp.h conscomp.h symblcmp.h
pattern.o: extnfunc.h userdata.h pattern.h reorder.h tmpltdef.h expressn.h
pattern.o: exprnops.h evaluatn.h constrct.h moduldef.h scanner.h pprint.h
pattern.o: factbld.h factmngr.h multifld.h fuzzylv.h dffnxfun.h tmpltlhs.h
pattern.o: symbol.h fuzzyval.h fuzzylhs.h fuzzypsr.h
pprint.o: setup.h constant.h memalloc.h utility.h pprint.h
prccode.o: setup.h memalloc.h constant.h globlpsr.h exprnpsr.h multifld.h
prccode.o: object.h constrct.h moduldef.h userdata.h scanner.h pprint.h
prccode.o: constrnt.h expressn.h exprnops.h symbol.h fuzzyval.h tmpltdef.h
prccode.o: evaluatn.h factbld.h pattern.h match.h network.h ruledef.h
prccode.o: cstrccom.h agenda.h reorder.h factmngr.h fuzzylv.h dffnxfun.h
prccode.o: extnfunc.h prcdrpsr.h router.h prntutil.h utility.h prccode.h
prcdrfun.o: setup.h memalloc.h router.h prntutil.h argacces.h symbol.h
prcdrfun.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
prcdrfun.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
prcdrfun.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
prcdrfun.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
prcdrfun.o: extnfunc.h cstrnchk.h cstrnops.h exprnpsr.h utility.h prcdrpsr.h
prcdrfun.o: prcdrfun.h globldef.h
prcdrpsr.o: setup.h memalloc.h router.h prntutil.h argacces.h symbol.h
prcdrpsr.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
prcdrpsr.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
prcdrpsr.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
prcdrpsr.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
prcdrpsr.o: extnfunc.h cstrnchk.h cstrnops.h exprnpsr.h utility.h modulutl.h
prcdrpsr.o: cstrnutl.h prcdrpsr.h globldef.h globlpsr.h
prdctfun.o: setup.h exprnpsr.h argacces.h symbol.h fuzzyval.h tmpltdef.h
prdctfun.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
prdctfun.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
prdctfun.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
prdctfun.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h
prdctfun.o: prntutil.h prdctfun.h
prntutil.o: setup.h constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h
prntutil.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h scanner.h
prntutil.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
prntutil.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
prntutil.o: fuzzylv.h dffnxfun.h extnfunc.h utility.h argacces.h router.h
prntutil.o: prntutil.h
proflfun.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
proflfun.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
proflfun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
proflfun.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
proflfun.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h classcom.h extobj.h
proflfun.o: genrccom.h genrcfun.h object.h memalloc.h msgcom.h router.h
proflfun.o: prntutil.h sysdep.h proflfun.h
reorder.o: setup.h memalloc.h tmpltdef.h expressn.h exprnops.h evaluatn.h
reorder.o: constant.h userdata.h constrct.h moduldef.h scanner.h pprint.h
reorder.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
reorder.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
reorder.o: dffnxfun.h extnfunc.h cstrnutl.h rulelhs.h prntutil.h router.h
reteutil.o: setup.h memalloc.h router.h prntutil.h retract.h drive.h
reteutil.o: expressn.h exprnops.h incrrset.h pattern.h match.h network.h
reteutil.o: ruledef.h cstrccom.h agenda.h reorder.h moduldef.h reteutil.h
retract.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
retract.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
retract.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
retract.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
retract.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h argacces.h router.h
retract.o: prntutil.h drive.h lgcldpnd.h retract.h
router.o: setup.h constant.h memalloc.h filertr.h strngrtr.h extnfunc.h
router.o: userdata.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
router.o: exprnops.h evaluatn.h constrct.h moduldef.h scanner.h pprint.h
router.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
router.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
router.o: dffnxfun.h sysdep.h router.h prntutil.h
rulebin.o: setup.h memalloc.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h
rulebin.o: reteutil.h agenda.h engine.h rulebsc.h pattern.h match.h network.h
rulebin.o: ruledef.h cstrccom.h reorder.h moduldef.h rulebin.h modulbin.h
rulebin.o: cstrcbin.h constrct.h userdata.h scanner.h pprint.h
rulebld.o: setup.h constant.h memalloc.h router.h prntutil.h watch.h
rulebld.o: constrct.h moduldef.h userdata.h scanner.h pprint.h drive.h
rulebld.o: expressn.h exprnops.h pattern.h match.h network.h ruledef.h
rulebld.o: cstrccom.h agenda.h reorder.h reteutil.h incrrset.h rulebld.h
rulebsc.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
rulebsc.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
rulebsc.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
rulebsc.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
rulebsc.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h prntutil.h
rulebsc.o: watch.h engine.h rulebin.h modulbin.h cstrcbin.h rulecmp.h
rulebsc.o: conscomp.h symblcmp.h rulebsc.h
rulecmp.o: setup.h factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
rulecmp.o: agenda.h reorder.h reteutil.h rulecmp.h conscomp.h symblcmp.h
rulecmp.o: extnfunc.h userdata.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
rulecmp.o: evaluatn.h constant.h constrct.h moduldef.h scanner.h pprint.h
rulecmp.o: constrnt.h factmngr.h multifld.h fuzzylv.h dffnxfun.h prntutil.h
rulecom.o: setup.h constant.h memalloc.h evaluatn.h userdata.h extnfunc.h
rulecom.o: engine.h watch.h crstrtgy.h agenda.h symbol.h fuzzyval.h
rulecom.o: tmpltdef.h expressn.h exprnops.h constrct.h moduldef.h scanner.h
rulecom.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
rulecom.o: ruledef.h cstrccom.h reorder.h factmngr.h multifld.h fuzzylv.h
rulecom.o: dffnxfun.h argacces.h router.h prntutil.h reteutil.h ruledlt.h
rulecom.o: rulebin.h modulbin.h cstrcbin.h lgcldpnd.h incrrset.h rulecom.h
rulecstr.o: setup.h router.h prntutil.h reorder.h cstrnchk.h cstrnops.h
rulecstr.o: extnfunc.h userdata.h analysis.h prcdrpsr.h cstrnutl.h rulepsr.h
rulecstr.o: rulecstr.h
ruledef.o: setup.h memalloc.h engine.h pattern.h match.h network.h ruledef.h
ruledef.o: cstrccom.h agenda.h reorder.h rulebsc.h rulecom.h drive.h
ruledef.o: expressn.h exprnops.h rulepsr.h ruledlt.h bload.h exprnbin.h
ruledef.o: sysdep.h symblbin.h rulebin.h modulbin.h cstrcbin.h constrct.h
ruledef.o: moduldef.h userdata.h scanner.h pprint.h rulecmp.h conscomp.h
ruledef.o: symblcmp.h extnfunc.h
ruledlt.o: setup.h memalloc.h engine.h reteutil.h pattern.h match.h network.h
ruledlt.o: ruledef.h cstrccom.h agenda.h reorder.h drive.h expressn.h
ruledlt.o: exprnops.h retract.h constrct.h moduldef.h userdata.h scanner.h
ruledlt.o: pprint.h bload.h exprnbin.h sysdep.h symblbin.h ruledlt.h symbol.h
ruledlt.o: fuzzyval.h tmpltdef.h evaluatn.h constant.h constrnt.h factbld.h
ruledlt.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
rulelhs.o: setup.h constant.h symbol.h fuzzyval.h tmpltdef.h expressn.h
rulelhs.o: exprnops.h evaluatn.h userdata.h constrct.h moduldef.h scanner.h
rulelhs.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
rulelhs.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
rulelhs.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h argacces.h
rulelhs.o: router.h prntutil.h cstrnchk.h rulelhs.h cfdef.h
rulepsr.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
rulepsr.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
rulepsr.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
rulepsr.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
rulepsr.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h router.h prntutil.h
rulepsr.o: engine.h rulelhs.h rulebld.h cstrnchk.h cstrnops.h exprnpsr.h
rulepsr.o: analysis.h prcdrpsr.h prccode.h incrrset.h rulecstr.h watch.h
rulepsr.o: ruledlt.h cstrcpsr.h rulebsc.h lgcldpnd.h tmpltfun.h bload.h
rulepsr.o: exprnbin.h sysdep.h symblbin.h rulepsr.h
scanner.o: setup.h constant.h router.h prntutil.h symbol.h fuzzyval.h
scanner.o: tmpltdef.h expressn.h exprnops.h evaluatn.h userdata.h constrct.h
scanner.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
scanner.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
scanner.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h utility.h
scanner.o: memalloc.h
sortfun.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
sortfun.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
sortfun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
sortfun.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
sortfun.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h sysdep.h
sortfun.o: sortfun.h
strngfun.o: setup.h memalloc.h extnfunc.h userdata.h argacces.h symbol.h
strngfun.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
strngfun.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
strngfun.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
strngfun.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h router.h
strngfun.o: prntutil.h strngrtr.h prcdrpsr.h cstrcpsr.h exprnpsr.h drive.h
strngfun.o: strngfun.h
strngrtr.o: setup.h constant.h memalloc.h router.h prntutil.h strngrtr.h
symblbin.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
symblbin.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
symblbin.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
symblbin.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
symblbin.o: fuzzylv.h dffnxfun.h extnfunc.h tmpltbin.h cstrcbin.h modulbin.h
symblbin.o: fuzzyrhs.h fuzzymod.h fuzzypsr.h memalloc.h exprnpsr.h argacces.h
symblbin.o: router.h prntutil.h cstrnbin.h bload.h exprnbin.h sysdep.h
symblbin.o: symblbin.h bsave.h
symblcmp.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
symblcmp.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
symblcmp.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h
symblcmp.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
symblcmp.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h exprnpsr.h argacces.h
symblcmp.o: cstrncmp.h router.h prntutil.h conscomp.h symblcmp.h utility.h
symblcmp.o: tmpltcmp.h
symbol.o: setup.h symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h
symbol.o: evaluatn.h constant.h userdata.h constrct.h moduldef.h scanner.h
symbol.o: pprint.h constrnt.h factbld.h pattern.h match.h network.h ruledef.h
symbol.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
symbol.o: dffnxfun.h extnfunc.h fuzzypsr.h fuzzyrhs.h fuzzymod.h memalloc.h
symbol.o: router.h prntutil.h utility.h argacces.h
sysdep.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
sysdep.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
sysdep.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
sysdep.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
sysdep.o: fuzzylv.h dffnxfun.h extnfunc.h bmathfun.h conscomp.h symblcmp.h
sysdep.o: cstrcpsr.h emathfun.h filecom.h iofun.h memalloc.h miscfun.h
sysdep.o: multifun.h parsefun.h prccode.h prdctfun.h proflfun.h prcdrfun.h
sysdep.o: router.h prntutil.h sortfun.h strngfun.h textpro.h utility.h
sysdep.o: watch.h sysdep.h dffctdef.h genrccom.h genrcfun.h object.h
sysdep.o: globldef.h cfdef.h fuzzydef.h extobj.h developr.h
textpro.o: setup.h commline.h memalloc.h argacces.h symbol.h fuzzyval.h
textpro.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
textpro.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
textpro.o: pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
textpro.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
textpro.o: router.h prntutil.h textpro.h
tmpltbin.o: setup.h memalloc.h bload.h exprnbin.h sysdep.h symblbin.h bsave.h
tmpltbin.o: factbin.h factbld.h pattern.h match.h network.h ruledef.h
tmpltbin.o: cstrccom.h agenda.h reorder.h cstrnbin.h factmngr.h multifld.h
tmpltbin.o: tmpltpsr.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
tmpltbin.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
tmpltbin.o: fuzzylv.h dffnxfun.h extnfunc.h tmpltutl.h tmpltbin.h cstrcbin.h
tmpltbin.o: modulbin.h fuzzyval.h dffnxbin.h
tmpltbsc.o: setup.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
tmpltbsc.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
tmpltbsc.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
tmpltbsc.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
tmpltbsc.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h memalloc.h router.h
tmpltbsc.o: prntutil.h factrhs.h cstrcpsr.h tmpltpsr.h tmpltbin.h cstrcbin.h
tmpltbsc.o: modulbin.h tmpltcmp.h tmpltutl.h tmpltbsc.h
tmpltcmp.o: setup.h conscomp.h symblcmp.h factcmp.h pattern.h match.h
tmpltcmp.o: network.h ruledef.h cstrccom.h agenda.h reorder.h cstrncmp.h
tmpltcmp.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
tmpltcmp.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
tmpltcmp.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h tmpltcmp.h
tmpltcmp.o: fuzzyval.h dffnxcmp.h prntutil.h
tmpltdef.o: setup.h memalloc.h exprnops.h cstrccom.h network.h ruledef.h
tmpltdef.o: agenda.h tmpltpsr.h tmpltbsc.h tmpltutl.h tmpltfun.h router.h
tmpltdef.o: prntutil.h modulpsr.h modulutl.h cstrnchk.h bload.h exprnbin.h
tmpltdef.o: sysdep.h symblbin.h tmpltbin.h cstrcbin.h constrct.h moduldef.h
tmpltdef.o: userdata.h scanner.h pprint.h modulbin.h tmpltcmp.h tmpltdef.h
tmpltdef.o: expressn.h evaluatn.h constant.h constrnt.h factbld.h pattern.h
tmpltdef.o: match.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
tmpltdef.o: extnfunc.h fuzzypsr.h fuzzyutl.h symbol.h fuzzyval.h
tmpltfun.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
tmpltfun.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
tmpltfun.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
tmpltfun.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
tmpltfun.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h exprnpsr.h argacces.h
tmpltfun.o: router.h prntutil.h cstrnchk.h default.h commline.h factrhs.h
tmpltfun.o: modulutl.h tmpltlhs.h tmpltutl.h tmpltrhs.h tmpltfun.h
tmpltlhs.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
tmpltlhs.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
tmpltlhs.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
tmpltlhs.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
tmpltlhs.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h exprnpsr.h router.h
tmpltlhs.o: prntutil.h factrhs.h modulutl.h tmpltutl.h tmpltlhs.h fuzzylhs.h
tmpltlhs.o: fuzzypsr.h
tmpltpsr.o: setup.h constant.h memalloc.h symbol.h fuzzyval.h tmpltdef.h
tmpltpsr.o: expressn.h exprnops.h evaluatn.h userdata.h constrct.h moduldef.h
tmpltpsr.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
tmpltpsr.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
tmpltpsr.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h exprnpsr.h router.h
tmpltpsr.o: prntutil.h cstrnchk.h cstrnpsr.h cstrcpsr.h bload.h exprnbin.h
tmpltpsr.o: sysdep.h symblbin.h default.h watch.h cstrnutl.h tmpltbsc.h
tmpltpsr.o: tmpltpsr.h fuzzypsr.h
tmpltrhs.o: setup.h memalloc.h prntutil.h router.h tmpltfun.h tmpltdef.h
tmpltrhs.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
tmpltrhs.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h
tmpltrhs.o: match.h network.h ruledef.h cstrccom.h agenda.h reorder.h
tmpltrhs.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h factrhs.h
tmpltrhs.o: modulutl.h default.h tmpltutl.h tmpltlhs.h symbol.h fuzzyval.h
tmpltrhs.o: tmpltrhs.h fuzzyrhs.h fuzzymod.h
tmpltutl.o: setup.h extnfunc.h userdata.h memalloc.h constrct.h moduldef.h
tmpltutl.o: scanner.h pprint.h router.h prntutil.h argacces.h symbol.h
tmpltutl.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h
tmpltutl.o: constrnt.h factbld.h pattern.h match.h network.h ruledef.h
tmpltutl.o: cstrccom.h agenda.h reorder.h factmngr.h multifld.h fuzzylv.h
tmpltutl.o: dffnxfun.h cstrnchk.h tmpltfun.h tmpltpsr.h modulutl.h watch.h
tmpltutl.o: tmpltbsc.h tmpltutl.h fuzzypsr.h fuzzyutl.h cfdef.h
userdata.o: setup.h userdata.h
utility.o: setup.h evaluatn.h constant.h userdata.h facthsh.h memalloc.h
utility.o: multifld.h prntutil.h utility.h
watch.o: setup.h constant.h memalloc.h router.h prntutil.h argacces.h
watch.o: symbol.h fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
watch.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
watch.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h agenda.h
watch.o: reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h
watch.o: watch.h
xclips.o: xsetup.h xclips.h xmenu_wind.h xclipstext.h setup.h agenda.h
xclips.o: bmathfun.h classcom.h commline.h crstrtgy.h symbol.h fuzzyval.h
xclips.o: tmpltdef.h expressn.h exprnops.h evaluatn.h constant.h userdata.h
xclips.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
xclips.o: pattern.h match.h network.h ruledef.h cstrccom.h reorder.h
xclips.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h defins.h
xclips.o: dffctdef.h engine.h exprnpsr.h facthsh.h filecom.h genrccom.h
xclips.o: genrcfun.h object.h globlcom.h globldef.h incrrset.h inscom.h
xclips.o: insfun.h router.h prntutil.h utility.h
xclipstext.o: setup.h commline.h evaluatn.h constant.h userdata.h filertr.h
xclipstext.o: router.h prntutil.h xclips.h xedit.h xmenu_file.h xmenu_exec.h
xclipstext.o: xmenu_wind.h xclipstext.h
xedit.o: setup.h clips.h argacces.h symbol.h fuzzyval.h tmpltdef.h expressn.h
xedit.o: exprnops.h evaluatn.h constant.h userdata.h constrct.h moduldef.h
xedit.o: scanner.h pprint.h constrnt.h factbld.h pattern.h match.h network.h
xedit.o: ruledef.h cstrccom.h agenda.h reorder.h factmngr.h multifld.h
xedit.o: fuzzylv.h dffnxfun.h extnfunc.h memalloc.h cstrcpsr.h filecom.h
xedit.o: router.h prntutil.h sysdep.h bmathfun.h exprnpsr.h utility.h watch.h
xedit.o: modulbsc.h bload.h exprnbin.h symblbin.h bsave.h rulebsc.h engine.h
xedit.o: drive.h incrrset.h rulecom.h crstrtgy.h dffctdef.h dffctbsc.h
xedit.o: tmpltbsc.h factcom.h facthsh.h globldef.h globlbsc.h globlcom.h
xedit.o: genrccom.h genrcfun.h object.h extobj.h commline.h xsetup.h xclips.h
xedit.o: xedit.h xmenu.h xmenu_file.h xclipstext.h
xmain.o: setup.h sysdep.h commline.h symbol.h fuzzyval.h tmpltdef.h
xmain.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h constrct.h
xmain.o: moduldef.h scanner.h pprint.h constrnt.h factbld.h pattern.h match.h
xmain.o: network.h ruledef.h cstrccom.h agenda.h reorder.h factmngr.h
xmain.o: multifld.h fuzzylv.h dffnxfun.h extnfunc.h xsetup.h xmain.h
xmain.o: xmenu_file.h xmenu.h xclips.h
xmenu.o: setup.h constrct.h moduldef.h userdata.h scanner.h pprint.h
xmenu.o: filecom.h xsetup.h xclips.h xmenu.h xedit.h xclipstext.h
xmenu.o: xmenu_wind.h xmenu_exec.h xmenu_file.h xmenu_watch.h xmenu_opt.h
xmenu_exec.o: xsetup.h xclipstext.h xmenu_exec.h xmenu.h setup.h router.h
xmenu_exec.o: prntutil.h factmngr.h multifld.h commline.h
xmenu_file.o: setup.h constant.h commline.h router.h prntutil.h symbol.h
xmenu_file.o: fuzzyval.h tmpltdef.h expressn.h exprnops.h evaluatn.h
xmenu_file.o: userdata.h constrct.h moduldef.h scanner.h pprint.h constrnt.h
xmenu_file.o: factbld.h pattern.h match.h network.h ruledef.h cstrccom.h
xmenu_file.o: agenda.h reorder.h factmngr.h multifld.h fuzzylv.h dffnxfun.h
xmenu_file.o: extnfunc.h xsetup.h xclips.h xclipstext.h xmenu.h xmenu_file.h
xmenu_opt.o: xsetup.h xclips.h xmenu.h xmenu_opt.h xclipstext.h setup.h
xmenu_opt.o: engine.h crstrtgy.h agenda.h symbol.h fuzzyval.h tmpltdef.h
xmenu_opt.o: expressn.h exprnops.h evaluatn.h constant.h userdata.h
xmenu_opt.o: constrct.h moduldef.h scanner.h pprint.h constrnt.h factbld.h
xmenu_opt.o: pattern.h match.h network.h ruledef.h cstrccom.h reorder.h
xmenu_opt.o: factmngr.h multifld.h fuzzylv.h dffnxfun.h extnfunc.h commline.h
xmenu_opt.o: router.h prntutil.h globlcom.h facthsh.h exprnpsr.h bmathfun.h
xmenu_opt.o: incrrset.h
xmenu_watch.o: xsetup.h xclips.h setup.h constant.h watch.h xmenu.h
xmenu_watch.o: xmenu_watch.h
xmenu_wind.o: setup.h agenda.h globldef.h genrcfun.h object.h constrct.h
xmenu_wind.o: moduldef.h userdata.h scanner.h pprint.h constrnt.h expressn.h
xmenu_wind.o: exprnops.h multifld.h symbol.h fuzzyval.h tmpltdef.h evaluatn.h
xmenu_wind.o: constant.h factbld.h pattern.h match.h network.h ruledef.h
xmenu_wind.o: cstrccom.h reorder.h factmngr.h fuzzylv.h dffnxfun.h extnfunc.h
xmenu_wind.o: defins.h classcom.h commline.h dffctdef.h engine.h genrccom.h
xmenu_wind.o: insfun.h msgcom.h router.h prntutil.h rulebsc.h tmpltbsc.h
xmenu_wind.o: xsetup.h xmenu_wind.h xclips.h xmenu_file.h xclipstext.h
