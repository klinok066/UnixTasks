.PHONY: build clean

build: clean
	@gcc lock.c -o lock
	@chmod a+x ./runme.sh
	@echo "Имя скрипта: ./lock"

clean:
	@rm -f file
	@rm -rf file.lck
	@rm -f lock
	@rm -f nohup.out
	@rm -f result.txt