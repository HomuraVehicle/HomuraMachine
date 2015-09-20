#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../Common/hmLib/hmLib/cdeque.c ../../../Common/hmLib/hmLib/coordinates.cpp ../../../Common/hmLib/hmLib/cqueue.c ../../../Common/hmLib/hmLib/cstack.c ../../../Common/hmLib/hmLib/cstring.c ../../../Common/hmLib/hmLib/random.cpp ../../../Common/homuraLib_v2/old/hmrVCom1.c ../../../Common/homuraLib_v2/old/hmrVComData.c ../../../Common/homuraLib_v2/old/hmrVIC1.c ../../../Common/homuraLib_v2/old/hmrVMC1.c ../../../Common/homuraLib_v2/machine/device/kk10.cpp ../../../Common/XC32Lib/clock.cpp ../../../Common/XC32Lib/interrupt.cpp ../../../Common/XC32Lib/sfr/i2c1.cpp ../../../Common/XC32Lib/sfr/i2c2.cpp ../../../Common/XC32Lib/sfr/i2c3.cpp ../../../Common/XC32Lib/sfr/i2c4.cpp ../../../Common/XC32Lib/sfr/i2c5.cpp ../../../Common/XC32Lib/sfr/rtcc.cpp ../../../Common/XC32Lib/sfr/spi1.cpp ../../../Common/XC32Lib/sfr/spi2.cpp ../../../Common/XC32Lib/sfr/spi3.cpp ../../../Common/XC32Lib/sfr/spi4.cpp ../../../Common/XC32Lib/sfr/timer1.cpp ../../../Common/XC32Lib/sfr/timer2.cpp ../../../Common/XC32Lib/sfr/timer3.cpp ../../../Common/XC32Lib/sfr/timer4.cpp ../../../Common/XC32Lib/sfr/timer5.cpp ../../../Common/XC32Lib/sfr/timer6.cpp ../../../Common/XC32Lib/sfr/timer7.cpp ../../../Common/XC32Lib/sfr/timer8.cpp ../../../Common/XC32Lib/sfr/timer9.cpp ../../../Common/XC32Lib/sfr/uart1.cpp ../../../Common/XC32Lib/sfr/uart2.cpp ../../../Common/XC32Lib/sfr/uart3.cpp ../../../Common/XC32Lib/sfr/uart4.cpp ../../../Common/XC32Lib/sfr/uart5.cpp ../../../Common/XC32Lib/sfr/uart6.cpp ../Mihara/Battery.cpp ../Mihara/CO2.cpp ../Mihara/Camera.cpp ../Mihara/Com.cpp ../Mihara/Courier.cpp ../Mihara/Data.cpp ../Mihara/DeviceManage.cpp ../Mihara/DeviceManageMessage.cpp ../Mihara/Drive.cpp ../Mihara/GPS.cpp ../Mihara/Inertial.cpp ../Mihara/Message.cpp ../Mihara/Mihara.cpp ../Mihara/Service.cpp ../Mihara/Thermo.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/530692284/cdeque.o ${OBJECTDIR}/_ext/530692284/coordinates.o ${OBJECTDIR}/_ext/530692284/cqueue.o ${OBJECTDIR}/_ext/530692284/cstack.o ${OBJECTDIR}/_ext/530692284/cstring.o ${OBJECTDIR}/_ext/530692284/random.o ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o ${OBJECTDIR}/_ext/1089583841/hmrVComData.o ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o ${OBJECTDIR}/_ext/6272988/kk10.o ${OBJECTDIR}/_ext/1923432922/clock.o ${OBJECTDIR}/_ext/1923432922/interrupt.o ${OBJECTDIR}/_ext/939895530/i2c1.o ${OBJECTDIR}/_ext/939895530/i2c2.o ${OBJECTDIR}/_ext/939895530/i2c3.o ${OBJECTDIR}/_ext/939895530/i2c4.o ${OBJECTDIR}/_ext/939895530/i2c5.o ${OBJECTDIR}/_ext/939895530/rtcc.o ${OBJECTDIR}/_ext/939895530/spi1.o ${OBJECTDIR}/_ext/939895530/spi2.o ${OBJECTDIR}/_ext/939895530/spi3.o ${OBJECTDIR}/_ext/939895530/spi4.o ${OBJECTDIR}/_ext/939895530/timer1.o ${OBJECTDIR}/_ext/939895530/timer2.o ${OBJECTDIR}/_ext/939895530/timer3.o ${OBJECTDIR}/_ext/939895530/timer4.o ${OBJECTDIR}/_ext/939895530/timer5.o ${OBJECTDIR}/_ext/939895530/timer6.o ${OBJECTDIR}/_ext/939895530/timer7.o ${OBJECTDIR}/_ext/939895530/timer8.o ${OBJECTDIR}/_ext/939895530/timer9.o ${OBJECTDIR}/_ext/939895530/uart1.o ${OBJECTDIR}/_ext/939895530/uart2.o ${OBJECTDIR}/_ext/939895530/uart3.o ${OBJECTDIR}/_ext/939895530/uart4.o ${OBJECTDIR}/_ext/939895530/uart5.o ${OBJECTDIR}/_ext/939895530/uart6.o ${OBJECTDIR}/_ext/1906020397/Battery.o ${OBJECTDIR}/_ext/1906020397/CO2.o ${OBJECTDIR}/_ext/1906020397/Camera.o ${OBJECTDIR}/_ext/1906020397/Com.o ${OBJECTDIR}/_ext/1906020397/Courier.o ${OBJECTDIR}/_ext/1906020397/Data.o ${OBJECTDIR}/_ext/1906020397/DeviceManage.o ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o ${OBJECTDIR}/_ext/1906020397/Drive.o ${OBJECTDIR}/_ext/1906020397/GPS.o ${OBJECTDIR}/_ext/1906020397/Inertial.o ${OBJECTDIR}/_ext/1906020397/Message.o ${OBJECTDIR}/_ext/1906020397/Mihara.o ${OBJECTDIR}/_ext/1906020397/Service.o ${OBJECTDIR}/_ext/1906020397/Thermo.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/530692284/cdeque.o.d ${OBJECTDIR}/_ext/530692284/coordinates.o.d ${OBJECTDIR}/_ext/530692284/cqueue.o.d ${OBJECTDIR}/_ext/530692284/cstack.o.d ${OBJECTDIR}/_ext/530692284/cstring.o.d ${OBJECTDIR}/_ext/530692284/random.o.d ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d ${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d ${OBJECTDIR}/_ext/6272988/kk10.o.d ${OBJECTDIR}/_ext/1923432922/clock.o.d ${OBJECTDIR}/_ext/1923432922/interrupt.o.d ${OBJECTDIR}/_ext/939895530/i2c1.o.d ${OBJECTDIR}/_ext/939895530/i2c2.o.d ${OBJECTDIR}/_ext/939895530/i2c3.o.d ${OBJECTDIR}/_ext/939895530/i2c4.o.d ${OBJECTDIR}/_ext/939895530/i2c5.o.d ${OBJECTDIR}/_ext/939895530/rtcc.o.d ${OBJECTDIR}/_ext/939895530/spi1.o.d ${OBJECTDIR}/_ext/939895530/spi2.o.d ${OBJECTDIR}/_ext/939895530/spi3.o.d ${OBJECTDIR}/_ext/939895530/spi4.o.d ${OBJECTDIR}/_ext/939895530/timer1.o.d ${OBJECTDIR}/_ext/939895530/timer2.o.d ${OBJECTDIR}/_ext/939895530/timer3.o.d ${OBJECTDIR}/_ext/939895530/timer4.o.d ${OBJECTDIR}/_ext/939895530/timer5.o.d ${OBJECTDIR}/_ext/939895530/timer6.o.d ${OBJECTDIR}/_ext/939895530/timer7.o.d ${OBJECTDIR}/_ext/939895530/timer8.o.d ${OBJECTDIR}/_ext/939895530/timer9.o.d ${OBJECTDIR}/_ext/939895530/uart1.o.d ${OBJECTDIR}/_ext/939895530/uart2.o.d ${OBJECTDIR}/_ext/939895530/uart3.o.d ${OBJECTDIR}/_ext/939895530/uart4.o.d ${OBJECTDIR}/_ext/939895530/uart5.o.d ${OBJECTDIR}/_ext/939895530/uart6.o.d ${OBJECTDIR}/_ext/1906020397/Battery.o.d ${OBJECTDIR}/_ext/1906020397/CO2.o.d ${OBJECTDIR}/_ext/1906020397/Camera.o.d ${OBJECTDIR}/_ext/1906020397/Com.o.d ${OBJECTDIR}/_ext/1906020397/Courier.o.d ${OBJECTDIR}/_ext/1906020397/Data.o.d ${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d ${OBJECTDIR}/_ext/1906020397/Drive.o.d ${OBJECTDIR}/_ext/1906020397/GPS.o.d ${OBJECTDIR}/_ext/1906020397/Inertial.o.d ${OBJECTDIR}/_ext/1906020397/Message.o.d ${OBJECTDIR}/_ext/1906020397/Mihara.o.d ${OBJECTDIR}/_ext/1906020397/Service.o.d ${OBJECTDIR}/_ext/1906020397/Thermo.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/530692284/cdeque.o ${OBJECTDIR}/_ext/530692284/coordinates.o ${OBJECTDIR}/_ext/530692284/cqueue.o ${OBJECTDIR}/_ext/530692284/cstack.o ${OBJECTDIR}/_ext/530692284/cstring.o ${OBJECTDIR}/_ext/530692284/random.o ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o ${OBJECTDIR}/_ext/1089583841/hmrVComData.o ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o ${OBJECTDIR}/_ext/6272988/kk10.o ${OBJECTDIR}/_ext/1923432922/clock.o ${OBJECTDIR}/_ext/1923432922/interrupt.o ${OBJECTDIR}/_ext/939895530/i2c1.o ${OBJECTDIR}/_ext/939895530/i2c2.o ${OBJECTDIR}/_ext/939895530/i2c3.o ${OBJECTDIR}/_ext/939895530/i2c4.o ${OBJECTDIR}/_ext/939895530/i2c5.o ${OBJECTDIR}/_ext/939895530/rtcc.o ${OBJECTDIR}/_ext/939895530/spi1.o ${OBJECTDIR}/_ext/939895530/spi2.o ${OBJECTDIR}/_ext/939895530/spi3.o ${OBJECTDIR}/_ext/939895530/spi4.o ${OBJECTDIR}/_ext/939895530/timer1.o ${OBJECTDIR}/_ext/939895530/timer2.o ${OBJECTDIR}/_ext/939895530/timer3.o ${OBJECTDIR}/_ext/939895530/timer4.o ${OBJECTDIR}/_ext/939895530/timer5.o ${OBJECTDIR}/_ext/939895530/timer6.o ${OBJECTDIR}/_ext/939895530/timer7.o ${OBJECTDIR}/_ext/939895530/timer8.o ${OBJECTDIR}/_ext/939895530/timer9.o ${OBJECTDIR}/_ext/939895530/uart1.o ${OBJECTDIR}/_ext/939895530/uart2.o ${OBJECTDIR}/_ext/939895530/uart3.o ${OBJECTDIR}/_ext/939895530/uart4.o ${OBJECTDIR}/_ext/939895530/uart5.o ${OBJECTDIR}/_ext/939895530/uart6.o ${OBJECTDIR}/_ext/1906020397/Battery.o ${OBJECTDIR}/_ext/1906020397/CO2.o ${OBJECTDIR}/_ext/1906020397/Camera.o ${OBJECTDIR}/_ext/1906020397/Com.o ${OBJECTDIR}/_ext/1906020397/Courier.o ${OBJECTDIR}/_ext/1906020397/Data.o ${OBJECTDIR}/_ext/1906020397/DeviceManage.o ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o ${OBJECTDIR}/_ext/1906020397/Drive.o ${OBJECTDIR}/_ext/1906020397/GPS.o ${OBJECTDIR}/_ext/1906020397/Inertial.o ${OBJECTDIR}/_ext/1906020397/Message.o ${OBJECTDIR}/_ext/1906020397/Mihara.o ${OBJECTDIR}/_ext/1906020397/Service.o ${OBJECTDIR}/_ext/1906020397/Thermo.o

# Source Files
SOURCEFILES=../../../Common/hmLib/hmLib/cdeque.c ../../../Common/hmLib/hmLib/coordinates.cpp ../../../Common/hmLib/hmLib/cqueue.c ../../../Common/hmLib/hmLib/cstack.c ../../../Common/hmLib/hmLib/cstring.c ../../../Common/hmLib/hmLib/random.cpp ../../../Common/homuraLib_v2/old/hmrVCom1.c ../../../Common/homuraLib_v2/old/hmrVComData.c ../../../Common/homuraLib_v2/old/hmrVIC1.c ../../../Common/homuraLib_v2/old/hmrVMC1.c ../../../Common/homuraLib_v2/machine/device/kk10.cpp ../../../Common/XC32Lib/clock.cpp ../../../Common/XC32Lib/interrupt.cpp ../../../Common/XC32Lib/sfr/i2c1.cpp ../../../Common/XC32Lib/sfr/i2c2.cpp ../../../Common/XC32Lib/sfr/i2c3.cpp ../../../Common/XC32Lib/sfr/i2c4.cpp ../../../Common/XC32Lib/sfr/i2c5.cpp ../../../Common/XC32Lib/sfr/rtcc.cpp ../../../Common/XC32Lib/sfr/spi1.cpp ../../../Common/XC32Lib/sfr/spi2.cpp ../../../Common/XC32Lib/sfr/spi3.cpp ../../../Common/XC32Lib/sfr/spi4.cpp ../../../Common/XC32Lib/sfr/timer1.cpp ../../../Common/XC32Lib/sfr/timer2.cpp ../../../Common/XC32Lib/sfr/timer3.cpp ../../../Common/XC32Lib/sfr/timer4.cpp ../../../Common/XC32Lib/sfr/timer5.cpp ../../../Common/XC32Lib/sfr/timer6.cpp ../../../Common/XC32Lib/sfr/timer7.cpp ../../../Common/XC32Lib/sfr/timer8.cpp ../../../Common/XC32Lib/sfr/timer9.cpp ../../../Common/XC32Lib/sfr/uart1.cpp ../../../Common/XC32Lib/sfr/uart2.cpp ../../../Common/XC32Lib/sfr/uart3.cpp ../../../Common/XC32Lib/sfr/uart4.cpp ../../../Common/XC32Lib/sfr/uart5.cpp ../../../Common/XC32Lib/sfr/uart6.cpp ../Mihara/Battery.cpp ../Mihara/CO2.cpp ../Mihara/Camera.cpp ../Mihara/Com.cpp ../Mihara/Courier.cpp ../Mihara/Data.cpp ../Mihara/DeviceManage.cpp ../Mihara/DeviceManageMessage.cpp ../Mihara/Drive.cpp ../Mihara/GPS.cpp ../Mihara/Inertial.cpp ../Mihara/Message.cpp ../Mihara/Mihara.cpp ../Mihara/Service.cpp ../Mihara/Thermo.cpp


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MZ2048EFG100
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/530692284/cdeque.o: ../../../Common/hmLib/hmLib/cdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cdeque.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cdeque.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cdeque.o.d" -o ${OBJECTDIR}/_ext/530692284/cdeque.o ../../../Common/hmLib/hmLib/cdeque.c   
	
${OBJECTDIR}/_ext/530692284/cqueue.o: ../../../Common/hmLib/hmLib/cqueue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cqueue.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cqueue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cqueue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cqueue.o.d" -o ${OBJECTDIR}/_ext/530692284/cqueue.o ../../../Common/hmLib/hmLib/cqueue.c   
	
${OBJECTDIR}/_ext/530692284/cstack.o: ../../../Common/hmLib/hmLib/cstack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstack.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cstack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cstack.o.d" -o ${OBJECTDIR}/_ext/530692284/cstack.o ../../../Common/hmLib/hmLib/cstack.c   
	
${OBJECTDIR}/_ext/530692284/cstring.o: ../../../Common/hmLib/hmLib/cstring.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstring.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstring.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cstring.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cstring.o.d" -o ${OBJECTDIR}/_ext/530692284/cstring.o ../../../Common/hmLib/hmLib/cstring.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVCom1.o: ../../../Common/homuraLib_v2/old/hmrVCom1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o ../../../Common/homuraLib_v2/old/hmrVCom1.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVComData.o: ../../../Common/homuraLib_v2/old/hmrVComData.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVComData.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVComData.o ../../../Common/homuraLib_v2/old/hmrVComData.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVIC1.o: ../../../Common/homuraLib_v2/old/hmrVIC1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o ../../../Common/homuraLib_v2/old/hmrVIC1.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVMC1.o: ../../../Common/homuraLib_v2/old/hmrVMC1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o ../../../Common/homuraLib_v2/old/hmrVMC1.c   
	
else
${OBJECTDIR}/_ext/530692284/cdeque.o: ../../../Common/hmLib/hmLib/cdeque.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cdeque.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cdeque.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cdeque.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cdeque.o.d" -o ${OBJECTDIR}/_ext/530692284/cdeque.o ../../../Common/hmLib/hmLib/cdeque.c   
	
${OBJECTDIR}/_ext/530692284/cqueue.o: ../../../Common/hmLib/hmLib/cqueue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cqueue.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cqueue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cqueue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cqueue.o.d" -o ${OBJECTDIR}/_ext/530692284/cqueue.o ../../../Common/hmLib/hmLib/cqueue.c   
	
${OBJECTDIR}/_ext/530692284/cstack.o: ../../../Common/hmLib/hmLib/cstack.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstack.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstack.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cstack.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cstack.o.d" -o ${OBJECTDIR}/_ext/530692284/cstack.o ../../../Common/hmLib/hmLib/cstack.c   
	
${OBJECTDIR}/_ext/530692284/cstring.o: ../../../Common/hmLib/hmLib/cstring.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstring.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/cstring.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/cstring.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/cstring.o.d" -o ${OBJECTDIR}/_ext/530692284/cstring.o ../../../Common/hmLib/hmLib/cstring.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVCom1.o: ../../../Common/homuraLib_v2/old/hmrVCom1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVCom1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVCom1.o ../../../Common/homuraLib_v2/old/hmrVCom1.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVComData.o: ../../../Common/homuraLib_v2/old/hmrVComData.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVComData.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVComData.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVComData.o ../../../Common/homuraLib_v2/old/hmrVComData.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVIC1.o: ../../../Common/homuraLib_v2/old/hmrVIC1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVIC1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVIC1.o ../../../Common/homuraLib_v2/old/hmrVIC1.c   
	
${OBJECTDIR}/_ext/1089583841/hmrVMC1.o: ../../../Common/homuraLib_v2/old/hmrVMC1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1089583841" 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1089583841/hmrVMC1.o.d" -o ${OBJECTDIR}/_ext/1089583841/hmrVMC1.o ../../../Common/homuraLib_v2/old/hmrVMC1.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/530692284/coordinates.o: ../../../Common/hmLib/hmLib/coordinates.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/coordinates.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/coordinates.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/coordinates.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/coordinates.o.d" -o ${OBJECTDIR}/_ext/530692284/coordinates.o ../../../Common/hmLib/hmLib/coordinates.cpp  
	
${OBJECTDIR}/_ext/530692284/random.o: ../../../Common/hmLib/hmLib/random.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/random.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/random.o.d" -o ${OBJECTDIR}/_ext/530692284/random.o ../../../Common/hmLib/hmLib/random.cpp  
	
${OBJECTDIR}/_ext/6272988/kk10.o: ../../../Common/homuraLib_v2/machine/device/kk10.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/6272988" 
	@${RM} ${OBJECTDIR}/_ext/6272988/kk10.o.d 
	@${RM} ${OBJECTDIR}/_ext/6272988/kk10.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/6272988/kk10.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/6272988/kk10.o.d" -o ${OBJECTDIR}/_ext/6272988/kk10.o ../../../Common/homuraLib_v2/machine/device/kk10.cpp  
	
${OBJECTDIR}/_ext/1923432922/clock.o: ../../../Common/XC32Lib/clock.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1923432922" 
	@${RM} ${OBJECTDIR}/_ext/1923432922/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1923432922/clock.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1923432922/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1923432922/clock.o.d" -o ${OBJECTDIR}/_ext/1923432922/clock.o ../../../Common/XC32Lib/clock.cpp  
	
${OBJECTDIR}/_ext/1923432922/interrupt.o: ../../../Common/XC32Lib/interrupt.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1923432922" 
	@${RM} ${OBJECTDIR}/_ext/1923432922/interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1923432922/interrupt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1923432922/interrupt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1923432922/interrupt.o.d" -o ${OBJECTDIR}/_ext/1923432922/interrupt.o ../../../Common/XC32Lib/interrupt.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c1.o: ../../../Common/XC32Lib/sfr/i2c1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c1.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c1.o ../../../Common/XC32Lib/sfr/i2c1.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c2.o: ../../../Common/XC32Lib/sfr/i2c2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c2.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c2.o ../../../Common/XC32Lib/sfr/i2c2.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c3.o: ../../../Common/XC32Lib/sfr/i2c3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c3.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c3.o ../../../Common/XC32Lib/sfr/i2c3.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c4.o: ../../../Common/XC32Lib/sfr/i2c4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c4.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c4.o ../../../Common/XC32Lib/sfr/i2c4.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c5.o: ../../../Common/XC32Lib/sfr/i2c5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c5.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c5.o ../../../Common/XC32Lib/sfr/i2c5.cpp  
	
${OBJECTDIR}/_ext/939895530/rtcc.o: ../../../Common/XC32Lib/sfr/rtcc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/rtcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/rtcc.o.d" -o ${OBJECTDIR}/_ext/939895530/rtcc.o ../../../Common/XC32Lib/sfr/rtcc.cpp  
	
${OBJECTDIR}/_ext/939895530/spi1.o: ../../../Common/XC32Lib/sfr/spi1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi1.o.d" -o ${OBJECTDIR}/_ext/939895530/spi1.o ../../../Common/XC32Lib/sfr/spi1.cpp  
	
${OBJECTDIR}/_ext/939895530/spi2.o: ../../../Common/XC32Lib/sfr/spi2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi2.o.d" -o ${OBJECTDIR}/_ext/939895530/spi2.o ../../../Common/XC32Lib/sfr/spi2.cpp  
	
${OBJECTDIR}/_ext/939895530/spi3.o: ../../../Common/XC32Lib/sfr/spi3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi3.o.d" -o ${OBJECTDIR}/_ext/939895530/spi3.o ../../../Common/XC32Lib/sfr/spi3.cpp  
	
${OBJECTDIR}/_ext/939895530/spi4.o: ../../../Common/XC32Lib/sfr/spi4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi4.o.d" -o ${OBJECTDIR}/_ext/939895530/spi4.o ../../../Common/XC32Lib/sfr/spi4.cpp  
	
${OBJECTDIR}/_ext/939895530/timer1.o: ../../../Common/XC32Lib/sfr/timer1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer1.o.d" -o ${OBJECTDIR}/_ext/939895530/timer1.o ../../../Common/XC32Lib/sfr/timer1.cpp  
	
${OBJECTDIR}/_ext/939895530/timer2.o: ../../../Common/XC32Lib/sfr/timer2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer2.o.d" -o ${OBJECTDIR}/_ext/939895530/timer2.o ../../../Common/XC32Lib/sfr/timer2.cpp  
	
${OBJECTDIR}/_ext/939895530/timer3.o: ../../../Common/XC32Lib/sfr/timer3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer3.o.d" -o ${OBJECTDIR}/_ext/939895530/timer3.o ../../../Common/XC32Lib/sfr/timer3.cpp  
	
${OBJECTDIR}/_ext/939895530/timer4.o: ../../../Common/XC32Lib/sfr/timer4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer4.o.d" -o ${OBJECTDIR}/_ext/939895530/timer4.o ../../../Common/XC32Lib/sfr/timer4.cpp  
	
${OBJECTDIR}/_ext/939895530/timer5.o: ../../../Common/XC32Lib/sfr/timer5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer5.o.d" -o ${OBJECTDIR}/_ext/939895530/timer5.o ../../../Common/XC32Lib/sfr/timer5.cpp  
	
${OBJECTDIR}/_ext/939895530/timer6.o: ../../../Common/XC32Lib/sfr/timer6.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer6.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer6.o.d" -o ${OBJECTDIR}/_ext/939895530/timer6.o ../../../Common/XC32Lib/sfr/timer6.cpp  
	
${OBJECTDIR}/_ext/939895530/timer7.o: ../../../Common/XC32Lib/sfr/timer7.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer7.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer7.o.d" -o ${OBJECTDIR}/_ext/939895530/timer7.o ../../../Common/XC32Lib/sfr/timer7.cpp  
	
${OBJECTDIR}/_ext/939895530/timer8.o: ../../../Common/XC32Lib/sfr/timer8.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer8.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer8.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer8.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer8.o.d" -o ${OBJECTDIR}/_ext/939895530/timer8.o ../../../Common/XC32Lib/sfr/timer8.cpp  
	
${OBJECTDIR}/_ext/939895530/timer9.o: ../../../Common/XC32Lib/sfr/timer9.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer9.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer9.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer9.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer9.o.d" -o ${OBJECTDIR}/_ext/939895530/timer9.o ../../../Common/XC32Lib/sfr/timer9.cpp  
	
${OBJECTDIR}/_ext/939895530/uart1.o: ../../../Common/XC32Lib/sfr/uart1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart1.o.d" -o ${OBJECTDIR}/_ext/939895530/uart1.o ../../../Common/XC32Lib/sfr/uart1.cpp  
	
${OBJECTDIR}/_ext/939895530/uart2.o: ../../../Common/XC32Lib/sfr/uart2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart2.o.d" -o ${OBJECTDIR}/_ext/939895530/uart2.o ../../../Common/XC32Lib/sfr/uart2.cpp  
	
${OBJECTDIR}/_ext/939895530/uart3.o: ../../../Common/XC32Lib/sfr/uart3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart3.o.d" -o ${OBJECTDIR}/_ext/939895530/uart3.o ../../../Common/XC32Lib/sfr/uart3.cpp  
	
${OBJECTDIR}/_ext/939895530/uart4.o: ../../../Common/XC32Lib/sfr/uart4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart4.o.d" -o ${OBJECTDIR}/_ext/939895530/uart4.o ../../../Common/XC32Lib/sfr/uart4.cpp  
	
${OBJECTDIR}/_ext/939895530/uart5.o: ../../../Common/XC32Lib/sfr/uart5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart5.o.d" -o ${OBJECTDIR}/_ext/939895530/uart5.o ../../../Common/XC32Lib/sfr/uart5.cpp  
	
${OBJECTDIR}/_ext/939895530/uart6.o: ../../../Common/XC32Lib/sfr/uart6.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart6.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart6.o.d" -o ${OBJECTDIR}/_ext/939895530/uart6.o ../../../Common/XC32Lib/sfr/uart6.cpp  
	
${OBJECTDIR}/_ext/1906020397/Battery.o: ../Mihara/Battery.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Battery.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Battery.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Battery.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Battery.o.d" -o ${OBJECTDIR}/_ext/1906020397/Battery.o ../Mihara/Battery.cpp  
	
${OBJECTDIR}/_ext/1906020397/CO2.o: ../Mihara/CO2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/CO2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/CO2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/CO2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/CO2.o.d" -o ${OBJECTDIR}/_ext/1906020397/CO2.o ../Mihara/CO2.cpp  
	
${OBJECTDIR}/_ext/1906020397/Camera.o: ../Mihara/Camera.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Camera.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Camera.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Camera.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Camera.o.d" -o ${OBJECTDIR}/_ext/1906020397/Camera.o ../Mihara/Camera.cpp  
	
${OBJECTDIR}/_ext/1906020397/Com.o: ../Mihara/Com.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Com.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Com.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Com.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Com.o.d" -o ${OBJECTDIR}/_ext/1906020397/Com.o ../Mihara/Com.cpp  
	
${OBJECTDIR}/_ext/1906020397/Courier.o: ../Mihara/Courier.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Courier.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Courier.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Courier.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Courier.o.d" -o ${OBJECTDIR}/_ext/1906020397/Courier.o ../Mihara/Courier.cpp  
	
${OBJECTDIR}/_ext/1906020397/Data.o: ../Mihara/Data.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Data.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Data.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Data.o.d" -o ${OBJECTDIR}/_ext/1906020397/Data.o ../Mihara/Data.cpp  
	
${OBJECTDIR}/_ext/1906020397/DeviceManage.o: ../Mihara/DeviceManage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d" -o ${OBJECTDIR}/_ext/1906020397/DeviceManage.o ../Mihara/DeviceManage.cpp  
	
${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o: ../Mihara/DeviceManageMessage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d" -o ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o ../Mihara/DeviceManageMessage.cpp  
	
${OBJECTDIR}/_ext/1906020397/Drive.o: ../Mihara/Drive.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Drive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Drive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Drive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Drive.o.d" -o ${OBJECTDIR}/_ext/1906020397/Drive.o ../Mihara/Drive.cpp  
	
${OBJECTDIR}/_ext/1906020397/GPS.o: ../Mihara/GPS.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/GPS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/GPS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/GPS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/GPS.o.d" -o ${OBJECTDIR}/_ext/1906020397/GPS.o ../Mihara/GPS.cpp  
	
${OBJECTDIR}/_ext/1906020397/Inertial.o: ../Mihara/Inertial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Inertial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Inertial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Inertial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Inertial.o.d" -o ${OBJECTDIR}/_ext/1906020397/Inertial.o ../Mihara/Inertial.cpp  
	
${OBJECTDIR}/_ext/1906020397/Message.o: ../Mihara/Message.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Message.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Message.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Message.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Message.o.d" -o ${OBJECTDIR}/_ext/1906020397/Message.o ../Mihara/Message.cpp  
	
${OBJECTDIR}/_ext/1906020397/Mihara.o: ../Mihara/Mihara.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Mihara.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Mihara.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Mihara.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Mihara.o.d" -o ${OBJECTDIR}/_ext/1906020397/Mihara.o ../Mihara/Mihara.cpp  
	
${OBJECTDIR}/_ext/1906020397/Service.o: ../Mihara/Service.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Service.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Service.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Service.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Service.o.d" -o ${OBJECTDIR}/_ext/1906020397/Service.o ../Mihara/Service.cpp  
	
${OBJECTDIR}/_ext/1906020397/Thermo.o: ../Mihara/Thermo.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Thermo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Thermo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Thermo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Thermo.o.d" -o ${OBJECTDIR}/_ext/1906020397/Thermo.o ../Mihara/Thermo.cpp  
	
else
${OBJECTDIR}/_ext/530692284/coordinates.o: ../../../Common/hmLib/hmLib/coordinates.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/coordinates.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/coordinates.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/coordinates.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/coordinates.o.d" -o ${OBJECTDIR}/_ext/530692284/coordinates.o ../../../Common/hmLib/hmLib/coordinates.cpp  
	
${OBJECTDIR}/_ext/530692284/random.o: ../../../Common/hmLib/hmLib/random.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/530692284" 
	@${RM} ${OBJECTDIR}/_ext/530692284/random.o.d 
	@${RM} ${OBJECTDIR}/_ext/530692284/random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/530692284/random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/530692284/random.o.d" -o ${OBJECTDIR}/_ext/530692284/random.o ../../../Common/hmLib/hmLib/random.cpp  
	
${OBJECTDIR}/_ext/6272988/kk10.o: ../../../Common/homuraLib_v2/machine/device/kk10.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/6272988" 
	@${RM} ${OBJECTDIR}/_ext/6272988/kk10.o.d 
	@${RM} ${OBJECTDIR}/_ext/6272988/kk10.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/6272988/kk10.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/6272988/kk10.o.d" -o ${OBJECTDIR}/_ext/6272988/kk10.o ../../../Common/homuraLib_v2/machine/device/kk10.cpp  
	
${OBJECTDIR}/_ext/1923432922/clock.o: ../../../Common/XC32Lib/clock.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1923432922" 
	@${RM} ${OBJECTDIR}/_ext/1923432922/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1923432922/clock.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1923432922/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1923432922/clock.o.d" -o ${OBJECTDIR}/_ext/1923432922/clock.o ../../../Common/XC32Lib/clock.cpp  
	
${OBJECTDIR}/_ext/1923432922/interrupt.o: ../../../Common/XC32Lib/interrupt.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1923432922" 
	@${RM} ${OBJECTDIR}/_ext/1923432922/interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1923432922/interrupt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1923432922/interrupt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1923432922/interrupt.o.d" -o ${OBJECTDIR}/_ext/1923432922/interrupt.o ../../../Common/XC32Lib/interrupt.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c1.o: ../../../Common/XC32Lib/sfr/i2c1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c1.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c1.o ../../../Common/XC32Lib/sfr/i2c1.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c2.o: ../../../Common/XC32Lib/sfr/i2c2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c2.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c2.o ../../../Common/XC32Lib/sfr/i2c2.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c3.o: ../../../Common/XC32Lib/sfr/i2c3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c3.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c3.o ../../../Common/XC32Lib/sfr/i2c3.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c4.o: ../../../Common/XC32Lib/sfr/i2c4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c4.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c4.o ../../../Common/XC32Lib/sfr/i2c4.cpp  
	
${OBJECTDIR}/_ext/939895530/i2c5.o: ../../../Common/XC32Lib/sfr/i2c5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/i2c5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/i2c5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/i2c5.o.d" -o ${OBJECTDIR}/_ext/939895530/i2c5.o ../../../Common/XC32Lib/sfr/i2c5.cpp  
	
${OBJECTDIR}/_ext/939895530/rtcc.o: ../../../Common/XC32Lib/sfr/rtcc.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/rtcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/rtcc.o.d" -o ${OBJECTDIR}/_ext/939895530/rtcc.o ../../../Common/XC32Lib/sfr/rtcc.cpp  
	
${OBJECTDIR}/_ext/939895530/spi1.o: ../../../Common/XC32Lib/sfr/spi1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi1.o.d" -o ${OBJECTDIR}/_ext/939895530/spi1.o ../../../Common/XC32Lib/sfr/spi1.cpp  
	
${OBJECTDIR}/_ext/939895530/spi2.o: ../../../Common/XC32Lib/sfr/spi2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi2.o.d" -o ${OBJECTDIR}/_ext/939895530/spi2.o ../../../Common/XC32Lib/sfr/spi2.cpp  
	
${OBJECTDIR}/_ext/939895530/spi3.o: ../../../Common/XC32Lib/sfr/spi3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi3.o.d" -o ${OBJECTDIR}/_ext/939895530/spi3.o ../../../Common/XC32Lib/sfr/spi3.cpp  
	
${OBJECTDIR}/_ext/939895530/spi4.o: ../../../Common/XC32Lib/sfr/spi4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/spi4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/spi4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/spi4.o.d" -o ${OBJECTDIR}/_ext/939895530/spi4.o ../../../Common/XC32Lib/sfr/spi4.cpp  
	
${OBJECTDIR}/_ext/939895530/timer1.o: ../../../Common/XC32Lib/sfr/timer1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer1.o.d" -o ${OBJECTDIR}/_ext/939895530/timer1.o ../../../Common/XC32Lib/sfr/timer1.cpp  
	
${OBJECTDIR}/_ext/939895530/timer2.o: ../../../Common/XC32Lib/sfr/timer2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer2.o.d" -o ${OBJECTDIR}/_ext/939895530/timer2.o ../../../Common/XC32Lib/sfr/timer2.cpp  
	
${OBJECTDIR}/_ext/939895530/timer3.o: ../../../Common/XC32Lib/sfr/timer3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer3.o.d" -o ${OBJECTDIR}/_ext/939895530/timer3.o ../../../Common/XC32Lib/sfr/timer3.cpp  
	
${OBJECTDIR}/_ext/939895530/timer4.o: ../../../Common/XC32Lib/sfr/timer4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer4.o.d" -o ${OBJECTDIR}/_ext/939895530/timer4.o ../../../Common/XC32Lib/sfr/timer4.cpp  
	
${OBJECTDIR}/_ext/939895530/timer5.o: ../../../Common/XC32Lib/sfr/timer5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer5.o.d" -o ${OBJECTDIR}/_ext/939895530/timer5.o ../../../Common/XC32Lib/sfr/timer5.cpp  
	
${OBJECTDIR}/_ext/939895530/timer6.o: ../../../Common/XC32Lib/sfr/timer6.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer6.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer6.o.d" -o ${OBJECTDIR}/_ext/939895530/timer6.o ../../../Common/XC32Lib/sfr/timer6.cpp  
	
${OBJECTDIR}/_ext/939895530/timer7.o: ../../../Common/XC32Lib/sfr/timer7.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer7.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer7.o.d" -o ${OBJECTDIR}/_ext/939895530/timer7.o ../../../Common/XC32Lib/sfr/timer7.cpp  
	
${OBJECTDIR}/_ext/939895530/timer8.o: ../../../Common/XC32Lib/sfr/timer8.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer8.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer8.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer8.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer8.o.d" -o ${OBJECTDIR}/_ext/939895530/timer8.o ../../../Common/XC32Lib/sfr/timer8.cpp  
	
${OBJECTDIR}/_ext/939895530/timer9.o: ../../../Common/XC32Lib/sfr/timer9.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer9.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/timer9.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/timer9.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/timer9.o.d" -o ${OBJECTDIR}/_ext/939895530/timer9.o ../../../Common/XC32Lib/sfr/timer9.cpp  
	
${OBJECTDIR}/_ext/939895530/uart1.o: ../../../Common/XC32Lib/sfr/uart1.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart1.o.d" -o ${OBJECTDIR}/_ext/939895530/uart1.o ../../../Common/XC32Lib/sfr/uart1.cpp  
	
${OBJECTDIR}/_ext/939895530/uart2.o: ../../../Common/XC32Lib/sfr/uart2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart2.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart2.o.d" -o ${OBJECTDIR}/_ext/939895530/uart2.o ../../../Common/XC32Lib/sfr/uart2.cpp  
	
${OBJECTDIR}/_ext/939895530/uart3.o: ../../../Common/XC32Lib/sfr/uart3.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart3.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart3.o.d" -o ${OBJECTDIR}/_ext/939895530/uart3.o ../../../Common/XC32Lib/sfr/uart3.cpp  
	
${OBJECTDIR}/_ext/939895530/uart4.o: ../../../Common/XC32Lib/sfr/uart4.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart4.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart4.o.d" -o ${OBJECTDIR}/_ext/939895530/uart4.o ../../../Common/XC32Lib/sfr/uart4.cpp  
	
${OBJECTDIR}/_ext/939895530/uart5.o: ../../../Common/XC32Lib/sfr/uart5.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart5.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart5.o.d" -o ${OBJECTDIR}/_ext/939895530/uart5.o ../../../Common/XC32Lib/sfr/uart5.cpp  
	
${OBJECTDIR}/_ext/939895530/uart6.o: ../../../Common/XC32Lib/sfr/uart6.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/939895530" 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart6.o.d 
	@${RM} ${OBJECTDIR}/_ext/939895530/uart6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/939895530/uart6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/939895530/uart6.o.d" -o ${OBJECTDIR}/_ext/939895530/uart6.o ../../../Common/XC32Lib/sfr/uart6.cpp  
	
${OBJECTDIR}/_ext/1906020397/Battery.o: ../Mihara/Battery.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Battery.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Battery.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Battery.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Battery.o.d" -o ${OBJECTDIR}/_ext/1906020397/Battery.o ../Mihara/Battery.cpp  
	
${OBJECTDIR}/_ext/1906020397/CO2.o: ../Mihara/CO2.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/CO2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/CO2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/CO2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/CO2.o.d" -o ${OBJECTDIR}/_ext/1906020397/CO2.o ../Mihara/CO2.cpp  
	
${OBJECTDIR}/_ext/1906020397/Camera.o: ../Mihara/Camera.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Camera.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Camera.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Camera.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Camera.o.d" -o ${OBJECTDIR}/_ext/1906020397/Camera.o ../Mihara/Camera.cpp  
	
${OBJECTDIR}/_ext/1906020397/Com.o: ../Mihara/Com.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Com.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Com.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Com.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Com.o.d" -o ${OBJECTDIR}/_ext/1906020397/Com.o ../Mihara/Com.cpp  
	
${OBJECTDIR}/_ext/1906020397/Courier.o: ../Mihara/Courier.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Courier.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Courier.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Courier.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Courier.o.d" -o ${OBJECTDIR}/_ext/1906020397/Courier.o ../Mihara/Courier.cpp  
	
${OBJECTDIR}/_ext/1906020397/Data.o: ../Mihara/Data.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Data.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Data.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Data.o.d" -o ${OBJECTDIR}/_ext/1906020397/Data.o ../Mihara/Data.cpp  
	
${OBJECTDIR}/_ext/1906020397/DeviceManage.o: ../Mihara/DeviceManage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/DeviceManage.o.d" -o ${OBJECTDIR}/_ext/1906020397/DeviceManage.o ../Mihara/DeviceManage.cpp  
	
${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o: ../Mihara/DeviceManageMessage.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o.d" -o ${OBJECTDIR}/_ext/1906020397/DeviceManageMessage.o ../Mihara/DeviceManageMessage.cpp  
	
${OBJECTDIR}/_ext/1906020397/Drive.o: ../Mihara/Drive.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Drive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Drive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Drive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Drive.o.d" -o ${OBJECTDIR}/_ext/1906020397/Drive.o ../Mihara/Drive.cpp  
	
${OBJECTDIR}/_ext/1906020397/GPS.o: ../Mihara/GPS.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/GPS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/GPS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/GPS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/GPS.o.d" -o ${OBJECTDIR}/_ext/1906020397/GPS.o ../Mihara/GPS.cpp  
	
${OBJECTDIR}/_ext/1906020397/Inertial.o: ../Mihara/Inertial.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Inertial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Inertial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Inertial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Inertial.o.d" -o ${OBJECTDIR}/_ext/1906020397/Inertial.o ../Mihara/Inertial.cpp  
	
${OBJECTDIR}/_ext/1906020397/Message.o: ../Mihara/Message.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Message.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Message.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Message.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Message.o.d" -o ${OBJECTDIR}/_ext/1906020397/Message.o ../Mihara/Message.cpp  
	
${OBJECTDIR}/_ext/1906020397/Mihara.o: ../Mihara/Mihara.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Mihara.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Mihara.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Mihara.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Mihara.o.d" -o ${OBJECTDIR}/_ext/1906020397/Mihara.o ../Mihara/Mihara.cpp  
	
${OBJECTDIR}/_ext/1906020397/Service.o: ../Mihara/Service.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Service.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Service.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Service.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Service.o.d" -o ${OBJECTDIR}/_ext/1906020397/Service.o ../Mihara/Service.cpp  
	
${OBJECTDIR}/_ext/1906020397/Thermo.o: ../Mihara/Thermo.cpp  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1906020397" 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Thermo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1906020397/Thermo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1906020397/Thermo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CPPC} $(MP_EXTRA_CC_PRE)  -g -x c++ -c -mprocessor=$(MP_PROCESSOR_OPTION)  -frtti -fexceptions -fno-check-new -fenforce-eh-specs -I"../../../Common" -I"../../" -I"../../../Common/hmLib" -MMD -MF "${OBJECTDIR}/_ext/1906020397/Thermo.o.d" -o ${OBJECTDIR}/_ext/1906020397/Thermo.o ../Mihara/Thermo.cpp  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_ICD3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}           -mreserve=data@0x0:0x363   -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,--defsym=_min_heap_size=1024,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=1024,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/HomuraMachine.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
