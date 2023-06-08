SUBDIRS = prozubilib

bin_PROGRAMS = nprozubi
nprozubi_SOURCES = \
									 Xray.c Xray.h\
									 PlanLecheniya.c PlanLecheniya.h\
									 PlanLecheniyaEdit.c PlanLecheniyaEdit.h\
									 CasesList.c CasesList.h\
									 CaseEdit.c CaseEdit.h\
									 PatientsList.c PatientsList.h\
									 PatientEdit.c PatientEdit.h\
									 TextUTF8Handler.c TextUTF8Handler.h\
									 InfoPannel.c InfoPannel.h\
									 PriceList.c PriceList.h\
									 NomenklaruraList.c NomenklaruraList.h\
									 error.c error.h\
									 input.c input.h\
									 main.c

CDK_LIBS = -lcdkw -lncursesw -ltinfow

nprozubi_CFLAGS =
nprozubi_LDADD = $(CDK_LIBS) -ltiff -lm -lcurl -lsqlite3 prozubilib/kdata2/cYandexDisk/*.o prozubilib/kdata2/*.o prozubilib/*.o

