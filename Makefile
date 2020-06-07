run: build
	./sorted-diff base.txt branch.txt additions.txt deletions.txt

build: sorted_diff.c
	gcc -o sorted-diff sorted_diff.c

test: build
	cd tests && ./test.sh
