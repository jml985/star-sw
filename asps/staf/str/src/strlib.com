$ @STR_REF:STR_DEFINE
$ OPEN/READ TT: TT:
$ READ/prompt="Compile STRBYTE     (Y/N N=default)?" TT: STRBYTE
$ READ/prompt="Compile STRBYTEF    (Y/N N=default)?" TT: STRBYTEF
$ READ/prompt="Compile STRLIB      (Y/N N=default)?" TT: STRLIB
$ READ/prompt="Compile STRLIB_VMS  (Y/N N=default)?" TT: STRLIB_VMS
$ READ/prompt="Compile STRMTH      (Y/N N=default)?" TT: STRMTH
$ READ/prompt="Compile STRPLOT     (Y/N N=default)?" TT: STRPLOT
$ READ/prompt="Compile STR_NEXTARG (Y/N N=default)?" TT: STR_NEXTARG
$ READ/prompt="Compile STRTERM_VMS (Y/N N=default)?" TT: STRTERM_VMS
$ READ/prompt="Non-DEBUG version   (Y/N Y=default)?" TT: NONDEBUG
$ READ/prompt="    DEBUG version   (Y/N Y=default)?" TT: DEBUG
$ !
$ IF NONDEBUG .EQS. "N" THEN GOTO NO_NONDEBUG
$ IF NONDEBUG .EQS. "n" THEN GOTO NO_NONDEBUG
$ !
$ !
$  IF STRBYTE .EQS. "y" THEN STRBYTE="Y"
$  IF STRBYTE .NES. "Y" THEN GOTO NO_STRBYTE
$   WRITE SYS$OUTPUT "Compiling STRBYTE, w/o debugger"
$   CC/OPT/LIST/OBJ=STRBYTE STRBYTE_VMS
$  NO_STRBYTE:
$ !
$  IF STRBYTEF .EQS. "y" THEN STRBYTEF="Y"
$  IF STRBYTEF .NES. "Y" THEN GOTO NO_STRBYTEF
$   WRITE SYS$OUTPUT "Compiling STRBYTEF, w/o debugger"
$   FORT/OPT/LIST STRBYTEF
$  NO_STRBYTEF:
$ !
$  IF STRLIB .EQS. "y" THEN STRLIB="Y"
$  IF STRLIB .NES. "Y" THEN GOTO NO_STRLIB
$   WRITE SYS$OUTPUT "Compiling STRLIB, w/o debugger"
$   FORT/OPT/LIST STRLIB
$  NO_STRLIB:
$ !
$  IF STRLIB_VMS .EQS. "y" THEN STRLIB_VMS="Y"
$  IF STRLIB_VMS .NES. "Y" THEN GOTO NO_STRLIB_VMS
$   WRITE SYS$OUTPUT "Compiling STRLIB_VMS, w/o debugger"
$   FORT/OPT/LIST STRLIB_VMS
$  NO_STRLIB_VMS:
$ !
$  IF STRMTH .EQS. "y" THEN STRMTH="Y"
$  IF STRMTH .NES. "Y" THEN GOTO NO_STRMTH
$   WRITE SYS$OUTPUT "Compiling STRMTH, w/o debugger, optimized"
$   FORT/OPT/LIST STRMTH
$  NO_STRMTH:
$ !
$  IF STRPLOT .EQS. "y" THEN STRPLOT="Y"
$  IF STRPLOT .NES. "Y" THEN GOTO NO_STRPLOT
$   WRITE SYS$OUTPUT "Compiling STRPLOT, w/o debugger, optimized"
$   FORT/OPT/LIST STRPLOT
$  NO_STRPLOT:
$ !
$  IF STR_NEXTARG .EQS. "y" THEN STR_NEXTARG="Y"
$  IF STR_NEXTARG .NES. "Y" THEN GOTO NO_STR_NEXTARG
$   WRITE SYS$OUTPUT "Compiling STR_NEXTARG, w/o debugger, optimized"
$   FORT/OPT/LIST STR_NEXTARG
$  NO_STR_NEXTARG:
$ !
$  IF STRTERM_VMS .EQS. "y" THEN STRTERM_VMS="Y"
$  IF STRTERM_VMS .NES. "Y" THEN GOTO NO_STRTERM_VMS
$   WRITE SYS$OUTPUT "Compiling STRTERM_VMS, w/o debugger, optimized"
$   FORT/OPT/LIST STRTERM_VMS
$  NO_STRTERM_VMS:
$ !
$  LIBR/CREATE STRLIB STRLIB,STRLIB_VMS,STRMTH,STRPLOT,STR_NEXTARG,STRTERM_VMS
$  LIBR/CREATE STRLIBC STRBYTE
$  LIBR/CREATE STRLIBF STRBYTEF
$ NO_NONDEBUG:
$ !
$ IF DEBUG .EQS. "N" THEN GOTO NO_DEBUG
$ IF DEBUG .EQS. "n" THEN GOTO NO_DEBUG
$ !
$  IF STRBYTE .EQS. "y" THEN STRBYTE_DBG="Y"
$  IF STRBYTE .NES. "Y" THEN GOTO NO_STRBYTE_DBG
$   WRITE SYS$OUTPUT "Compiling STRBYTE, with debugger"
$   CC/NOOPT/LIST/DEBUG/OBJ=STRBYTE_DBG STRBYTE_VMS
$  NO_STRBYTE_DBG:
$ !
$  IF STRBYTEF .EQS. "y" THEN STRBYTEF_DBG="Y"
$  IF STRBYTEF .NES. "Y" THEN GOTO NO_STRBYTEF_DBG
$   WRITE SYS$OUTPUT "Compiling STRBYTEF, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRBYTEF_DBG STRBYTEF
$  NO_STRBYTEF_DBG:
$ !
$  IF STRLIB .EQS. "y" THEN STRLIB_DBG="Y"
$  IF STRLIB .NES. "Y" THEN GOTO NO_STRLIB_DBG
$   WRITE SYS$OUTPUT "Compiling STRLIB, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRLIB_DBG STRLIB
$  NO_STRLIB_DBG:
$ !
$  IF STRLIB_VMS .EQS. "y" THEN STRLIB_VMS_DBG="Y"
$  IF STRLIB_VMS .NES. "Y" THEN GOTO NO_STRLIB_VMS_DBG
$   WRITE SYS$OUTPUT "Compiling STRLIB_VMS, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRLIB_VMS_DBG STRLIB_VMS
$  NO_STRLIB_VMS_DBG:
$ !
$  IF STRMTH .EQS. "y" THEN STRMTH_DBG="Y"
$  IF STRMTH .NES. "Y" THEN GOTO NO_STRMTH_DBG
$   WRITE SYS$OUTPUT "Compiling STRMTH, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRMTH_DBG STRMTH
$  NO_STRMTH_DBG:
$ !
$  IF STRPLOT .EQS. "y" THEN STRPLOT_DBG="Y"
$  IF STRPLOT .NES. "Y" THEN GOTO NO_STRPLOT_DBG
$   WRITE SYS$OUTPUT "Compiling STRPLOT, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRPLOT_DBG STRPLOT
$  NO_STRPLOT_DBG:
$ !
$  IF STR_NEXTARG .EQS. "y" THEN STR_NEXTARG_DBG="Y"
$  IF STR_NEXTARG .NES. "Y" THEN GOTO NO_STR_NEXTARG_DBG
$   WRITE SYS$OUTPUT "Compiling STR_NEXTARG, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STR_NEXTARG_DBG STR_NEXTARG
$  NO_STR_NEXTARG_DBG:
$ !
$  IF STRTERM_VMS .EQS. "y" THEN STRTERM_VMS_DBG="Y"
$  IF STRTERM_VMS .NES. "Y" THEN GOTO NO_STRTERM_VMS_DBG
$   WRITE SYS$OUTPUT "Compiling STRTERM_VMS, with debugger"
$   FORT/NOOPT/LIST/DEBUG/OBJ=STRTERM_VMS_DBG STRTERM_VMS
$  NO_STRTERM_VMS_DBG:
$ !
$  LIBR/CREATE STRLIB_DBG STRLIB_DBG,STRLIB_VMS_DBG,STRMTH_DBG,STRPLOT_DBG,STRTERM_VMS_DBG,STR_NEXTARG_DBG
$  LIBR/CREATE STRLIBC_DBG STRBYTE_DBG
$  LIBR/CREATE STRLIBF_DBG STRBYTEF_DBG
$ NO_DEBUG:
$ PURGE/KEEP=1 *.LIS,*.OBJ,*.OLB
$ !
$ EXIT
