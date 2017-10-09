#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
#TARGETは出力の名前です
#BUILDは、オブジェクトファイルと中間ファイルが配置されるディレクトリです
#SOURCESは、ソースコードを含むディレクトリのリストです
#DATAはデータファイルを含むディレクトリのリストです
#INCLUDESはヘッダファイルを含むディレクトリのリストです
#
#NO_SMDH：何も設定されていない場合、SMDHファイルは生成されません。
#APP_TITLEは、SMDHファイルに保存されているアプリの名前です（オプション）。
#APP_DESCRIPTIONは、SMDHファイルに保存されているアプリの説明です（オプション）。
#APP_AUTHORは、SMDHファイルに格納されているアプリの作者です（オプション）
#ICONは、プロジェクトフォルダを基準にしたアイコン（.png）のファイル名です。
#設定されていない場合、以下のいずれかを（この順序で）使用しようとします。
# - <プロジェクト名> .png
# - icon.png
# - <libctru folder> /default_icon.png
#---------------------------------------------------------------------------------
TARGET		:=	TJAplayer_on_3DS
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	include
ROMFS		:=	romfs
ICON        :=  icon.png
#BANNER_IMAGE := banner.png
APP_TITLE	:= TJAplayer on 3DS
APP_DESCRIPTION	:= TJAplayer on 3DS
APP_AUTHOR	:= Togetoge
APP_UNIQUE_ID := 0xb765

#---------------------------------------------------------------------------------
# コード生成のオプション
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	:=	-g -Wall -O2 -mword-relocations \
			-fomit-frame-pointer -ffast-math -ffunction-sections\
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lsfil -lpng -ljpeg -lz -lsf2d -lcitro3d -lctru -lm -lsftd -lfreetype -lvorbisidec

#---------------------------------------------------------------------------------
# ライブラリを含むディレクトリのリストです
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB) $(PORTLIBS)


#---------------------------------------------------------------------------------
# 追加する必要がない限り、この時点を過ぎて何かを編集する必要はありません
# 異なるファイル拡張子の規則
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(OUTPUT).smdh $(TARGET).elf $(TARGET)-strip.elf $(TARGET).cia $(TARGET).3ds
#---------------------------------------------------------------------------------
$(TARGET)-strip.elf: $(BUILD)
	@$(STRIP) $(TARGET).elf -o $(TARGET)-strip.elf
#---------------------------------------------------------------------------------
cci: $(TARGET)-strip.elf
	@makerom -f cci -rsf resources/$(TARGET).rsf -target d -exefslogo -elf $(TARGET)-strip.elf -o $(TARGET).3ds
	@echo "built ... sfil_sample.3ds"
#---------------------------------------------------------------------------------
cia: $(TARGET)-strip.elf
	@makerom -f cia -o $(TARGET).cia -elf $(TARGET)-strip.elf -rsf resources/$(TARGET).rsf -exefslogo -target t
	@echo "built ... sfil_sample.cia"
#---------------------------------------------------------------------------------
send: $(BUILD)
	@3dslink $(TARGET).3dsx
#---------------------------------------------------------------------------------
run: $(BUILD)
	@citra $(TARGET).3dsx

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# メインターゲット
#---------------------------------------------------------------------------------
ifeq ($(strip $(NO_SMDH)),)
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(OUTPUT).smdh
else
$(OUTPUT).3dsx	:	$(OUTPUT).elf
endif

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# バイナリデータとして使用する各拡張子に対して、このようなルールが必要です
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)
#---------------------------------------------------------------------------------
%.ttf.o	:	%.ttf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)
#---------------------------------------------------------------------------------
%.jpeg.o:	%.jpeg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

%.png.o	:	%.ogg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

# 警告：これはこれを行う正しい方法ではありません！ TODO：そうだよ！
#---------------------------------------------------------------------------------
%.vsh.o	:	%.vsh
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@python $(AEMSTRO)/aemstro_as.py $< ../$(notdir $<).shbin
	@bin2s ../$(notdir $<).shbin | $(PREFIX)as -o $@
	@echo "extern const u8" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(notdir $<).shbin | tr . _)`.h
	@echo "extern const u8" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(notdir $<).shbin | tr . _)`.h
	@echo "extern const u32" `(echo $(notdir $<).shbin | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(notdir $<).shbin | tr . _)`.h
	@rm ../$(notdir $<).shbin

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
