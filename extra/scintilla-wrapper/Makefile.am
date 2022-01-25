## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libscintilla.la

INCLUDES = @GTK_CFLAGS@ -I scintilla/include -DNDEBUG -DGTK -DSCI_LEXER

libscintilla_la_SOURCES = AutoComplete.cxx KeyWords.cxx LexCLW.cxx LexGui4Cli.cxx LexMetapost.cxx LexSQL.cxx PropSet.cxx CallTip.cxx  LexAPDL.cxx  LexCPP.cxx LexHTML.cxx  LexNsis.cxx LexScriptol.cxx RESearch.cxx CellBuffer.cxx LexAU3.cxx LexCSS.cxx LexKix.cxx LexOthers.cxx LexSpecman.cxx ScintillaBase.cxx ContractionState.cxx LexAVE.cxx LexConf.cxx  LexLisp.cxx  LexPB.cxx LexTeX.cxx ScintillaGTK.cxx Document.cxx  LexAda.cxx LexCrontab.cxx LexLout.cxx  LexPOV.cxx LexVB.cxx Style.cxx DocumentAccessor.cxx LexAsm.cxx LexEScript.cxx LexLua.cxx LexPS.cxx LexVHDL.cxx StyleContext.cxx Editor.cxx LexAsn1.cxx  LexEiffel.cxx LexMMIXAL.cxx LexPascal.cxx LexVerilog.cxx UniConversion.cxx ExternalLexer.cxx  LexBaan.cxx  LexErlang.cxx LexMPT.cxx LexPerl.cxx LexYAML.cxx ViewStyle.cxx Indicator.cxx LexBash.cxx  LexForth.cxx LexMSSQL.cxx LexPython.cxx LineMarker.cxx WindowAccessor.cxx KeyMap.cxx LexBullant.cxx LexFortran.cxx LexMatlab.cxx LexRuby.cxx PlatGTK.cxx XPM.cxx scintilla-marshal.c \
AutoComplete.h CallTip.h CellBuffer.h ContractionState.h Converter.h Document.h DocumentAccessor.h Editor.h \
ExternalLexer.h Indicator.h KeyMap.h LineMarker.h RESearch.h SVector.h ScintillaBase.h Style.h StyleContext.h \
UniConversion.h ViewStyle.h XPM.h scintilla-marshal.h


libscintilla.a:
	cd scintilla/gtk && make GTK2=1
	${CP} scintilla/bin/libscintilla.a .


EXTRA_DIST = scintilla
