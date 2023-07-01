SUBDIRS = prozubilib

bin_PROGRAMS = nprozubi
nprozubi_SOURCES = \
									 MainScreen.c MainScreen.h\
									 Xray.c Xray.h\
									 PlanLecheniya.c PlanLecheniya.h\
									 PlanLecheniyaEdit.c PlanLecheniyaEdit.h\
									 CasesList.c CasesList.h\
									 CaseEdit.c CaseEdit.h\
									 PatientsList.c PatientsList.h\
									 PatientEdit.c PatientEdit.h\
									 TextUTF8Handler.c TextUTF8Handler.h\
									 PriceList.c PriceList.h\
									 NomenklaruraList.c NomenklaruraList.h\
									 error.c error.h\
									 input.c input.h\
									 ncwidgets/src/ncwidgets.c\
									 ncwidgets/src/ncscreen.c\
									 ncwidgets/src/nclabel.c\
									 ncwidgets/src/nclist.c\
									 ncwidgets/src/ncwin.c\
									 ncwidgets/src/ncentry.c\
									 ncwidgets/src/ncbutton.c\
									 ncwidgets/src/ncselection.c\
									 ncwidgets/src/nccalendar.c\
									 main.c

CDK_LIBS = -lpanelw -lcdkw -lncursesw -ltinfow

nprozubi_CFLAGS =
nprozubi_LDADD = $(CDK_LIBS) -ltiff -lm -lcurl -lsqlite3 prozubilib/kdata2/cYandexDisk/*.o prozubilib/kdata2/*.o prozubilib/*.o

