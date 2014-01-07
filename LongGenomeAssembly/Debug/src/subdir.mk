################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Cell.cpp \
../src/Cluster.cpp \
../src/ClusterSS.cpp \
../src/HashFreq.cpp \
../src/Kmer.cpp \
../src/LongReadsOverlapNaive.cpp \
../src/LrcExactSeeds.cpp \
../src/OverlapMatrix.cpp \
../src/Read.cpp \
../src/ReadFastaFile.cpp \
../src/ReadOverlap.cpp \
../src/Smithwaterman.cpp \
../src/SpacedSeeds.cpp \
../src/ValidateClusters.cpp 

OBJS += \
./src/Cell.o \
./src/Cluster.o \
./src/ClusterSS.o \
./src/HashFreq.o \
./src/Kmer.o \
./src/LongReadsOverlapNaive.o \
./src/LrcExactSeeds.o \
./src/OverlapMatrix.o \
./src/Read.o \
./src/ReadFastaFile.o \
./src/ReadOverlap.o \
./src/Smithwaterman.o \
./src/SpacedSeeds.o \
./src/ValidateClusters.o 

CPP_DEPS += \
./src/Cell.d \
./src/Cluster.d \
./src/ClusterSS.d \
./src/HashFreq.d \
./src/Kmer.d \
./src/LongReadsOverlapNaive.d \
./src/LrcExactSeeds.d \
./src/OverlapMatrix.d \
./src/Read.d \
./src/ReadFastaFile.d \
./src/ReadOverlap.d \
./src/Smithwaterman.d \
./src/SpacedSeeds.d \
./src/ValidateClusters.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


