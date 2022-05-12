objs = main.o Func_0.o

dep_files := $(patsubst %, .%.d, $(objs))
dep_files := $(wildcard $(dep_files))

test:$(objs)
	@gcc -o test $^		#$^表示所有的依赖文件
	
ifneq ($(dep_files),)	#如果没有就包含
include $(dep_files)
endif

#Func_0.o : Func_0.c Func_0.h

%.o:%.c
#$@表示目标文件，$<表示第一个依赖文件
	@gcc -c -o $@ $< -MD -MF .$@.d   #编译并把依赖写入.d文件

clean:
	@rm *.o test		#删除所有的.o文件和删除text文件
	
distclean:
	rm $(dep_files)
	
.PHONY: clean			#定义为假想目标，用于处理目录下有clean这个文件的情况
