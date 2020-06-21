run: build
	./sorted-diff normal base.txt branch.txt additions.txt deletions.txt

build: sorted-diff.c
	gcc -o sorted-diff sorted-diff.c

test: build
	cd tests && ./test.sh
