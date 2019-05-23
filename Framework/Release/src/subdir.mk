################################################################################
# Automatically-generated file. Do not edit!
################################################################################

TREE_TYPE = TWO_LEVEL_TREE

IDIR = ../src/auxi/LKH/INCLUDE
CFLAGS = -O3 -Wall -I$(IDIR) -D$(TREE_TYPE)

_DEPS = Delaunay.h GainType.h Genetic.h GeoConversion.h Hashing.h      \
        Heap.h LKH.h Segment.h Sequence.h BIT.h gpx.h LKH_Lib.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../src/EF_local_search_solver.cpp \
../src/EF_manager.cpp \
../src/EF_second_stage_solver.cpp \
../src/Example_STSP_LKH.cpp \
../src/SP_solution.cpp

OBJS += \
./src/EF_local_search_solver.o \
./src/EF_manager.o \
./src/EF_second_stage_solver.o \
./src/Example_STSP_LKH.o \
./src/SP_solution.o

CPP_DEPS += \
./src/EF_local_search_solver.d \
./src/EF_manager.d \
./src/EF_second_stage_solver.d \
./src/Example_STSP_LKH.d \
./src/SP_solution.d


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp $(DEPS)
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(CFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
