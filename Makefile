all: test.dsk
CC = m6809-unknown-gcc
CFLAGS = -Os -DBARE -fno-builtin -fomit-frame-pointer
AR = m6809-unknown-ar
LD = m6809-unknown-ld
AS = m6809-unknown-as
SRC = snazzy.c label.c button.c ll-coco.c font.c poplist.c hslide.c panel.c
SRC += menu.c window.c text.c joy.c keyboard.c file.c bitmap.c
OBJ = $(SRC:.c=.o)

$(OBJ): $(SRC)

snazzy.a: $(OBJ) snazzy.h ll.h test.frm
	$(AS) -o joyll.o joyll.s
	$(AS) -o graf.o graf.s
	$(AS) --defsym KERNEL -o file_ll.o file_ll.s
	$(AR) -rc snazzy.a $(OBJ) joyll.o graf.o file_ll.o
	rm -f $(OBJ)

boot.bin:
	$(AS) -o crt0_simple.o crt0_simple.s
	$(AS) -o file_ll.o file_ll.s
	$(CC) $(CFLAGS) -c boot.c
	$(CC) $(CFLAGS) -c file.c
	lwlink -s basic.link -m boot.map -o boot.bin crt0_simple.o boot.o file.o file_ll.o


kernel.bin: snazzy.a test-coco.c crt0.s graf.s joyll.s joy.c
	$(AS) -o crt0.o crt0.s
	$(CC) $(CCFLAGS) -c test-coco.c
	lwlink -s kernel.link -m kernel.map -o kernel.bin crt0.o test-coco.o \
	snazzy.a

test.dsk: kernel.bin boot.bin autoexec.bas test.frm
	decb2 mkfs test.dsk
	decb2 label test.dsk "SNAZZY GUI"
	decb2 put -l0a test.dsk autoexec.bas AUTOEXEC.BAS
	decb2 put -2b test.dsk kernel.bin KERNEL.BIN
	decb2 put -2b test.dsk boot.bin BOOT.BIN
	decb2 put -1a test.dsk test.frm TEST.FRM


test.frm: guipe/guipe guipe/test.rc
	cd guipe; ./guipe > ../test.frm

guipe/guipe:  guipe/guipe.c
	$(MAKE) -C guipe all

clean:
	$(MAKE) -C guipe clean
	rm -f *.o snazzy *~ test.h test.frm test snazzy.a kernel.bin test.map \
	test.dsk boot.bin boot.map kernel.map
	$(MAKE) -C guipe clean
