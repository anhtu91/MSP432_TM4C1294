################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Devices/%.obj: ../Devices/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/Middlewares" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/Devices" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/Hardware" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/FATFS/Target" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/FATFS/App" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/Middlewares/Third_Party/FatFs/src" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/Middlewares/Third_Party/FatFs/src/option" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2/FATFS" --include_path="C:/Users/ATN/Documents/Code Compose Studio workspace/MSP432_[not_done]_SDCard_ConvertFrom_STM32_2" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --advice:power=none --define=__MSP432P401R__ --define=DeviceFamily_MSP432P401x -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Devices/$(basename $(<F)).d_raw" --obj_directory="Devices" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

