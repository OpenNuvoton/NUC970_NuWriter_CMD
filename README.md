# NuWriter command tool for NUC970 family processors
The Nu-writer Command Tool is a linux console application consisting of functions 
to access storage(eg. DRAM,NAND,SPI,eMMC) in a NUC970 family processors

## NuWriter command tool Installation Steps
ubuntu : 
```
sudo apt-get install libusb-1.0-dev
```
debian : 
```
sudo apt-get install libusb-1.0-0-dev
```

compiler :
```
./configure --prefix=$PWD/install
make
make install
```
## NuWriter command tool examples

## SDRAM mode examples
show supported ddr model:
```
./nuwriter -d show
```
Download:
```
./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/970image
```
Downalod & run:
```
./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/970image -n
```
Download dtb(device tree):
```
./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x1E00000 -w $(IMAGE_PATH)/nuc970-evb.dtb
./nuwriter -m sdram -d NUC972DF62Y.ini -a 0x8000 -w $IMAGE_PATH/970image -n -i 0x1E00000
```
## NAND mode examples
Burn u-boot-spl.bin to NAND:
```
./nuwriter -m nand -d NUC972DF62Y.ini -t uboot -a 0x200 -w $IMAGE_PATH/nand_uboot_spl.bin -v
```
Burn u-boot.bin to NAND:
```
./nuwriter -m nand -d NUC972DF62Y.ini -t data -a 0x100000 -w $IMAGE_PATH/nand_uboot.bin -v
```
Burn env.txt to NAND:
```
./nuwriter -m nand -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/nand_env.txt -v
```
Burn linux to NAND:
```
./nuwriter -m nand -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/970uimage -v
```
Burn Pack image to NAND:
```
./nuwriter -m nand -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/nand_pack.bin
```
Erase NAND (chip erase):
```
./nuwriter -m nand -d NUC972DF62Y.ini -e 0xffffffff
```
Erase NAND 10th block to 30th block:
```
./nuwriter -m nand -d NUC972DF62Y.ini -a 10 -e 20
```
Read NAND 10th block to 30th block:
```
./nuwriter -m nand -d NUC972DF62Y.ini -a 10 -e 1 -r $IMAGE_PATH/test.bin
```
## SPI mode examples
Burn u-boot.bin to SPI:
```
./nuwriter -m spi -d NUC972DF62Y.ini -t uboot -a 0xE00000 -w $IMAGE_PATH/spi_uboot.bin -v
```
Burn env.txt to SPI:
```
./nuwriter -m spi -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/spi_env.txt -v
```
Burn linux to SPI:
```
./nuwriter -m spi -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/970uimage -v
```
Burn Pack image to SPI:
```
./nuwriter -m spi -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/spi_pack.bin
```
Erase SPI (chip erase):
```
./nuwriter -m spi -d NUC972DF62Y.ini -e 0xffffffff
```
Erase SPI 10th block to 30th block:
```
./nuwriter -m spi -d NUC972DF62Y.ini -a 10 -e 20
```
Read SPI 10th block to 30th block:
```
./nuwriter -m spi -d NUC972DF62Y.ini -a 10 -e 20 -r $IMAGE_PATH/test.bin
```
## eMMC mode examples

Burn u-boot.bin to eMMC:
```
./nuwriter -m emmc -d NUC972DF62Y.ini -t uboot -a 0xE00000 -w $IMAGE_PATH/emmc_uboot.bin -v
```
Burn env.txt to eMMC:
```
./nuwriter -m emmc -d NUC972DF62Y.ini -t env -a 0x80000 -w $IMAGE_PATH/emmc_env.txt -v
```
Burn linux to eMMC:
```
./nuwriter -m emmc -d NUC972DF62Y.ini -t data -a 0x200000 -w $IMAGE_PATH/970uimage -v
```
Burn Pack image to eMMC
```
./nuwriter -m emmc -d NUC972DF62Y.ini -t pack -w $IMAGE_PATH/emmc_pack.bin
```
Erase eMMC 10th block to 30th block:
```
./nuwriter -m emmc -d NUC972DF62Y.ini -a 10 -e 20
```
Read eMMC 10th block to 30th block:
```
./nuwriter -m emmc -d NUC972DF62Y.ini -a 10 -e 1 -r $IMAGE_PATH/test.bin
```
**Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved**


